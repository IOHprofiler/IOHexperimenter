#include "../utils.hpp"

#include "ioh/logger/analyzer.hpp"
#include "ioh/suite.hpp"
#include "ioh/problem/bbob.hpp"

fs::path get_dat_path(const fs::path &root, const ioh::problem::RealSingleObjective &p)
{
    return root / fmt::format("data_f{:d}_{}", p.meta_data().problem_id, p.meta_data().name) /
        fmt::format("IOHprofiler_f{:d}_DIM{:d}.dat", p.meta_data().problem_id, p.meta_data().n_variables);
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

    BestState<ioh::problem::SingleObjective> bs{67, {std::vector<double>(5), 10.0}};
    EXPECT_EQ("\"evals\": 67, \"y\": 10, \"x\": [0, 0, 0, 0, 0]", bs.repr());

    RunInfo<ioh::problem::SingleObjective> r1{1, 1000, bs, rv};
    EXPECT_EQ("\"instance\": 1, \"evals\": 1000, \"best\": {" + bs.repr() + "}, " + rv.at(0).repr(), r1.repr());

    ScenarioInfo<ioh::problem::SingleObjective> d5 = {5, "/tmp/txt.csv", {r1}};
    EXPECT_EQ("\"dimension\": 5,\n\t\t\"path\": \"/tmp/txt.csv\",\n\t\t\"runs\": [\n\t\t\t{" + r1.repr() + "}\n\t\t]",
              d5.repr());

    ExperimentInfo<ioh::problem::SingleObjective> e1{"suite", problem.meta_data(), algorithm, rx, {"r1", "r1"}, {"x1"}, {d5}};
}

TEST_F(BaseTest, logger_v2)
{
    using namespace ioh;
    fs::path output_directory;
    {
        auto p0 = problem::bbob::Sphere(1, 2);
        auto p1 = problem::bbob::Sphere(1, 5);
        logger::analyzer::Analyzer logger({trigger::on_improvement, trigger::each(3)},
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