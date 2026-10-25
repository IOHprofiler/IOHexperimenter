#include "../utils.hpp"

#include <cmath>
#include <fstream>
#include <random>

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

    //! The first entry of each of the four graph problem classes
    bool is_first_of_graph_class(const int id) { return id == 2000 || id == 2100 || id == 2200 || id == 2300; }

    /**
     * @brief Checks a problem factory against the golden registry: the same {id: name} entries, and every one
     * constructs as a problem with that id and name (CEC2013: its golden meta name), reports whether its
     * dimension and instance are fixed, and evaluates.
     *
     * The union factories (REAL, INTEGER) may also hold problems registered at runtime by other tests and the
     * examples compiled into this binary, after the built-ins; of their graph entries, only the first of each
     * class is constructed (the GRAPH factory constructs all of them, and each loads its graph data).
     */
    template <class P>
    void check_factory(const std::string &section, const std::string &key, const std::function<int(int)> &n_variables,
                       const bool union_factory = false)
    {
        const auto &factory = ioh::problem::ProblemRegistry<P>::instance();
        const auto expected = golden_registry(section, key);
        ASSERT_FALSE(expected.empty()) << key;

        auto builtins = factory.map();
        if (union_factory)
        {
            for (auto it = builtins.begin(); it != builtins.end();)
            {
                if (expected.count(it->first) == 0)
                {
                    EXPECT_GT(it->first, expected.rbegin()->first) << key << " " << it->second;
                    it = builtins.erase(it);
                }
                else
                    ++it;
            }
        }
        EXPECT_EQ(builtins, expected) << key;

        static const auto cec2013_meta_names = golden_cec2013_meta_names();
        for (const auto &[id, name] : builtins)
        {
            if (union_factory && is_graph(id) && !is_first_of_graph_class(id))
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
    check_factory<RealSingleObjective>(
        "problem_classes", "REAL", [](const int id) { return is_cec2022(id) ? 10 : 2; }, true);
    check_factory<IntegerSingleObjective>("problem_classes", "INTEGER", [](int) { return 16; }, true);
}

TEST_F(BaseTest, test_registry_suites)
{
    using namespace ioh;
    // the built-in suites have ids 1-4; test_registry_register_suite adds a suite after them
    const auto builtins = [](std::map<int, std::string> map) {
        map.erase(map.upper_bound(4), map.end());
        return map;
    };
    const std::map<int, std::string> real = {{1, "Real"}, {2, "BBOB"}, {3, "SBOX"}, {4, "RealStarDiscrepancy"}};
    EXPECT_EQ(builtins(suite::SuiteRegistry<problem::RealSingleObjective>::instance().map()), real);

    const std::map<int, std::string> integer = {
        {1, "Integer"}, {2, "PBO"}, {3, "Submodular"}, {4, "IntegerStarDiscrepancy"}};
    EXPECT_EQ(builtins(suite::SuiteRegistry<problem::IntegerSingleObjective>::instance().map()), integer);

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
        // id 1 belongs to Sphere
        static constexpr int registry_id = 1;
        static constexpr const char *registry_name = "RegistryTestUserProblem";

        UserProblem(const int instance, const int n_variables) :
            ioh::problem::RealSingleObjective(ioh::problem::MetaData(registry_id, instance, registry_name,
                                                                     n_variables, ioh::common::OptimizationType::MIN))
        {
        }
    };

    //! A user problem defined by data, with two entries of fixed dimension
    class UserDataProblem final : public ioh::problem::RealSingleObjective
    {
    protected:
        double evaluate(const std::vector<double> &x) override { return x.at(0); }

    public:
        UserDataProblem(const int id, const int n_variables) :
            ioh::problem::RealSingleObjective(ioh::problem::MetaData(id, 1, "RegistryTestUserDataProblem",
                                                                     n_variables, ioh::common::OptimizationType::MIN))
        {
        }

        static std::vector<ioh::problem::RegistryEntry<UserDataProblem>> registry_entries()
        {
            return {{65001, "RegistryTestUserData2D", [](int, int) { return std::make_shared<UserDataProblem>(65001, 2); }},
                    {65002, "RegistryTestUserData3D", [](int, int) { return std::make_shared<UserDataProblem>(65002, 3); }}};
        }
    };
} // namespace registry_test

