#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh::problem;


static std::string to_lower(const std::string &s)
{
    auto res = s;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::tolower(c); });
    return res;
}

template <typename P>
void define_bbob_problems(py::module &mi, const std::string &name = "BBOB", const bool submodule = false)
{
    py::class_<P, RealSingleObjective, std::shared_ptr<P>>(mi, name.c_str(),
                                                           R"pbdoc(
            Black-Box Optimization Benchmarking (BBOB) problem set.

            Contains 24 noiselessreal-valued test functions supported on [-5, 5]^n, where n is the dimensionality.

            This problem was orginally proposed by Hansen et. al. in [FinckHRA10] and was implemented
            as the core component of the COmparing Continous Optimizer (COCO) platform [HansenARMTB20].

            We took the implementation of those 24 functions in
            https://github.com/numbbo/coco/tree/master/code-experiments/src (v2.2)
            and adopted those to our framework.

            We have acknowledged and specified in our license file
            https://github.com/IOHprofiler/IOHexperimenter/blob/master/LICENSE.md
            the usage and modification to the COCO/BBOB sources.

            Reference
            ---------
            [HansenARMTB20] Nikolaus Hansen, Anne Auger, Raymond Ros, Olaf Mersmann,
            Tea Tusar, and Dimo Brockhoff. "COCO: A platform for comparing continuous optimizers in
            a black-box setting." Optimization Methods and Software (2020): 1-31.

            [FinckHRA10] Steffen Finck, Nikolaus Hansen, Raymond Ros, and Anne Auger.
            "Real-parameter black-box optimization benchmarking 2009: Presentation of the noiseless functions."
            Technical Report 2009/20, Research Center PPE, 2009. Updated February, 2010.

        )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<P, int, int>::instance().create(name, iid, dim);
            },
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: str
                        a string indicating the problem name. 
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
            )pbdoc")

        .def_static(
            "create",
            [](int id, int iid, int dim) { return ioh::common::Factory<P, int, int>::instance().create(id, iid, dim); },
            py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: int
                        a string indicating the problem name. 
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
            )pbdoc")
        .def_property_readonly_static(
            "problems", [](py::object) { return ioh::common::Factory<P, int, int>::instance().map(); },
            "All registered problems");

    auto m = mi;
    if (submodule)
        m = mi.def_submodule(to_lower(name).c_str(), fmt::format("module containing {} problems", name).c_str());

    py::class_<bbob::Sphere<P>, P, std::shared_ptr<bbob::Sphere<P>>>(m, "Sphere", py::is_final(), "Sphere function")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Ellipsoid<P>, P, std::shared_ptr<bbob::Ellipsoid<P>>>(m, "Ellipsoid", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Rastrigin<P>, P, std::shared_ptr<bbob::Rastrigin<P>>>(m, "Rastrigin", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::BuecheRastrigin<P>, P, std::shared_ptr<bbob::BuecheRastrigin<P>>>(m, "BuecheRastrigin",
                                                                                       py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::LinearSlope<P>, P, std::shared_ptr<bbob::LinearSlope<P>>>(m, "LinearSlope", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::AttractiveSector<P>, P, std::shared_ptr<bbob::AttractiveSector<P>>>(m, "AttractiveSector",
                                                                                         py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::StepEllipsoid<P>, P, std::shared_ptr<bbob::StepEllipsoid<P>>>(m, "StepEllipsoid", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Rosenbrock<P>, P, std::shared_ptr<bbob::Rosenbrock<P>>>(m, "Rosenbrock", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::RosenbrockRotated<P>, P, std::shared_ptr<bbob::RosenbrockRotated<P>>>(m, "RosenbrockRotated",
                                                                                           py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::EllipsoidRotated<P>, P, std::shared_ptr<bbob::EllipsoidRotated<P>>>(m, "EllipsoidRotated",
                                                                                         py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Discus<P>, P, std::shared_ptr<bbob::Discus<P>>>(m, "Discus", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::BentCigar<P>, P, std::shared_ptr<bbob::BentCigar<P>>>(m, "BentCigar", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::SharpRidge<P>, P, std::shared_ptr<bbob::SharpRidge<P>>>(m, "SharpRidge", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::DifferentPowers<P>, P, std::shared_ptr<bbob::DifferentPowers<P>>>(m, "DifferentPowers",
                                                                                       py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::RastriginRotated<P>, P, std::shared_ptr<bbob::RastriginRotated<P>>>(m, "RastriginRotated",
                                                                                         py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Weierstrass<P>, P, std::shared_ptr<bbob::Weierstrass<P>>>(m, "Weierstrass", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schaffers10<P>, P, std::shared_ptr<bbob::Schaffers10<P>>>(m, "Schaffers10", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schaffers1000<P>, P, std::shared_ptr<bbob::Schaffers1000<P>>>(m, "Schaffers1000", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::GriewankRosenbrock<P>, P, std::shared_ptr<bbob::GriewankRosenbrock<P>>>(m, "GriewankRosenbrock",
                                                                                             py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Schwefel<P>, P, std::shared_ptr<bbob::Schwefel<P>>>(m, "Schwefel", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Gallagher101<P>, P, std::shared_ptr<bbob::Gallagher101<P>>>(m, "Gallagher101", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Gallagher21<P>, P, std::shared_ptr<bbob::Gallagher21<P>>>(m, "Gallagher21", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::Katsuura<P>, P, std::shared_ptr<bbob::Katsuura<P>>>(m, "Katsuura", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<bbob::LunacekBiRastrigin<P>, P, std::shared_ptr<bbob::LunacekBiRastrigin<P>>>(m, "LunacekBiRastrigin",
                                                                                             py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
}


void define_cec2013_problems(py::module &m)
{
    py::class_<CEC2013, RealSingleObjective, std::shared_ptr<CEC2013>>(m, "CEC2013",
                                                                       R"pbdoc(
            Functions from the CEC2013 conference. 
        )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<CEC2013, int, int>::instance().create(name, iid, dim);
            },
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
            Create a problem instance

            Parameters
            ----------
                problem_name: str
                    a string indicating the problem name.
                instance_id: int
                    an integer identifier of the problem instance
                dimension: int
                    the dimensionality of the search space
        )pbdoc")
        .def_static(
            "create",
            [](int id, int iid, int dim) {
                return ioh::common::Factory<CEC2013, int, int>::instance().create(id, iid, dim);
            },
            py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
            Create a problem instance

            Parameters
            ----------
                problem_id: int
                    a number indicating the problem numeric identifier.
                instance_id: int
                    an integer identifier of the problem instance
                dimension: int
                    the dimensionality of the search space
        )pbdoc")
        .def_property_readonly_static(
            "problems", [](py::object) { return ioh::common::Factory<CEC2013, int, int>::instance().map(); },
            "All registered problems")
        .def_readonly("n_optima", &CEC2013::n_optima)
        .def_readonly("optima", &CEC2013::optima)
        .def_readonly("single_global_optimum", &CEC2013::single_global_optimum)
        .def_readonly("sphere_limit", &CEC2013::sphere_limit)
        .def_readonly("rho", &CEC2013::rho)
        .def("set_optimum", &CEC2013::set_optimum, py::arg("i"), py::arg("single_global") = true);

    using namespace cec2013;

    py::class_<EqualMaxima, CEC2013, std::shared_ptr<EqualMaxima>>(m, "EqualMaxima", py::is_final());
    py::class_<FivePeaks, CEC2013, std::shared_ptr<FivePeaks>>(m, "FivePeaks", py::is_final());
    py::class_<ModifiedRastrigin, CEC2013, std::shared_ptr<ModifiedRastrigin>>(m, "ModifiedRastrigin", py::is_final());
    py::class_<Shubert, CEC2013, std::shared_ptr<Shubert>>(m, "Shubert", py::is_final());
    py::class_<SixHumpCamelback, CEC2013, std::shared_ptr<SixHumpCamelback>>(m, "SixHumpCamelback", py::is_final());
    py::class_<UnevenEqualMaxima, CEC2013, std::shared_ptr<UnevenEqualMaxima>>(m, "UnevenEqualMaxima", py::is_final());
    py::class_<Vincent, CEC2013, std::shared_ptr<Vincent>>(m, "Vincent", py::is_final());
    py::class_<CompositionFunction, CEC2013, std::shared_ptr<CompositionFunction>>(m, "CEC2013CompositionFunction",
                                                                                   py::is_final());
}

void define_cec2022_problems(py::module &m)
{
    py::class_<CEC2022, RealSingleObjective, std::shared_ptr<CEC2022>>(m, "CEC2022",
                                                                       R"pbdoc(
            Functions from the CEC2022 2022 conference.
        )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<CEC2022, int, int>::instance().create(name, iid, dim);
            },
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
            Create a problem instance

            Parameters
            ----------
                problem_name: str
                    a string indicating the problem name.
                instance_id: int
                    an integer identifier of the problem instance
                dimension: int
                    the dimensionality of the search space
        )pbdoc")
        .def_static(
            "create",
            [](int id, int iid, int dim) {
                return ioh::common::Factory<CEC2022, int, int>::instance().create(id, iid, dim);
            },
            py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
            Create a problem instance

            Parameters
            ----------
                problem_id: int
                    a number indicating the problem numeric identifier.
                instance_id: int
                    an integer identifier of the problem instance
                dimension: int
                    the dimensionality of the search space
        )pbdoc")
        .def_property_readonly_static(
            "problems", [](py::object) { return ioh::common::Factory<CEC2022, int, int>::instance().map(); },
            "All registered problems");

    py::class_<cec2022::Zakharov, CEC2022, std::shared_ptr<cec2022::Zakharov>>(m, "CEC2022Zakharov", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::Rosenbrock, CEC2022, std::shared_ptr<cec2022::Rosenbrock>>(m, "CEC2022Rosenbrock",
                                                                                   py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::SchafferF7, CEC2022, std::shared_ptr<cec2022::SchafferF7>>(m, "CEC2022SchafferF7",
                                                                                   py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::Rastrigin, CEC2022, std::shared_ptr<cec2022::Rastrigin>>(m, "CEC2022Rastrigin", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::Levy, CEC2022, std::shared_ptr<cec2022::Levy>>(m, "CEC2022Levy", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::HybridFunction1, CEC2022, std::shared_ptr<cec2022::HybridFunction1>>(
        m, "CEC2022HybridFunction1", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::HybridFunction2, CEC2022, std::shared_ptr<cec2022::HybridFunction2>>(
        m, "CEC2022HybridFunction2", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::HybridFunction3, CEC2022, std::shared_ptr<cec2022::HybridFunction3>>(
        m, "CEC2022HybridFunction3", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::CompositionFunction1, CEC2022, std::shared_ptr<cec2022::CompositionFunction1>>(
        m, "CEC2022CompositionFunction1", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::CompositionFunction2, CEC2022, std::shared_ptr<cec2022::CompositionFunction2>>(
        m, "CEC2022CompositionFunction2", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::CompositionFunction3, CEC2022, std::shared_ptr<cec2022::CompositionFunction3>>(
        m, "CEC2022CompositionFunction3", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));

    py::class_<cec2022::CompositionFunction4, CEC2022, std::shared_ptr<cec2022::CompositionFunction4>>(
        m, "CEC2022CompositionFunction4", py::is_final())
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
}


enum class SamplerType
{
    UNIFORM,
    SOBOL,
    HALTON
};

template <typename T>
T star_discrepancy_init(const int instance, const int n_variables, const int n_samples, const SamplerType sampler_type)
{
    using namespace ioh::common::random::sampler;

    std::vector<std::vector<double>> grid;
    switch (sampler_type)
    {
    case SamplerType::UNIFORM:
        grid = star_discrepancy::generate_grid<Uniform<double>>(instance, n_variables, n_samples);
        break;
    case SamplerType::HALTON:
        grid = star_discrepancy::generate_grid<Halton>(instance, n_variables, n_samples);
        break;
    default:
    case SamplerType::SOBOL:
        grid = star_discrepancy::generate_grid<Sobol>(instance, n_variables, n_samples);
    }
    const std::string py_name =
        std::is_base_of_v<ioh::problem::RealSingleObjective, T> ? "RealStarDiscrepancy" : "IntegerStarDiscrepancy";
    return T(0, instance, n_variables, py_name, grid);
}


template <typename T, typename P>
void define_star_discrepancy_problem(py::module &m, const std::string &name)
{
    py::class_<T, P, std::shared_ptr<T>>(m, name.c_str(), R"pbdoc(
            Star-discrepancy problems
             
            The 𝐿_infinity star discrepancy is a measure for the regularity of a finite set
            of points taken from [0, 1)^d. Calculating this measure is challenging, and exact algorithm
            fall short even for relatively small grids. This suite provides access to a set of these problems
            to tackle the problem from an optimization perspective.

            There are two equivalent representations for these problems: Integer-based and Real-valued. 

            For the Real-valued, the goal is to find the a point in [0,1)^d which minimizes the resulting discepancy directly.

            The Integer-valued version takes advantage of the fact that for each dimension, the optimium 
            has a value matching an existing point int the grid. As such, the search for a point in the grid can 
            be reduced to finding in each dimension the index of the coordinate, resulting in a search domain 
            [0,n]^d, where n is the number of gridpoints. 


            Reference
            ---------
            [Clement23] Clement, Francois, Diederick Vermetten, Jacob de Nobel, Alexandre Jesus, 
            Luís Paquete, and Carola Doerr. "Computing Star Discrepancies with 
            Numerical Black-Box Optimization Algorithms."

        )pbdoc")
        .def(py::init(&star_discrepancy_init<T>), py::arg("instance") = 1, py::arg("n_variables") = 5,
             py::arg("n_samples") = 5, py::arg("sampler_type") = SamplerType::UNIFORM,
             R"pbdoc(
                Star Discrepancy Problems

                Parameters
                ----------
                instance: int = 1
                    The instance of the problem, will be used as seed for sampler
                n_variables: int = 5
                    The dimension of the problem
                n_samples: int = 5
                    The number of sampled points
                sampler_type: StarDiscrepancySampler
                    The type of sampler to use when sampling the points
             )pbdoc")
        .def_property_readonly("grid",
                               [](const T &self) {
                                   const auto grid = self.get_grid();
                                   const auto n = grid.size(), m = grid[0].size();

                                   py::array_t<double, py::array::c_style> arr({n, m});

                                   auto ra = arr.mutable_unchecked();

                                   for (size_t i = 0; i < n; i++)
                                       for (size_t j = 0; j < m; j++)
                                           ra(i, j) = grid[i][j];

                                   return arr;
                               })
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<T, int, int>::instance().create(name, iid, dim);
            },
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: str
                        a string indicating the problem name. 
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
            )pbdoc")

        .def_static(
            "create",
            [](int id, int iid, int dim) { return ioh::common::Factory<T, int, int>::instance().create(id, iid, dim); },
            py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: int
                        a string indicating the problem name. 
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
            )pbdoc")
        .def_property_readonly_static(
            "problems", [](py::object) { return ioh::common::Factory<T, int, int>::instance().map(); },
            "All registered problems");
    ;
}


void define_star_discrepancy_problems(py::module &m)
{
    using namespace ioh::problem::star_discrepancy;
    using namespace ioh::common::random::sampler;

    py::enum_<SamplerType>(m, "StarDiscrepancySampler",
                           "Methods which can be used to sample the initial grids for the star discrepancy problems")
        .value("UNIFORM", SamplerType::UNIFORM)
        .value("HALTON", SamplerType::HALTON)
        .value("SOBOL", SamplerType::SOBOL)
        .export_values();

    define_star_discrepancy_problem<real::StarDiscrepancy, RealSingleObjective>(m, "RealStarDiscrepancy");
    define_star_discrepancy_problem<integer::StarDiscrepancy, IntegerSingleObjective>(m, "IntegerStarDiscrepancy");
}

void define_many_affine(py::module &m)
{
    py::class_<ioh::problem::bbob::ManyAffine, ioh::problem::RealSingleObjective,
               std::shared_ptr<ioh::problem::bbob::ManyAffine>>(m, "ManyAffine")
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"))
        .def(py::init<std::vector<double>, std::array<double, 24>, std::array<int, 24>, int, std::array<double, 24>>(),
             py::arg("xopt"), py::arg("weights"), py::arg("instances"), py::arg("n_variables"),
             py::arg("scale_factors") = ioh::problem::bbob::ManyAffine::default_scales)
        .def_property_readonly("weights", &ioh::problem::bbob::ManyAffine::get_weights)
        .def_property_readonly("instances", &ioh::problem::bbob::ManyAffine::get_instances)
        .def_property_readonly("scale_factors", &ioh::problem::bbob::ManyAffine::get_scale_factors)
        .def_property_readonly("sub_problems", &ioh::problem::bbob::ManyAffine::get_problems)
        .def_property_readonly("function_values", &ioh::problem::bbob::ManyAffine::get_function_values);
}


void define_funnel(py::module &m)
{
    using namespace ioh::problem;
    py::class_<funnel::DoubleFunnel, RealSingleObjective, std::shared_ptr<funnel::DoubleFunnel>>(m, "DoubleFunnel")
        .def_property_readonly("d", &funnel::DoubleFunnel::d)
        .def_property_readonly("s", &funnel::DoubleFunnel::s)
        .def_property_readonly("u1", &funnel::DoubleFunnel::u1)
        .def_property_readonly("u2", &funnel::DoubleFunnel::u2);


    py::class_<funnel::DoubleSphere, funnel::DoubleFunnel, std::shared_ptr<funnel::DoubleSphere>>(m, "DoubleSphere")
        .def(py::init<int, double, double>(), py::arg("n_variables"), py::arg("d") = 0.0, py::arg("s") = 1.0);


    py::class_<funnel::DoubleRastrigin, funnel::DoubleFunnel, std::shared_ptr<funnel::DoubleRastrigin>>(
        m, "DoubleRastrigin")
        .def(py::init<int, double, double>(), py::arg("n_variables"), py::arg("d") = 0.0, py::arg("s") = 1.0);
}

void define_bbob(py::module &m)
{
    define_bbob_problems<ioh::problem::BBOB>(m);
    define_bbob_problems<ioh::problem::SBOX>(m, "SBOX", true);
}


void define_dynamic_bin_val_problem(py::module &m)
{
    using namespace ioh::problem::dynamic_bin_val;

    py::class_<DynamicBinVal, IntegerSingleObjective, std::shared_ptr<DynamicBinVal>>(m, "DynamicBinVal")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<DynamicBinVal, int, int>::instance().create(name, iid, dim);
            },
            py::arg("problem_name"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: str
                        a string indicating the problem name.
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
        )pbdoc")
        .def_static(
            "create",
            [](int id, int iid, int dim) {
                return ioh::common::Factory<DynamicBinVal, int, int>::instance().create(id, iid, dim);
            },
            py::arg("problem_id"), py::arg("instance_id"), py::arg("dimension"),
            R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_id: int
                        a number indicating the problem numeric identifier.
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
        )pbdoc")
        .def_property_readonly_static(
            "problems", [](py::object) { return ioh::common::Factory<DynamicBinVal, int, int>::instance().map(); },
            "All registered problems")
        .def("step", &DynamicBinVal::step, R"pbdoc(
            Step the dynamic binary value problem forward by one timestep, and permute the weights randomly.

            Returns
            -------
            int
                The current timestep after the step.
        )pbdoc")
        .def_readonly("time_step", &DynamicBinVal::time_step)
        .def_readonly("weights", &DynamicBinVal::weights)
        ;

    const auto doc =
        R"pbdoc(Dynamic BinVal. Details: https://link.springer.com/article/10.1007/s42979-022-01203-z )pbdoc";
    
    py::class_<Uniform, DynamicBinVal, std::shared_ptr<Uniform>>(m, "DynamicBinValUniform", doc)
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<PowersOfTwo, DynamicBinVal, std::shared_ptr<PowersOfTwo>>(m, "DynamicBinValPowersOfTwo", doc)
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"));
    py::class_<Pareto, DynamicBinVal, std::shared_ptr<Pareto>>(m, "DynamicBinValPareto", doc)
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"))
        .def_property_readonly("pareto_shape", &Pareto::get_pareto_shape);

    py::class_<Ranking, DynamicBinVal, std::shared_ptr<Ranking>>(m, "DynamicBinValRanking", doc)
        .def(py::init<int, int>(), py::arg("instance"), py::arg("n_variables"))
        .def("rank", &Ranking::rank, R"pbdoc(
            Sort a list of bitstrings in lexicographical order in-place.

            Parameters
            ----------
            bitstrings : list
                A list of bitstrings to sort.
        )pbdoc")
        .def("rank_indices", &Ranking::rank_indices, R"pbdoc(
            Sort a list of bitstrings in lexicographical order in-place.

            Parameters
            ----------
            bitstrings : list
                A list of bitstrings to sort.
        )pbdoc")
        .def("get_comparison_ordering", &Ranking::get_comparison_ordering, R"pbdoc(
            Get the current comparison ordering vector.

            Returns
            -------
            list of int
                The current state of the comparison ordering vector.
        )pbdoc");
}