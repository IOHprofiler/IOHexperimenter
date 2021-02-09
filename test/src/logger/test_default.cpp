#include <fstream>
#include <gtest/gtest.h>

#include "ioh.hpp"


fs::path get_dat_path(const fs::path& root, const ioh::problem::bbob::bbob_base& p) {
    using namespace ioh::common;
    return root / string_format("data_f%d_%s", p.get_problem_id(), p.get_problem_name().c_str()) /
           string_format("IOHprofiler_f%d_DIM%d.dat", p.get_problem_id(), p.get_number_of_variables());
}

std::string get_file_as_string(const fs::path& path) {
    std::ifstream t(path);
    std::string str((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
    t.close();
    return str;
}

TEST(logger, default) {
    using namespace ioh::logger;
    using namespace ioh::common;
    using namespace ioh::problem::bbob;
    file::UniqueFolder l_folder;
    auto p = Sphere();
    auto p1 = Sphere(1, 2);
    auto p2 = AttractiveSector();
    
    const auto info_file_name1 = string_format("IOHprofiler_f%d_%s.info",
                                               p.get_problem_id(), p.get_problem_name().c_str());
    const auto info_file_name2 = string_format("IOHprofiler_f%d_%s.info",
                                               p2.get_problem_id(), p2.get_problem_name().c_str());
    {
        auto l = Default<bbob_base>();
        l_folder = l.experiment_folder();

        ASSERT_TRUE(fs::exists(l_folder.path()));

        for (auto *p: std::array<bbob_base*, 3>({&p, &p1, &p2})) {
            const auto x = std::vector<double>(p->get_number_of_variables(), 0);
            for (auto count = 0; 2 > count; ++count) {
                l.track_problem(*p);
                p->evaluate(x);
                l.do_log(p->loggerCOCOInfo());
            }
        }
            
        ASSERT_TRUE(fs::exists(l_folder.path() / info_file_name1));
        ASSERT_TRUE(fs::exists(l_folder.path() / info_file_name2));
    } // force destructor of logger to be called, flushes to files

    const std::string test_info1 =
        R"(suite = "No suite", funcId = 1, funcName = "Sphere", DIM = 4, maximization = "F", algId = "algorithm_name", algInfo = "algorithm_info")"
        "\n%\ndata_f1_Sphere/IOHprofiler_f1_DIM4.dat, 1:1|5.637287, 1:1|5.637287\n"
        R"(suite = "No suite", funcId = 1, funcName = "Sphere", DIM = 2, maximization = "F", algId = "algorithm_name", algInfo = "algorithm_info")"
        "\n%\ndata_f1_Sphere/IOHprofiler_f1_DIM2.dat, 1:1|1.402094, 1:1|1.402094";

    const std::string test_info2 =
        R"(suite = "No suite", funcId = 6, funcName = "Attractive_Sector", DIM = 4, maximization = "F", algId = "algorithm_name", algInfo = "algorithm_info")"
        "\n%\ndata_f6_Attractive_Sector/IOHprofiler_f6_DIM4.dat, 1:1|28268.397464, 1:1|28268.397464";

    ASSERT_EQ(0, get_file_as_string(l_folder.path() / info_file_name1).compare(test_info1));
    ASSERT_EQ(0, get_file_as_string(l_folder.path() / info_file_name2).compare(test_info2));

        
    const std::string header = R"#("function evaluation" "current f(x)" "best-so-far f(x)" "current af(x)+b" "best af(x)+b")#";
    std::array<std::pair<std::string, fs::path>, 3> cases;
    cases[0] = { header + "\n1 1.402094 1.402094 80.882094 80.882094\n", get_dat_path(l_folder.path(),  p1) };
    cases[1] = { header + "\n1 5.637287 5.637287 85.117287 85.117287\n", get_dat_path(l_folder.path(),  p) };
    cases[2] = { header + "\n1 28268.397464 28268.397464 28304.297464 28304.297464\n", get_dat_path(l_folder.path(),  p2) };

    for (const auto&[data, path]: cases) 
        ASSERT_EQ(0, get_file_as_string(path).compare(data + data));
    //
    l_folder.remove();
    ASSERT_TRUE(!fs::exists(l_folder.path()));
}