TEST_F(BaseTest, test_registry_register_problem)
{
    using namespace ioh::problem;
    const auto &factory = ProblemRegistry<RealSingleObjective>::instance();

    // id 1 (its registry_id) belongs to Sphere, so the problem gets the next free id, and reports it
    register_problem<registry_test::UserProblem, RealSingleObjective>();
    const auto map = factory.map();
    const auto entry = std::find_if(map.begin(), map.end(),
                                    [](const auto &kv) { return kv.second == "RegistryTestUserProblem"; });
    ASSERT_NE(entry, map.end());
    EXPECT_GT(entry->first, 1120);
    const auto problem = factory.create("RegistryTestUserProblem", 1, 3);
    EXPECT_EQ(problem->meta_data().problem_id, entry->first);
    EXPECT_EQ(problem->meta_data().name, "RegistryTestUserProblem");
    EXPECT_EQ(problem->meta_data().n_variables, 3);

    register_problem<registry_test::UserProblem, RealSingleObjective>(54321, "RegistryTestUserProblemById");
    EXPECT_EQ(factory.map().at(54321), "RegistryTestUserProblemById");
    const auto by_id = factory.create(54321, 2, 4);
    EXPECT_EQ(by_id->meta_data().problem_id, 54321);
    EXPECT_EQ(by_id->meta_data().name, "RegistryTestUserProblemById");
    EXPECT_EQ(by_id->meta_data().instance, 2);

    // a class that lists its entries (like the CEC2013 and graph problems)
    register_problem<registry_test::UserDataProblem, RealSingleObjective>();
    EXPECT_EQ(factory.map().at(65001), "RegistryTestUserData2D");
    EXPECT_EQ(factory.create("RegistryTestUserData3D", 5, 7)->meta_data().n_variables, 3);
    EXPECT_EQ(factory.create(65002, 5, 7)->meta_data().problem_id, 65002);
}

namespace registry_test
{
    //! Registered under the name of an earlier registration
    class ReplacingProblem final : public ioh::problem::RealSingleObjective
    {
    protected:
        double evaluate(const std::vector<double> &x) override { return -x.at(0); }

    public:
        static constexpr int registry_id = 54400;
        static constexpr const char *registry_name = "RegistryTestReplaced";

        ReplacingProblem(const int instance, const int n_variables) :
            ioh::problem::RealSingleObjective(ioh::problem::MetaData(registry_id, instance, registry_name,
                                                                     n_variables, ioh::common::OptimizationType::MIN))
        {
        }
    };
} // namespace registry_test

TEST_F(BaseTest, test_registry_register_problem_existing_name)
{
    using namespace ioh::problem;
    const auto &factory = ProblemRegistry<RealSingleObjective>::instance();
    register_problem<registry_test::UserProblem, RealSingleObjective>(54300, "RegistryTestReplaced");

    // a registered name keeps its id and gets the new class, with both overloads
    register_problem<registry_test::ReplacingProblem, RealSingleObjective>();
    EXPECT_EQ(factory.map().at(54300), "RegistryTestReplaced");
    EXPECT_EQ(factory.map().count(54400), 0u);
    const auto replaced = factory.create("RegistryTestReplaced", 1, 2);
    EXPECT_NE(std::dynamic_pointer_cast<registry_test::ReplacingProblem>(replaced), nullptr);
    EXPECT_EQ(replaced->meta_data().problem_id, 54300);

    register_problem<registry_test::UserProblem, RealSingleObjective>(54500, "RegistryTestReplaced");
    EXPECT_EQ(factory.map().count(54500), 0u);
    const auto user = factory.create(54300, 1, 2);
    EXPECT_NE(std::dynamic_pointer_cast<registry_test::UserProblem>(user), nullptr);
    EXPECT_EQ(user->meta_data().problem_id, 54300);
}

namespace registry_test
{
    //! A user suite whose registry_id belongs to the BBOB suite
    struct UserSuite final : ioh::suite::Suite<ioh::problem::RealSingleObjective>
    {
        static constexpr int registry_id = 2;
        static constexpr const char *registry_name = "RegistryTestUserSuite";

        UserSuite(const std::vector<int> &problem_ids, const std::vector<int> &instances,
                  const std::vector<int> &dimensions) :
            Suite(problem_ids, instances, dimensions, registry_name)
        {
        }
    };
} // namespace registry_test

