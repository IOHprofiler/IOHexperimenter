#include "../utils.hpp"

#include "ioh/logger/analyzer.hpp"
#include "ioh/suite.hpp"
#include "ioh/problem/bbob.hpp"

fs::path get_dat_path(const fs::path &root, const ioh::problem::Real &p)
{
    return root / fmt::format("data_f{:d}_{}", p.meta_data().problem_id, p.meta_data().name) /
        fmt::format("IOHprofiler_f{:d}_DIM{:d}.dat", p.meta_data().problem_id, p.meta_data().n_variables);
}

TEST_F(BaseTest, logger_v1)
{
    using namespace ioh;
    auto p = problem::bbob::Sphere(1, 4);
    auto p1 = problem::bbob::Sphere(1, 2);
    auto p2 = problem::bbob::AttractiveSector(1, 4);
    fs::path output_directory;
    const auto info_file_name1 = fmt::format("IOHprofiler_f{:d}_{}.info", p.meta_data().problem_id, p.meta_data().name);
    const auto info_file_name2 =
        fmt::format("IOHprofiler_f{:d}_{}.info", p2.meta_data().problem_id, p2.meta_data().name);
    {
        logger::analyzer::v1::Analyzer l;
        output_directory = l.output_directory();

        for (auto *problem : std::array<ioh::problem::BBOB *, 3>({&p, &p1, &p2}))
        {
            const auto x = std::vector<double>(problem->meta_data().n_variables, 0.);
            for (auto count = 0; 2 > count; ++count)
            {
                problem->attach_logger(l);
                (*problem)(x);
                problem->reset();
            }
        }
        EXPECT_TRUE(fs::exists(output_directory));
        EXPECT_TRUE(fs::exists(output_directory / info_file_name1));
        EXPECT_TRUE(fs::exists(output_directory / info_file_name2));
    } // force destructor of logger to be called, flushes to files

    const std::string test_info1 =
        R"(suite = "unknown_suite", funcId = 1, funcName = "Sphere", DIM = 4, maximization = "F", algId = "algorithm_name", algInfo = "algorithm_info")"
        "\n%\ndata_f1_Sphere/IOHprofiler_f1_DIM4.dat, 1:1|5.63729, 1:1|5.63729\n"
        R"(suite = "unknown_suite", funcId = 1, funcName = "Sphere", DIM = 2, maximization = "F", algId = "algorithm_name", algInfo = "algorithm_info")"
        "\n%\ndata_f1_Sphere/IOHprofiler_f1_DIM2.dat, 1:1|1.40209, 1:1|1.40209";

    const std::string test_info2 =
        R"(suite = "unknown_suite", funcId = 6, funcName = "AttractiveSector", DIM = 4, maximization = "F", algId = "algorithm_name", algInfo = "algorithm_info")"
        "\n%\ndata_f6_AttractiveSector/IOHprofiler_f6_DIM4.dat, 1:1|28268.4, 1:1|28268.4";

    compare_file_with_string(output_directory / info_file_name1, test_info1);
    compare_file_with_string(output_directory / info_file_name2, test_info2);

    const std::string header =
        R"#("function evaluation" "current f(x)" "best-so-far f(x)" "current af(x)+b" "best af(x)+b")#";
    std::array<std::pair<std::string, fs::path>, 3> cases;
    cases[0] = {header + "\n1 1.4020940800 1.4020940800 80.8820940800 80.8820940800\n",
                get_dat_path(output_directory, p1)};
    cases[1] = {header + "\n1 5.6372870400 5.6372870400 85.1172870400 85.1172870400\n",
                get_dat_path(output_directory, p)};

#if defined(__APPLE__)
    // On MacOS double values are sligthly different, this is probably causes by implementations in math.h
    // see: https://stackoverflow.com/questions/44765611/slightly-different-result-from-exp-function-on-mac-and-linux
    cases[2] = {header + "\n1 28268.3974644752 28268.3974644752 28304.2974644752 28304.2974644752\n",
                get_dat_path(output_directory, p2)};
#else
    cases[2] = {header + "\n1 28268.3974644746 28268.3974644746 28304.2974644746 28304.2974644746\n",
                get_dat_path(output_directory, p2)};
#endif

    for (const auto &[data, path] : cases)
        compare_file_with_string(path, data + data);

    fs::remove_all(output_directory);
    EXPECT_TRUE(!fs::exists(output_directory));
}

TEST_F(BaseTest, structures)
{
    using namespace ioh::logger::analyzer::structures;

    ioh::problem::bbob::AttractiveSector problem{1, 5};

    AlgorithmInfo algorithm{"PSO", "A1"};
    EXPECT_EQ("\"name\": \"PSO\", \"info\": \"A1\"", algorithm.repr());

    std::vector<Attribute<double>> rv{{"r1", 1.0}};
    EXPECT_EQ("\"r1\": 1", fmt::format("{}", fmt::join(rv, ", ")));

    std::vector<Attribute<std::string>> rx{{"ra", "b"}};
    EXPECT_EQ("\"ra\": \"b\"", fmt::format("{}", fmt::join(rx, ", ")));

    BestPoint bp{67, {std::vector<double>(5), 10.0}};
    EXPECT_EQ("\"evals\": 67, \"y\": 10, \"x\": [0, 0, 0, 0, 0]", bp.repr());

    RunInfo r1{1, 1000, bp, rv};
    EXPECT_EQ("\"instance\": 1, \"evals\": 1000, \"best\": {" + bp.repr() + "}, " + rv.at(0).repr(), r1.repr());

    ScenarioInfo d5 = {5, "/tmp/txt.csv", {r1}};
    EXPECT_EQ("\"dimension\": 5,\n\t\t\"path\": \"/tmp/txt.csv\",\n\t\t\"runs\": [\n\t\t\t{" + r1.repr() + "}\n\t\t]",
              d5.repr());

    ExperimentInfo e1{"suite", problem.meta_data(), algorithm, rx, {"r1", "r1"}, {"x1"}, {d5}};
}

