#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

#include "ioh.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip> // This is the new include
#include <limits>
#include "ioh/problem/cec.hpp"

int main()
{
    std::cout.imbue(std::locale("en_US.utf8"));

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::RealSingleObjective>::instance();

    auto rosenbrock_instance_from_factory = problem_factory.create("CEC_Rosenbrock", 1, 2);
    auto rosenbrock_input = std::vector<double>{1, 2};
    auto rosenbrock_output = (*rosenbrock_instance_from_factory)(rosenbrock_input);
    std::cout << "rosenbrock_output: " << rosenbrock_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rosenbrock_instance_from_factory = problem_factory.create("CEC_Rosenbrock", 1, 2);
    rosenbrock_input = std::vector<double>{4, 5};
    rosenbrock_output = (*rosenbrock_instance_from_factory)(rosenbrock_input);
    std::cout << "rosenbrock_output: " << rosenbrock_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rosenbrock_instance_from_factory = problem_factory.create("CEC_Rosenbrock", 1, 10);
    rosenbrock_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rosenbrock_output = (*rosenbrock_instance_from_factory)(rosenbrock_input);
    std::cout << "rosenbrock_output: " << rosenbrock_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rosenbrock_instance_from_factory = problem_factory.create("CEC_Rosenbrock", 1, 10);
    rosenbrock_input = std::vector<double>{1, 2, 5, 3, 22, 44, 9, 7, 6, 5};
    rosenbrock_output = (*rosenbrock_instance_from_factory)(rosenbrock_input);
    std::cout << "rosenbrock_output: " << rosenbrock_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rosenbrock_instance_from_factory = problem_factory.create("CEC_Rosenbrock", 1, 20);
    rosenbrock_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rosenbrock_output = (*rosenbrock_instance_from_factory)(rosenbrock_input);
    std::cout << "rosenbrock_output: " << rosenbrock_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rosenbrock_instance_from_factory = problem_factory.create("CEC_Rosenbrock", 1, 20);
    rosenbrock_input = std::vector<double>{1, 2, 5, 3, 4, 8, 11, 12, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rosenbrock_output = (*rosenbrock_instance_from_factory)(rosenbrock_input);
    std::cout << "rosenbrock_output: " << rosenbrock_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;


    auto levy_instance_from_factory = problem_factory.create("CEC_Levy", 1, 2);
    auto levy_input = std::vector<double>{1, 2};
    auto levy_output = (*levy_instance_from_factory)(levy_input);
    std::cout << "levy_output: " << levy_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    levy_instance_from_factory = problem_factory.create("CEC_Levy", 1, 2);
    levy_input = std::vector<double>{4, 5};
    levy_output = (*levy_instance_from_factory)(levy_input);
    std::cout << "levy_output: " << levy_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    levy_instance_from_factory = problem_factory.create("CEC_Levy", 1, 10);
    levy_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    levy_output = (*levy_instance_from_factory)(levy_input);
    std::cout << "levy_output: " << levy_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    levy_instance_from_factory = problem_factory.create("CEC_Levy", 1, 10);
    levy_input = std::vector<double>{1, 2, 5, 3, 22, 44, 9, 7, 6, 5};
    levy_output = (*levy_instance_from_factory)(levy_input);
    std::cout << "levy_output: " << levy_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    levy_instance_from_factory = problem_factory.create("CEC_Levy", 1, 20);
    levy_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    levy_output = (*levy_instance_from_factory)(levy_input);
    std::cout << "levy_output: " << levy_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    levy_instance_from_factory = problem_factory.create("CEC_Levy", 1, 20);
    levy_input = std::vector<double>{1, 2, 5, 3, 4, 8, 11, 12, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    levy_output = (*levy_instance_from_factory)(levy_input);
    std::cout << "levy_output: " << levy_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;


    auto expanded_schaffer_instance_from_factory = problem_factory.create("CEC_ExpandedSchaffer", 1, 2);
    auto expanded_schaffer_input = std::vector<double>{1, 2};
    auto expanded_schaffer_output = (*expanded_schaffer_instance_from_factory)(expanded_schaffer_input);
    std::cout << "expanded_schaffer_output: " << expanded_schaffer_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    expanded_schaffer_instance_from_factory = problem_factory.create("CEC_ExpandedSchaffer", 1, 2);
    expanded_schaffer_input = std::vector<double>{4, 5};
    expanded_schaffer_output = (*expanded_schaffer_instance_from_factory)(expanded_schaffer_input);
    std::cout << "expanded_schaffer_output: " << expanded_schaffer_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    expanded_schaffer_instance_from_factory = problem_factory.create("CEC_ExpandedSchaffer", 1, 10);
    expanded_schaffer_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    expanded_schaffer_output = (*expanded_schaffer_instance_from_factory)(expanded_schaffer_input);
    std::cout << "expanded_schaffer_output: " << expanded_schaffer_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    expanded_schaffer_instance_from_factory = problem_factory.create("CEC_ExpandedSchaffer", 1, 10);
    expanded_schaffer_input = std::vector<double>{1, 2, 5, 3, 22, 44, 9, 7, 6, 5};
    expanded_schaffer_output = (*expanded_schaffer_instance_from_factory)(expanded_schaffer_input);
    std::cout << "expanded_schaffer_output: " << expanded_schaffer_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    expanded_schaffer_instance_from_factory = problem_factory.create("CEC_ExpandedSchaffer", 1, 20);
    expanded_schaffer_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    expanded_schaffer_output = (*expanded_schaffer_instance_from_factory)(expanded_schaffer_input);
    std::cout << "expanded_schaffer_output: " << expanded_schaffer_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    expanded_schaffer_instance_from_factory = problem_factory.create("CEC_ExpandedSchaffer", 1, 20);
    expanded_schaffer_input = std::vector<double>{1, 2, 5, 3, 4, 8, 11, 12, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    expanded_schaffer_output = (*expanded_schaffer_instance_from_factory)(expanded_schaffer_input);
    std::cout << "expanded_schaffer_output: " << expanded_schaffer_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;


    auto rastrigin_instance_from_factory = problem_factory.create("CEC_Rastrigin", 1, 2);
    auto rastrigin_input = std::vector<double>{1, 2};
    auto rastrigin_output = (*rastrigin_instance_from_factory)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    auto rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(1, 2);
    rastrigin_input = std::vector<double>{4, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(1, 10);
    rastrigin_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(1, 10);
    rastrigin_input = std::vector<double>{1, 2, 5, 3, 22, 44, 9, 7, 6, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(1, 20);
    rastrigin_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(1, 20);
    rastrigin_input = std::vector<double>{1, 2, 5, 3, 4, 8, 11, 12, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;


    auto zakharov_instance_from_factory = problem_factory.create("CEC_Zakharov", 1, 2);
    auto zakharov_input = std::vector<double>{1, 2};
    auto zakharov_output = (*zakharov_instance_from_factory)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    auto zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(1, 2);
    zakharov_input = std::vector<double>{33, 1};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(1, 2);
    zakharov_input = std::vector<double>{-100, 100};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(1, 2);
    zakharov_input = std::vector<double>{100, -100};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(1, 10);
    zakharov_input = std::vector<double>{1, 2, 3, 22, 33, 3, 4, 5, 6, 7};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(1, 10);
    zakharov_input = std::vector<double>{55, 66, 77, 8, 9, 2, 3, 4, 5, 6};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(1, 20);
    zakharov_input = std::vector<double>{33, 66, 77, 22, 22, 2, 3, 4, 5, 6, 55, 66, 77, 8, 9, 2, 3, 4, 5, 6};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(1, 20);
    zakharov_input = std::vector<double>{55, 11, 77, 8, 9, 2, 3, 4, 5, 6, 55, 66, 77, 8, 9, 2, 33, 4, 5, 6};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;



    auto hybrid_function_1_instance_from_factory = problem_factory.create("CEC_HybridFunction1", 1, 2);
    auto hybrid_function_1_input = std::vector<double>{55, 11};
    auto hybrid_function_1_output = (*hybrid_function_1_instance_from_factory)(hybrid_function_1_input);
    std::cout << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10);
    std::cout << "CEC_HybridFunction1: " << hybrid_function_1_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    hybrid_function_1_instance_from_factory = problem_factory.create("CEC_HybridFunction1", 1, 10);
    hybrid_function_1_input = std::vector<double>{55, 66, 77, 8, 9, 2, 3, 4, 5, 6};
    hybrid_function_1_output = (*hybrid_function_1_instance_from_factory)(hybrid_function_1_input);
    std::cout << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10);
    std::cout << "CEC_HybridFunction1: " << hybrid_function_1_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    auto hybrid_function_2_instance_from_factory = problem_factory.create("CEC_HybridFunction2", 1, 10);
    auto hybrid_function_2_input = std::vector<double>{55, 66, 77, 8, 9, 2, 3, 4, 5, 6};
    auto hybrid_function_2_output = (*hybrid_function_2_instance_from_factory)(hybrid_function_2_input);
    std::cout << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10);
    std::cout << "CEC_HybridFunction2: " << hybrid_function_2_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    auto hybrid_function_3_instance_from_factory = problem_factory.create("CEC_HybridFunction3", 1, 10);
    auto hybrid_function_3_input = std::vector<double>{55, 66, 77, 8, 9, 2, 3, 4, 5, 6};
    auto hybrid_function_3_output = (*hybrid_function_3_instance_from_factory)(hybrid_function_3_input);
    std::cout << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10);
    std::cout << "CEC_HybridFunction3: " << hybrid_function_3_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;
}
