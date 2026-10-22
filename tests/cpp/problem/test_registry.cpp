#include "../utils.hpp"

#include <cmath>
#include <fstream>

#include "ioh.hpp"

namespace
{
    using Registry = std::map<int, std::string>;

    std::string trim(const std::string &s)
    {
        const auto first = s.find_first_not_of(" \t\r");
        const auto last = s.find_last_not_of(" \t\r,");
        return first == std::string::npos ? "" : s.substr(first, last - first + 1);
    }

    //! The {"id": "name"} object `key` in `section` of tests/data/golden/registry.json, which is pretty-printed
    //! with one entry per line and plain identifiers as names
    Registry golden_registry(const std::string &section, const std::string &key)
    {
        std::ifstream in(std::string(IOH_GOLDEN_DIR) + "/registry.json");
        Registry registry;
        bool in_section = false, in_object = false;
        for (std::string line; std::getline(in, line);)
        {
            const auto entry = trim(line);
            if (!in_section)
                in_section = entry == "\"" + section + "\": {";
            else if (!in_object)
                in_object = entry == "\"" + key + "\": {";
            else if (entry.front() == '}')
                break;
            else
            {
                // "id": "name"
                const auto id_end = entry.find('"', 1);
                const auto name_start = entry.find('"', id_end + 1) + 1;
                registry[std::stoi(entry.substr(1, id_end - 1))] =
                    entry.substr(name_start, entry.find('"', name_start) - name_start);
            }
        }
        return registry;
    }

    //! The meta names of the CEC2013 problems, which differ from their registry names, from the golden
    //! behaviour records (tests/data/golden/behaviour.json has one record per line)
    Registry golden_cec2013_meta_names()
    {
        std::ifstream in(std::string(IOH_GOLDEN_DIR) + "/behaviour.json");
        const std::string prefix = "\"CEC2013/", name_key = "\"name\": \"";
        Registry names;
        for (std::string line; std::getline(in, line);)
            if (line.compare(0, prefix.size(), prefix) == 0)
            {
                const auto name_start = line.find(name_key) + name_key.size();
                names[std::stoi(line.substr(prefix.size()))] =
                    line.substr(name_start, line.find('"', name_start) - name_start);
            }
        return names;
    }

    bool is_cec2013(const int id) { return id >= 1101 && id <= 1120; }
    bool is_cec2022(const int id) { return id >= 1001 && id <= 1012; }
    bool is_graph(const int id) { return id >= 2000 && id <= 2308; }

    /**
     * @brief Checks a problem factory against the golden registry: the same built-in {id: name} entries, and
     * every one constructs as a problem with that id and name (CEC2013: its golden meta name), reports whether
     * its dimension and instance are fixed, and evaluates.
     */
    template <class P>
    void check_factory(const std::string &section, const std::string &key, const std::function<int(int)> &n_variables,
                       const bool construct_graphs = true)
    {
        const auto &factory = ioh::problem::ProblemRegistry<P>::instance();
        const auto expected = golden_registry(section, key);
        ASSERT_FALSE(expected.empty()) << key;

        // problems registered at runtime, like the examples compiled into this binary, come after the built-ins
        auto builtins = factory.map();
        for (auto it = builtins.begin(); it != builtins.end();)
            if (expected.count(it->first) == 0)
            {
                EXPECT_GT(it->first, expected.rbegin()->first) << key << " " << it->second;
                it = builtins.erase(it);
            }
            else
                ++it;
        EXPECT_EQ(builtins, expected) << key;

        static const auto cec2013_meta_names = golden_cec2013_meta_names();
        for (const auto &[id, name] : builtins)
        {
            if (is_graph(id) && !construct_graphs)
                continue;
            const auto problem = factory.create(id, 1, n_variables(id));
            const auto meta = problem->meta_data();
            EXPECT_EQ(meta.problem_id, id) << key << " " << name;
            EXPECT_EQ(meta.name, is_cec2013(id) ? cec2013_meta_names.at(id) : name) << key << " " << id;

            if (is_cec2013(id) || is_graph(id))
            {
                EXPECT_EQ(problem->fixed_dimension(), std::optional<int>(meta.n_variables)) << key << " " << id;
                EXPECT_EQ(problem->fixed_instance(), std::optional<int>(1)) << key << " " << id;
            }
            else
            {
                EXPECT_EQ(meta.n_variables, n_variables(id)) << key << " " << id;
                EXPECT_EQ(problem->fixed_dimension(), std::nullopt) << key << " " << id;
                EXPECT_EQ(problem->fixed_instance(), std::nullopt) << key << " " << id;
            }
            EXPECT_FALSE(std::isnan((*problem)(problem->bounds().lb))) << key << " " << id;
        }
    }
} // namespace

