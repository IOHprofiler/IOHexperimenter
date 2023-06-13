#include "../utils.hpp"

#include "ioh/problem/cec.hpp"

using namespace ioh::problem::cec;

CecUtils cec_utils;

TEST_F(BaseTest, loadOShiftData)
{
    std::vector<double> Os;
    std::string dataPath = "/usr/local/include/ioh/problem/cec/cec_data";
    cec_utils.loadOShiftData(Os, dataPath, 2, 1, 2022);
    for (auto d : Os)
    {
        std::cout << d << " ";
    }
    std::cout << std::endl;
}

TEST_F(BaseTest, loadMatrixData)
{
    std::vector<double> Mr;
    std::string dataPath = "/usr/local/include/ioh/problem/cec/cec_data";
    cec_utils.loadMatrixData(Mr, dataPath, 2, 1, 2022);
    for (auto d : Mr)
    {
        std::cout << d << " ";
    }
    std::cout << std::endl;
}


TEST_F(BaseTest, cec2022_basic)
{
    const auto &problem_factory =
        ioh::problem::ProblemRegistry<ioh::problem::CEC2022>::instance();

    auto problem_01 = problem_factory.create(1, 1, 2);
    std::ofstream file1("F1.txt");
    if (!file1.is_open())
        std::cout << "open failed" << std::endl;
    for (double x1 = -100.0; x1 <= 100.0; x1 += 0.5)
    {
        for (double x2 = -100.0; x2 <= 100.0; x2 += 0.5)
        {
            std::vector<double> x = {x1, x2};
            file1 << std::to_string(x1) << " " << std::to_string(x2) << " "
                  << std::to_string((*problem_01)(x)-300.0) << std::endl;
        }
    }
    file1.close();

    auto problem_02 = problem_factory.create(2, 1, 2);
    std::ofstream file2("F2.txt");
    if (!file2.is_open())
        std::cout << "open failed" << std::endl;
    for (double x1 = -100.0; x1 <= 100.0; x1 += 0.5)
    {
        for (double x2 = -100.0; x2 <= 100.0; x2 += 0.5)
        {
            std::vector<double> x = {x1, x2};
            file2 << std::to_string(x1) << " " << std::to_string(x2) << " "
                  << std::to_string((*problem_02)(x)-400.0) << std::endl;
        }
    }
    file2.close();

    auto problem_03 = problem_factory.create(3, 1, 2);
    std::ofstream file3("F3.txt");
    if (!file3.is_open())
        std::cout << "open failed" << std::endl;
    for (double x1 = -100.0; x1 <= 100.0; x1 += 0.5)
    {
        for (double x2 = -100.0; x2 <= 100.0; x2 += 0.5)
        {
            std::vector<double> x = {x1, x2};
            file3 << std::to_string(x1) << " " << std::to_string(x2) << " "
                  << std::to_string((*problem_03)(x)-600.0) << std::endl;
        }
    }
    file3.close();

    auto problem_04 = problem_factory.create(4, 1, 2);
    std::ofstream file4("F4.txt");
    if (!file4.is_open())
        std::cout << "open failed" << std::endl;
    for (double x1 = -100.0; x1 <= 100.0; x1 += 0.5)
    {
        for (double x2 = -100.0; x2 <= 100.0; x2 += 0.5)
        {
            std::vector<double> x = {x1, x2};
            file4 << std::to_string(x1) << " " << std::to_string(x2) << " "
                  << std::to_string((*problem_04)(x)-800.0) << std::endl;
        }
    }
    file4.close();

    auto problem_05 = problem_factory.create(5, 1, 2);
    std::ofstream file5("F5.txt");
    if (!file5.is_open())
        std::cout << "open failed" << std::endl;
    for (double x1 = -100.0; x1 <= 100.0; x1 += 0.5)
    {
        for (double x2 = -100.0; x2 <= 100.0; x2 += 0.5)
        {
            std::vector<double> x = {x1, x2};
            file5 << std::to_string(x1) << " " << std::to_string(x2) << " "
                  << std::to_string((*problem_05)(x)-900.0) << std::endl;
        }
    }
    file5.close();
}


// TEST_F(BaseTest, cec2022_hybrid)
// {
//     std::vector<int> SS;
//     std::vector<double> Os, Mr, x;
//     for (int i = 0; i < 10; i++)
//         x.push_back(i * 1.0);
//     std::string dataPath = "/usr/local/include/ioh/problem/cec/cec_data";
//     cec_utils.loadOShiftData(&Os, dataPath, 10, 6, 2022);
//     cec_utils.loadMatrixData(&Mr, dataPath, 10, 6, 2022);
//     cec_utils.loadShuffleData(&SS, dataPath, 10, 6, 2022);
//     double f = cec2022_hf01(x, Os, Mr, SS, true, true, 10);
//     printf("%f\n", f);
// }