TEST_F(BaseTest, test_registry_register_suite)
{
    using namespace ioh;
    const auto &factory = suite::SuiteRegistry<problem::RealSingleObjective>::instance();

    // id 2 (its registry_id) belongs to the BBOB suite, so the suite gets the next free id
    suite::register_suite<registry_test::UserSuite>();
    const auto map = factory.map();
    EXPECT_EQ(map.at(2), "BBOB");
    const auto entry =
        std::find_if(map.begin(), map.end(), [](const auto &kv) { return kv.second == "RegistryTestUserSuite"; });
    ASSERT_NE(entry, map.end());
    EXPECT_GT(entry->first, 4);
    const auto suite = factory.create(entry->first, {1, 2}, {1}, {2});
    EXPECT_EQ(suite->name(), "RegistryTestUserSuite");
    EXPECT_EQ(suite->problem_ids(), std::vector<int>({1, 2}));
}

TEST_F(BaseTest, test_registry_graph_definitions_file)
{
    using namespace ioh::problem;
    using submodular::MaxCut;

    // a directory of its own: ctest runs this test in test_ioh and in test_registry, possibly at the same time
    const auto dir = fs::temp_directory_path() /
                     ("ioh_test_registry_graph_definitions_" + std::to_string(std::random_device{}()));
    ASSERT_TRUE(fs::create_directories(dir / "graphs"));

    // brings the built-in graphs back for the other tests and removes the files, also when the test fails
    struct Cleanup
    {
        fs::path dir;
        ~Cleanup()
        {
            try
            {
                submodular::GraphProblemType<MaxCut>::load_graph_instances<int, int>();
            }
            catch (...)
            {
            }
            std::error_code ec;
            fs::remove_all(dir, ec);
        }
    } cleanup{dir};

    const auto write = [](const fs::path &path, const std::string &text) {
        std::ofstream out(path.string());
        out << text;
        out.close();
        return !out.fail();
    };

    // a user definitions file with two graphs, a triangle and a path of four vertices, whose files are relative
    // to the definitions file; the empty line between them is skipped
    ASSERT_TRUE(write(dir / "graphs" / "triangle", "0\n1 2\n2 3\n3 1\n"));
    ASSERT_TRUE(write(dir / "graphs" / "path", "0\n1 2\n2 3\n3 4\n"));
    ASSERT_TRUE(write(dir / "definitions", "graphs/triangle|NULL|NULL|NULL\n\ngraphs/path|NULL|NULL|NULL\n"));

    // ids from MaxCut::default_id on and names like MaxCut2000, one entry per graph
    const auto entries = MaxCut::registry_entries(dir / "definitions");
    ASSERT_EQ(entries.size(), 2u);
    EXPECT_EQ(entries[0].id, 2000);
    EXPECT_EQ(entries[0].name, "MaxCut2000");
    EXPECT_EQ(entries[1].id, 2001);
    EXPECT_EQ(entries[1].name, "MaxCut2001");
    const auto triangle = entries[0].create(5, 10); // the instance and the dimension are fixed
    EXPECT_EQ(triangle->meta_data().problem_id, 2000);
    EXPECT_EQ(triangle->meta_data().instance, 1);
    EXPECT_EQ(triangle->meta_data().n_variables, 3);
    EXPECT_EQ(entries[1].create(1, 1)->meta_data().n_variables, 4);

    // loading the file registers its graphs under the names of the built-in graphs, in both factories: their
    // creators are replaced and their ids kept
    const auto &graphs = ProblemRegistry<submodular::GraphProblem>::instance();
    const auto &integer = ProblemRegistry<IntegerSingleObjective>::instance();
    const auto graph_map = graphs.map();
    const auto integer_map = integer.map();
    submodular::GraphProblemType<MaxCut>::load_graph_instances<int, int>(dir / "definitions");
    EXPECT_EQ(graphs.map(), graph_map);
    EXPECT_EQ(integer.map(), integer_map);
    EXPECT_EQ(graphs.create(2001, 1, 1)->meta_data().n_variables, 4);
    EXPECT_EQ(integer.create("MaxCut2000", 1, 1)->meta_data().n_variables, 3);

    // the default definitions file brings the built-in graphs back
    submodular::GraphProblemType<MaxCut>::load_graph_instances<int, int>();
    EXPECT_EQ(graphs.create(2000, 1, 1)->meta_data().n_variables, 800);
    EXPECT_EQ(integer.create(2001, 1, 1)->meta_data().n_variables, 800);
}