TEST_F(BaseTest, test_registry_family_factories)
{
    using namespace ioh::problem;
    const auto real = [](const int id) { return is_cec2022(id) ? 10 : 2; }; // CEC2022: 10 and 20 only
    const auto integer = [](int) { return 16; };
    check_factory<BBOB>("problem_classes", "BBOB", real);
    check_factory<SBOX>("problem_classes", "SBOX", real);
    check_factory<PBO>("problem_classes", "PBO", integer);
    check_factory<CEC2013>("problem_classes", "CEC2013", real);
    check_factory<CEC2022>("problem_classes", "CEC2022", real);
    check_factory<submodular::GraphProblem>("problem_classes", "GRAPH", integer);
    check_factory<star_discrepancy::real::StarDiscrepancy>("problem_classes", "STAR_REAL", real);
    check_factory<star_discrepancy::integer::StarDiscrepancy>("problem_classes", "STAR_INTEGER", integer);
    check_factory<DynamicBinVal>("views", "DynamicBinVal", integer);
}

TEST_F(BaseTest, test_registry_union_factories)
{
    using namespace ioh::problem;
    check_factory<RealSingleObjective>("problem_classes", "REAL", [](const int id) { return is_cec2022(id) ? 10 : 2; });
    // the graph problems were constructed by the GRAPH factory test (constructing them loads all graph data)
    check_factory<IntegerSingleObjective>("problem_classes", "INTEGER", [](int) { return 16; }, false);
}

TEST_F(BaseTest, test_registry_suites)
{
    using namespace ioh;
    const std::map<int, std::string> real = {{1, "Real"}, {2, "BBOB"}, {3, "SBOX"}, {4, "RealStarDiscrepancy"}};
    EXPECT_EQ(suite::SuiteRegistry<problem::RealSingleObjective>::instance().map(), real);

    const std::map<int, std::string> integer = {
        {1, "Integer"}, {2, "PBO"}, {3, "Submodular"}, {4, "IntegerStarDiscrepancy"}};
    EXPECT_EQ(suite::SuiteRegistry<problem::IntegerSingleObjective>::instance().map(), integer);

    const auto bbob = suite::SuiteRegistry<problem::RealSingleObjective>::instance().create("BBOB", {1, 2}, {1}, {2});
    EXPECT_EQ(bbob->problem_ids(), std::vector<int>({1, 2}));
}

TEST_F(BaseTest, test_registry_unknown_entries_throw)
{
    const auto &factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();
    EXPECT_THROW((void)factory.create(9999, 1, 2), std::out_of_range);
    EXPECT_THROW((void)factory.create("NoSuchProblem", 1, 2), std::out_of_range);
}

namespace registry_test
{
    // Factories of these types get duplicate built-in entries; add_builtins is found by argument-dependent lookup
    struct DuplicateName
    {
    };
    struct DuplicateId
    {
    };

    void add_builtins(ioh::common::Factory<DuplicateName, int> &factory)
    {
        factory.include("A", 1, [](int) { return std::make_shared<DuplicateName>(); });
        factory.include("A", 2, [](int) { return std::make_shared<DuplicateName>(); });
    }

    void add_builtins(ioh::common::Factory<DuplicateId, int> &factory)
    {
        factory.include("A", 1, [](int) { return std::make_shared<DuplicateId>(); });
        factory.include("B", 1, [](int) { return std::make_shared<DuplicateId>(); });
    }
} // namespace registry_test

TEST_F(BaseTest, test_registry_duplicate_builtins_throw)
{
    using DuplicateNames = ioh::common::Factory<registry_test::DuplicateName, int>;
    using DuplicateIds = ioh::common::Factory<registry_test::DuplicateId, int>;
    EXPECT_THROW((void)DuplicateNames::instance(), std::logic_error);
    EXPECT_THROW((void)DuplicateIds::instance(), std::logic_error);
}

namespace registry_test
{
    class UserProblem final : public ioh::problem::RealSingleObjective
    {
    protected:
        double evaluate(const std::vector<double> &x) override { return x.at(0); }

    public:
        UserProblem(const int instance, const int n_variables) :
            ioh::problem::RealSingleObjective(ioh::problem::MetaData(
                1, instance, "RegistryTestUserProblem", n_variables, ioh::common::OptimizationType::MIN))
        {
        }
    };
} // namespace registry_test

TEST_F(BaseTest, test_registry_register_problem)
{
    using namespace ioh::problem;
    const auto &factory = ProblemRegistry<RealSingleObjective>::instance();

    // id 1 (from the meta data) belongs to Sphere, so the problem gets the next free id
    register_problem<registry_test::UserProblem, RealSingleObjective>();
    const auto map = factory.map();
    const auto entry = std::find_if(map.begin(), map.end(),
                                    [](const auto &kv) { return kv.second == "RegistryTestUserProblem"; });
    ASSERT_NE(entry, map.end());
    EXPECT_GT(entry->first, 1120);
    EXPECT_EQ(factory.create("RegistryTestUserProblem", 1, 3)->meta_data().n_variables, 3);

    register_problem<registry_test::UserProblem, RealSingleObjective>(54321, "RegistryTestUserProblemById");
    EXPECT_EQ(factory.map().at(54321), "RegistryTestUserProblemById");
    EXPECT_EQ(factory.create(54321, 2, 4)->meta_data().instance, 2);
}
