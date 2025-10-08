#include "pch.hpp"

using namespace ioh::problem;


static std::string to_lower(const std::string &s)
{
    auto res = s;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::tolower(c); });
    return res;
}

template <typename P>
void define_bbob_problems(nb::module_ &mi, const std::string &name = "BBOB", const bool submodule = false)
{
    nb::class_<P, RealSingleObjective>(mi, name.c_str(),
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
            nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"),
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
            nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"),
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
        .def_prop_ro_static(
            "problems", [](nb::object) { return ioh::common::Factory<P, int, int>::instance().map(); },
            "All registered problems");

    auto m = mi;
    if (submodule)
        m = mi.def_submodule(to_lower(name).c_str(), fmt::format("module containing {} problems", name).c_str());

    nb::class_<bbob::Sphere<P>, P>(m, "Sphere", nb::is_final(), "Sphere function")
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Ellipsoid<P>, P>(m, "Ellipsoid", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Rastrigin<P>, P>(m, "Rastrigin", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::BuecheRastrigin<P>, P>(m, "BuecheRastrigin", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::LinearSlope<P>, P>(m, "LinearSlope", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::AttractiveSector<P>, P>(m, "AttractiveSector", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::StepEllipsoid<P>, P>(m, "StepEllipsoid", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Rosenbrock<P>, P>(m, "Rosenbrock", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::RosenbrockRotated<P>, P>(m, "RosenbrockRotated", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::EllipsoidRotated<P>, P>(m, "EllipsoidRotated", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Discus<P>, P>(m, "Discus", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::BentCigar<P>, P>(m, "BentCigar", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::SharpRidge<P>, P>(m, "SharpRidge", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::DifferentPowers<P>, P>(m, "DifferentPowers", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::RastriginRotated<P>, P>(m, "RastriginRotated", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Weierstrass<P>, P>(m, "Weierstrass", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Schaffers10<P>, P>(m, "Schaffers10", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Schaffers1000<P>, P>(m, "Schaffers1000", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::GriewankRosenbrock<P>, P>(m, "GriewankRosenbrock", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Schwefel<P>, P>(m, "Schwefel", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Gallagher101<P>, P>(m, "Gallagher101", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Gallagher21<P>, P>(m, "Gallagher21", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::Katsuura<P>, P>(m, "Katsuura", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<bbob::LunacekBiRastrigin<P>, P>(m, "LunacekBiRastrigin", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));
}


void define_cec2013_problems(nb::module_ &m)
{
    nb::class_<CEC2013, RealSingleObjective>(m, "CEC2013",
                                             R"pbdoc(
            Functions from the CEC2013 conference.
        )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<CEC2013, int, int>::instance().create(name, iid, dim);
            },
            nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"),
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
            nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"),
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
        .def_prop_ro_static(
            "problems", [](nb::object) { return ioh::common::Factory<CEC2013, int, int>::instance().map(); },
            "All registered problems")
        .def_ro("n_optima", &CEC2013::n_optima)
        .def_ro("optima", &CEC2013::optima)
        .def_ro("single_global_optimum", &CEC2013::single_global_optimum)
        .def_ro("sphere_limit", &CEC2013::sphere_limit)
        .def_ro("rho", &CEC2013::rho)
        .def("set_optimum", &CEC2013::set_optimum, nb::arg("i"), nb::arg("single_global") = true);

    using namespace cec2013;

    nb::class_<EqualMaxima, CEC2013>(m, "EqualMaxima", nb::is_final());
    nb::class_<FivePeaks, CEC2013>(m, "FivePeaks", nb::is_final());
    nb::class_<ModifiedRastrigin, CEC2013>(m, "ModifiedRastrigin", nb::is_final());
    nb::class_<Shubert, CEC2013>(m, "Shubert", nb::is_final());
    nb::class_<SixHumpCamelback, CEC2013>(m, "SixHumpCamelback", nb::is_final());
    nb::class_<UnevenEqualMaxima, CEC2013>(m, "UnevenEqualMaxima", nb::is_final());
    nb::class_<Vincent, CEC2013>(m, "Vincent", nb::is_final());
    nb::class_<CompositionFunction, CEC2013>(m, "CEC2013CompositionFunction", nb::is_final());
}

void define_cec2022_problems(nb::module_ &m)
{
    nb::class_<CEC2022, RealSingleObjective>(m, "CEC2022",
                                             R"pbdoc(
            Functions from the CEC2022 2022 conference.
        )pbdoc")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<CEC2022, int, int>::instance().create(name, iid, dim);
            },
            nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"),
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
            nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"),
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
        .def_prop_ro_static(
            "problems", [](nb::object) { return ioh::common::Factory<CEC2022, int, int>::instance().map(); },
            "All registered problems");

    nb::class_<cec2022::Zakharov, CEC2022>(m, "CEC2022Zakharov", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::Rosenbrock, CEC2022>(m, "CEC2022Rosenbrock", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::SchafferF7, CEC2022>(m, "CEC2022SchafferF7", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::Rastrigin, CEC2022>(m, "CEC2022Rastrigin", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::Levy, CEC2022>(m, "CEC2022Levy", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::HybridFunction1, CEC2022>(m, "CEC2022HybridFunction1", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::HybridFunction2, CEC2022>(m, "CEC2022HybridFunction2", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::HybridFunction3, CEC2022>(m, "CEC2022HybridFunction3", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::CompositionFunction1, CEC2022>(m, "CEC2022CompositionFunction1", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::CompositionFunction2, CEC2022>(m, "CEC2022CompositionFunction2", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::CompositionFunction3, CEC2022>(m, "CEC2022CompositionFunction3", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));

    nb::class_<cec2022::CompositionFunction4, CEC2022>(m, "CEC2022CompositionFunction4", nb::is_final())
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));
}


enum class SamplerType
{
    UNIFORM,
    SOBOL,
    HALTON
};

template <typename T>
void star_discrepancy_init(T *t, const int instance, const int n_variables, const int n_samples,
                           const SamplerType sampler_type)
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
    new (t) T(0, instance, n_variables, py_name, grid);
}


template <typename T, typename P>
void define_star_discrepancy_problem(nb::module_ &m, const std::string &name)
{
    nb::class_<T, P>(m, name.c_str(), R"pbdoc(
            Star-discrepancy problems

            The 𝐿_infinity star discrepancy is a measure for the regularity of a finite set
            of points taken from [0, 1)^d. Calculating this measure is challenging, and exact algorithm
            fall short even for relatively small grids. This suite provides access to a set of these problems
            to tackle the problem from an optimization perspective.

            There are two equivalent representations for these problems: Integer-based and Real-valued.

            For the Real-valued, the goal is to find the a point in [0,1)^d which minimizes the resulting discepancy
            directly.

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
        .def("__init__", &star_discrepancy_init<T>, nb::arg("instance") = 1, nb::arg("n_variables") = 5,
             nb::arg("n_samples") = 5, nb::arg("sampler_type") = SamplerType::UNIFORM,
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
        // .def_prop_ro("grid",
        //                        [](const T &self) {
        //                            const auto grid = self.get_grid();
        //                            const auto n = grid.size(), m = grid[0].size();

        //                            nb::ndarray<double, nb::array::c_style> arr({n, m});

        //                            auto ra = arr.mutable_unchecked();

        //                            for (size_t i = 0; i < n; i++)
        //                                for (size_t j = 0; j < m; j++)
        //                                    ra(i, j) = grid[i][j];

        //                            return arr;
        //                        })
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<T, int, int>::instance().create(name, iid, dim);
            },
            nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"),
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
            nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"), R"pbdoc(
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
        .def_prop_ro_static(
            "problems", [](nb::object) { return ioh::common::Factory<T, int, int>::instance().map(); },
            "All registered problems");
    ;
}


void define_star_discrepancy_problems(nb::module_ &m)
{
    using namespace ioh::problem::star_discrepancy;
    using namespace ioh::common::random::sampler;

    nb::enum_<SamplerType>(m, "StarDiscrepancySampler",
                           "Methods which can be used to sample the initial grids for the star discrepancy problems")
        .value("UNIFORM", SamplerType::UNIFORM)
        .value("HALTON", SamplerType::HALTON)
        .value("SOBOL", SamplerType::SOBOL)
        .export_values();

    define_star_discrepancy_problem<real::StarDiscrepancy, RealSingleObjective>(m, "RealStarDiscrepancy");
    define_star_discrepancy_problem<integer::StarDiscrepancy, IntegerSingleObjective>(m, "IntegerStarDiscrepancy");
}

void define_many_affine(nb::module_ &m)
{
    nb::class_<ioh::problem::bbob::ManyAffine, ioh::problem::RealSingleObjective>(m, "ManyAffine")
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"))
        .def(nb::init<std::vector<double>, std::array<double, 24>, std::array<int, 24>, int, std::array<double, 24>>(),
             nb::arg("xopt"), nb::arg("weights"), nb::arg("instances"), nb::arg("n_variables"),
             nb::arg("scale_factors") = ioh::problem::bbob::ManyAffine::default_scales)
        .def_prop_ro("weights", &ioh::problem::bbob::ManyAffine::get_weights)
        .def_prop_ro("instances", &ioh::problem::bbob::ManyAffine::get_instances)
        .def_prop_ro("scale_factors", &ioh::problem::bbob::ManyAffine::get_scale_factors)
        .def_prop_ro("sub_problems", &ioh::problem::bbob::ManyAffine::get_problems)
        .def_prop_ro("function_values", &ioh::problem::bbob::ManyAffine::get_function_values);
}


void define_funnel(nb::module_ &m)
{
    using namespace ioh::problem;
    nb::class_<funnel::DoubleFunnel, RealSingleObjective>(m, "DoubleFunnel")
        .def_prop_ro("d", &funnel::DoubleFunnel::d)
        .def_prop_ro("s", &funnel::DoubleFunnel::s)
        .def_prop_ro("u1", &funnel::DoubleFunnel::u1)
        .def_prop_ro("u2", &funnel::DoubleFunnel::u2);

    nb::class_<funnel::DoubleSphere, funnel::DoubleFunnel>(m, "DoubleSphere")
        .def(nb::init<int, double, double>(), nb::arg("n_variables"), nb::arg("d") = 0.0, nb::arg("s") = 1.0);

    nb::class_<funnel::DoubleRastrigin, funnel::DoubleFunnel>(m, "DoubleRastrigin")
        .def(nb::init<int, double, double>(), nb::arg("n_variables"), nb::arg("d") = 0.0, nb::arg("s") = 1.0);
}

void define_bbob(nb::module_ &m)
{
    define_bbob_problems<ioh::problem::BBOB>(m);
    define_bbob_problems<ioh::problem::SBOX>(m, "SBOX", true);
}


void define_dynamic_bin_val_problem(nb::module_ &m)
{
    using namespace ioh::problem::dynamic_bin_val;

    nb::class_<DynamicBinVal, IntegerSingleObjective>(m, "DynamicBinVal")
        .def_static(
            "create",
            [](const std::string &name, int iid, int dim) {
                return ioh::common::Factory<DynamicBinVal, int, int>::instance().create(name, iid, dim);
            },
            nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"),
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
            nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"),
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
        .def_prop_ro_static(
            "problems", [](nb::object) { return ioh::common::Factory<DynamicBinVal, int, int>::instance().map(); },
            "All registered problems")
        .def("step", &DynamicBinVal::step, R"pbdoc(
            Step the dynamic binary value problem forward by one timestep, and permute the weights randomly.

            Returns
            -------
            int
                The current timestep after the step.
        )pbdoc")
        .def_ro("time_step", &DynamicBinVal::time_step)
        .def_ro("weights", &DynamicBinVal::weights);

    const auto doc =
        R"pbdoc(Dynamic BinVal. Details: https://link.springer.com/article/10.1007/s42979-022-01203-z )pbdoc";

    nb::class_<Uniform, DynamicBinVal>(m, "DynamicBinValUniform", doc)
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));
    nb::class_<PowersOfTwo, DynamicBinVal>(m, "DynamicBinValPowersOfTwo", doc)
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));
    nb::class_<Pareto, DynamicBinVal>(m, "DynamicBinValPareto", doc)
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"))
        .def_prop_ro("pareto_shape", &Pareto::get_pareto_shape);

    nb::class_<Ranking, DynamicBinVal>(m, "DynamicBinValRanking", doc)
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"))
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