TEST_F(BaseTest, logger_v2)
{
    using namespace ioh;
    fs::path output_directory;
    {
        auto p0 = problem::bbob::Sphere(1, 2);
        auto p1 = problem::bbob::Sphere(1, 5);
        logger::analyzer::v2::Analyzer logger({trigger::on_improvement, trigger::each(3)},
                                              {watch::violation, watch::penalty});
        logger.add_experiment_attribute("hallo", "10");
        output_directory = logger.output_directory();
        double r = 0;
        logger.add_run_attribute("runid", &r);
        logger.add_run_attribute("runid2", &r);

        for (auto *pb : std::vector<problem::BBOB *>({&p0, &p1}))
        {
            pb->attach_logger(logger);
            for (r = 0; r < 3; ++r)
            {
                for (auto s = 1; s < 2; ++s)
                    (*pb)(common::random::pbo::uniform(pb->meta_data().n_variables, s));
                pb->reset();
            }
        }
    }
    EXPECT_TRUE(fs::exists(output_directory));
    EXPECT_TRUE(fs::exists(output_directory / "IOHprofiler_f1_Sphere.json"));
    EXPECT_TRUE(fs::exists(output_directory / "data_f1_Sphere"));

    const auto dat_file = output_directory / "data_f1_Sphere" / "IOHprofiler_f1_DIM2.dat";
    EXPECT_TRUE(fs::exists(dat_file));


    const std::string header = "evaluations raw_y violation penalty"
                               "\n1 1.5860477825 0.0000000000 0.0000000000\n";
    compare_file_with_string(dat_file, header + header + header);

    EXPECT_TRUE(fs::exists(output_directory / "data_f1_Sphere" / "IOHprofiler_f1_DIM5.dat"));
    EXPECT_TRUE(fs::remove_all(output_directory));
    EXPECT_TRUE(!fs::exists(output_directory));
}


TEST_F(BaseTest, test_logging_new_header)
{
    using namespace ioh;
    auto p = problem::bbob::Katsuura(1, 4);
    const auto info_file_name1 = fmt::format("IOHprofiler_f{:d}_{}.info", p.meta_data().problem_id, p.meta_data().name);
    fs::path output_directory;
    {
        logger::analyzer::v1::Analyzer l({trigger::on_improvement}, {watch::violation, watch::penalty},
                                         fs::current_path(), "ioh_data", "algorithm_name", "algorithm_info", false,
                                         false);
        output_directory = l.output_directory();

        const auto x = std::vector<double>(p.meta_data().n_variables, -6.);
        for (auto count = 0; 2 > count; ++count)
        {
            p.attach_logger(l);
            p(x);
            p.reset();
        }
    } // force destructor of logger to be called, flushes to files

    const std::string test_info1 =
        R"(suite = "unknown_suite", funcId = 23, funcName = "Katsuura", DIM = 4, maximization = "F", algId = "algorithm_name", algInfo = "algorithm_info")"
        "\n%\ndata_f23_Katsuura/IOHprofiler_f23_DIM4.dat, 1:1|27.7634, 1:1|27.7634";

    compare_file_with_string(output_directory / info_file_name1, test_info1);

    const std::string header = "evaluations raw_y violation penalty"
                               "\n1 27.7633746377 4.0000000000 4.0000000000\n";
    compare_file_with_string(get_dat_path(output_directory, p), header + header);

    fs::remove_all(output_directory);
    EXPECT_TRUE(!fs::exists(output_directory));
}


TEST_F(BaseTest, test_new_logger_n_files)
{
    using namespace ioh;
    ioh::suite::BBOB suite;

    fs::path output_directory;
    {
        logger::Analyzer l;
        output_directory = l.output_directory();

        for (auto &problem : suite)
        {
            const auto x = std::vector<double>(problem->meta_data().n_variables, 0.);

            for (auto count = 0; 1 > count; ++count)
            {
                problem->attach_logger(l);
                (*problem)(x);
                problem->reset();
            }
        }
        EXPECT_TRUE(fs::exists(output_directory));
    } // force destructor of logger to be called, flushes to files

    size_t n_json = 0, n_folder = 0, n_dat = 0;
    for (const auto &entry : fs::directory_iterator(output_directory)){
        if(entry.path().extension() == ".json")
            n_json++;
        else{
            n_folder++;
            for(const auto &sub : fs::directory_iterator(entry.path())){
                if(sub.path().extension() == ".dat")
                    n_dat++;
            }
        }
    }
    EXPECT_EQ(n_json, suite.size());
    EXPECT_EQ(n_folder, suite.size());
    EXPECT_EQ(n_dat, suite.size());

    fs::remove_all(output_directory);
    EXPECT_TRUE(!fs::exists(output_directory));
}