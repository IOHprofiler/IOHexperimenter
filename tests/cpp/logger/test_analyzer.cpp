#include "../utils.hpp"
 
#include "ioh/logger/analyzer.hpp"
#include "ioh/problem/bbob/sphere.hpp"
#include "ioh/problem/bbob/attractive_sector.hpp"

fs::path get_dat_path(const fs::path& root, const ioh::problem::Real& p) {
    return root 
        / fmt::format("data_f{:d}_{}", p.meta_data().problem_id, p.meta_data().name) 
        / fmt::format("IOHprofiler_f{:d}_DIM{:d}.dat", p.meta_data().problem_id, p.meta_data().n_variables);
}

TEST_F(BaseTest, logger_default)
{
    using namespace ioh;
    auto p = problem::bbob::Sphere(1, 4);
    auto p1 = problem::bbob::Sphere(1, 2);
    auto p2 = problem::bbob::AttractiveSector(1, 4);
    fs::path output_directory;    
    const auto info_file_name1 = fmt::format("IOHprofiler_f{:d}_{}.info", p.meta_data().problem_id, p.meta_data().name);
    const auto info_file_name2 = fmt::format("IOHprofiler_f{:d}_{}.info", p2.meta_data().problem_id, p2.meta_data().name);
    {
        logger::Analyzer l;
        output_directory = l.output_directory();

        for (auto *problem: std::array<ioh::problem::BBOB*, 3>({&p, &p1, &p2})) {
            const auto x = std::vector<double>(problem->meta_data().n_variables, 0);
            for (auto count = 0; 2 > count; ++count) {
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
        
    const std::string header = R"#("function evaluation" "current f(x)" "best-so-far f(x)" "current af(x)+b" "best af(x)+b")#";
    std::array<std::pair<std::string, fs::path>, 3> cases;
    cases[0] = { header + "\n1 1.402094 1.402094 80.882094 80.882094\n", get_dat_path(output_directory, p1) };
    cases[1] = { header + "\n1 5.637287 5.637287 85.117287 85.117287\n", get_dat_path(output_directory, p) };
    cases[2] = { header + "\n1 28268.397464 28268.397464 28304.297464 28304.297464\n", get_dat_path(output_directory, p2) };

    for (const auto&[data, path]: cases)
        compare_file_with_string(path, data + data);
        
    fs::remove_all(output_directory);
    EXPECT_TRUE(!fs::exists(output_directory));
}
