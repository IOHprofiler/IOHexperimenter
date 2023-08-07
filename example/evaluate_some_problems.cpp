#include "ioh.hpp"
#include "ioh/problem/cec.hpp"

// A helper function to demangle C++ type names
std::string demangle(const char* name) {
    int status = -1;

    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status == 0) ? res.get() : std::string(name);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    out << "{";
    for (int i = 0; i < v.size(); ++i) {
        if (i != 0)
            out << ", ";
        out << v[i];
    }
    out << "}\n";
    return out;
}

template<typename T>
void print_info(std::string&& name, T& value) {
    std::cout << "---------------------------------------------------------------\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "Type: " << demangle(typeid(value).name()) << "\n";
    std::cout << "Value: " << value << "\n";
    std::cout << "---------------------------------------------------------------\n";
}

int main()
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::RealSingleObjective>::instance();

    auto rastrigin_instance_from_factory = problem_factory.create("CECRastrigin", 1000, 2);
    auto rastrigin_input = std::vector<double>{1, 2};
    auto rastrigin_output = (*rastrigin_instance_from_factory)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    auto rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(2000, 2);
    rastrigin_input = std::vector<double>{4, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(3000, 10);
    rastrigin_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(4000, 10);
    rastrigin_input = std::vector<double>{1, 2, 5, 3, 22, 44, 9, 7, 6, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(5000, 20);
    rastrigin_input = std::vector<double>{1, 2, 5, 3, 4, 8, 9, 7, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    rastrigin_instance = std::make_shared<ioh::problem::cec::Rastrigin>(6000, 20);
    rastrigin_input = std::vector<double>{1, 2, 5, 3, 4, 8, 11, 12, 6, 5, 1, 2, 5, 3, 4, 8, 9, 7, 6, 5};
    rastrigin_output = (*rastrigin_instance)(rastrigin_input);
    std::cout << "rastrigin_output: " << rastrigin_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    auto zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(10000, 2);
    auto zakharov_input = std::vector<double>{1, 2};
    auto zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(20000, 2);
    zakharov_input = std::vector<double>{33, 1};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(30000, 10);
    zakharov_input = std::vector<double>{1, 2, 3, 22, 33, 3, 4, 5, 6, 7};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(40000, 10);
    zakharov_input = std::vector<double>{55, 66, 77, 8, 9, 2, 3, 4, 5, 6};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(50000, 20);
    zakharov_input = std::vector<double>{33, 66, 77, 22, 22, 2, 3, 4, 5, 6, 55, 66, 77, 8, 9, 2, 3, 4, 5, 6};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;

    zakharov_instance = std::make_shared<ioh::problem::cec::Zakharov>(60000, 20);
    zakharov_input = std::vector<double>{55, 11, 77, 8, 9, 2, 3, 4, 5, 6, 55, 66, 77, 8, 9, 2, 33, 4, 5, 6};
    zakharov_output = (*zakharov_instance)(zakharov_input);
    std::cout << "zakharov_output: " << zakharov_output << std::endl;
    std::cout << "==========================" << std::endl << std::endl;
}
