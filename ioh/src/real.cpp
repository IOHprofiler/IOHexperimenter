#include "common.hpp"

#include <cctype>

#include "registry/families.hpp"

namespace ioh_python::real
{
    std::string to_lower(const std::string &s)
    {
        auto res = s;
        std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::tolower(c); });
        return res;
    }

    //! The docstring of a problem class defined from a type list, nullptr for a class without one
    template <class T>
    struct class_doc
    {
        static constexpr const char *value = nullptr;
    };

    template <class V>
    struct class_doc<ioh::problem::bbob::Sphere<V>>
    {
        static constexpr const char *value = "Sphere function";
    };

    //! A final problem class T with base Base named name, with its docstring from class_doc
    template <class T, class Base>
    nb::class_<T, Base> final_class(nb::handle scope, const char *name)
    {
        if constexpr (class_doc<T>::value != nullptr)
            return nb::class_<T, Base>(scope, name, nb::is_final(), class_doc<T>::value);
        else
            return nb::class_<T, Base>(scope, name, nb::is_final());
    }

    //! Defines the problem classes Ts of a family (type list), with base Base, named by their registry_name, and
    //! constructed from (instance, n_variables)
    template <class Base, class... Ts>
    void define_family(nb::handle scope, ioh::registry::type_list<Ts...>)
    {
        (final_class<Ts, Base>(scope, Ts::registry_name)
             .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables")),
         ...);
    }

    //! Defines the static methods create (by name and by id) and the static property problems of a problem family
    template <class P, class Base>
    void define_factory(nb::class_<P, Base> &c, const char *name_doc, const char *id_doc)
    {
        using Factory = ioh::common::Factory<P, int, int>;
        c.def_static(
             "create",
             [](const std::string &name, int iid, int dim) { return Factory::instance().create(name, iid, dim); },
             nb::arg("problem_name"), nb::arg("instance_id"), nb::arg("dimension"), name_doc)
            .def_static(
                "create", [](int id, int iid, int dim) { return Factory::instance().create(id, iid, dim); },
                nb::arg("problem_id"), nb::arg("instance_id"), nb::arg("dimension"), id_doc)
            .def_prop_ro_static(
                "problems", [](nb::handle) { return Factory::instance().map(); }, "All registered problems");
    }

    // The docstrings of create, as on master: the BBOB and star discrepancy families, ...
    constexpr const char *bbob_create_name_doc =
        "\n"
        "                Create a problem instance\n"
        "\n"
        "                Parameters\n"
        "                ----------\n"
        "                    problem_name: str\n"
        "                        a string indicating the problem name. \n"
        "                    instance_id: int\n"
        "                        an integer identifier of the problem instance\n"
        "                    dimension: int\n"
        "                        the dimensionality of the search space\n"
        "            ";

    constexpr const char *bbob_create_id_doc =
        "\n"
        "                Create a problem instance\n"
        "\n"
        "                Parameters\n"
        "                ----------\n"
        "                    problem_name: int\n"
        "                        a string indicating the problem name. \n"
        "                    instance_id: int\n"
        "                        an integer identifier of the problem instance\n"
        "                    dimension: int\n"
        "                        the dimensionality of the search space\n"
        "            ";

    // ... the CEC families, ...
    constexpr const char *cec_create_name_doc = R"pbdoc(
            Create a problem instance

            Parameters
            ----------
                problem_name: str
                    a string indicating the problem name.
                instance_id: int
                    an integer identifier of the problem instance
                dimension: int
                    the dimensionality of the search space
        )pbdoc";

    constexpr const char *cec_create_id_doc = R"pbdoc(
            Create a problem instance

            Parameters
            ----------
                problem_id: int
                    a number indicating the problem numeric identifier.
                instance_id: int
                    an integer identifier of the problem instance
                dimension: int
                    the dimensionality of the search space
        )pbdoc";

    // ... and DynamicBinVal
    constexpr const char *dynamic_bin_val_create_name_doc = R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_name: str
                        a string indicating the problem name.
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
        )pbdoc";

    constexpr const char *dynamic_bin_val_create_id_doc = R"pbdoc(
                Create a problem instance

                Parameters
                ----------
                    problem_id: int
                        a number indicating the problem numeric identifier.
                    instance_id: int
                        an integer identifier of the problem instance
                    dimension: int
                        the dimensionality of the search space
        )pbdoc";

    constexpr const char *bbob_doc = R"pbdoc(
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

        )pbdoc";

    //! Defines the base class P (problem::BBOB or problem::SBOX) named name, and its 24 functions, in a submodule
    //! named after it if submodule is true
    template <typename P>
    void define_bbob_problems(nb::module_ &mi, const std::string &name = "BBOB", const bool submodule = false)
    {
        nb::class_<P, ioh::problem::RealSingleObjective> base(mi, name.c_str(), bbob_doc);
        define_factory(base, bbob_create_name_doc, bbob_create_id_doc);

        auto m = mi;
        if (submodule)
            m = mi.def_submodule(to_lower(name).c_str(), ("module containing " + name + " problems").c_str());

        define_family<P>(m, ioh::registry::bbob_classes<P>{});
    }

    enum class SamplerType
    {
        UNIFORM,
        SOBOL,
        HALTON
    };

    //! Constructs the star discrepancy problem T in t, on a grid sampled by sampler_type
    template <typename T>
    void star_discrepancy_init(T *t, const int instance, const int n_variables, const int n_samples,
                               const SamplerType sampler_type)
    {
        using namespace ioh::common::random::sampler;
        namespace sd = ioh::problem::star_discrepancy;

        std::vector<std::vector<double>> grid;
        switch (sampler_type)
        {
        case SamplerType::UNIFORM:
            grid = sd::generate_grid<Uniform<double>>(instance, n_variables, n_samples);
            break;
        case SamplerType::HALTON:
            grid = sd::generate_grid<Halton>(instance, n_variables, n_samples);
            break;
        default:
        case SamplerType::SOBOL:
            grid = sd::generate_grid<Sobol>(instance, n_variables, n_samples);
        }
        const std::string py_name =
            std::is_base_of_v<ioh::problem::RealSingleObjective, T> ? "RealStarDiscrepancy" : "IntegerStarDiscrepancy";
        new (t) T(0, instance, n_variables, py_name, grid);
    }

    //! The grid of a star discrepancy problem, as a new (n_points, n_variables) numpy array
    template <typename T>
    nb::ndarray<nb::numpy, double, nb::ndim<2>> star_discrepancy_grid(const T &self)
    {
        const auto grid = self.get_grid();
        const size_t n = grid.size(), m = n ? grid[0].size() : 0;

        auto *data = new double[n * m];
        for (size_t i = 0; i < n; i++)
            std::copy(grid[i].begin(), grid[i].end(), data + i * m);

        nb::capsule owner(data, [](void *p) noexcept { delete[] static_cast<double *>(p); });
        return nb::ndarray<nb::numpy, double, nb::ndim<2>>(data, {n, m}, owner);
    }

    template <typename T, typename P>
    void define_star_discrepancy_problem(nb::module_ &m, const std::string &name)
    {
        nb::class_<T, P> c(m, name.c_str(),
                         "\n"
                         "            Star-discrepancy problems\n"
                         "             \n"
                         "            The 𝐿_infinity star discrepancy is a measure for the regularity of a finite set\n"
                         "            of points taken from [0, 1)^d. Calculating this measure is challenging, and exact algorithm\n"
                         "            fall short even for relatively small grids. This suite provides access to a set of these problems\n"
                         "            to tackle the problem from an optimization perspective.\n"
                         "\n"
                         "            There are two equivalent representations for these problems: Integer-based and Real-valued. \n"
                         "\n"
                         "            For the Real-valued, the goal is to find the a point in [0,1)^d which minimizes the resulting discepancy directly.\n"
                         "\n"
                         "            The Integer-valued version takes advantage of the fact that for each dimension, the optimium \n"
                         "            has a value matching an existing point int the grid. As such, the search for a point in the grid can \n"
                         "            be reduced to finding in each dimension the index of the coordinate, resulting in a search domain \n"
                         "            [0,n]^d, where n is the number of gridpoints. \n"
                         "\n"
                         "\n"
                         "            Reference\n"
                         "            ---------\n"
                         "            [Clement23] Clement, Francois, Diederick Vermetten, Jacob de Nobel, Alexandre Jesus, \n"
                         "            Luís Paquete, and Carola Doerr. \"Computing Star Discrepancies with \n"
                         "            Numerical Black-Box Optimization Algorithms.\"\n"
                         "\n"
                         "        ");
        c.def("__init__", &star_discrepancy_init<T>, nb::arg("instance") = 1, nb::arg("n_variables") = 5,
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
            // automatic: the array owns its data (def_prop_ro's reference_internal would fail on it)
            .def_prop_ro("grid", &star_discrepancy_grid<T>, nb::rv_policy::automatic);
        define_factory(c, bbob_create_name_doc, bbob_create_id_doc);
    }
} // namespace ioh_python::real


void define_cec2013_problems(nb::module_ &m)
{
    using ioh::problem::CEC2013;
    namespace cec2013 = ioh::problem::cec2013;

    nb::class_<CEC2013, ioh::problem::RealSingleObjective> c(m, "CEC2013",
                                                             "\n"
                                                             "            Functions from the CEC2013 conference. \n"
                                                             "        ");
    ioh_python::real::define_factory(c, ioh_python::real::cec_create_name_doc, ioh_python::real::cec_create_id_doc);
    c.def_ro("n_optima", &CEC2013::n_optima)
        .def_ro("optima", &CEC2013::optima)
        .def_ro("single_global_optimum", &CEC2013::single_global_optimum)
        .def_ro("sphere_limit", &CEC2013::sphere_limit)
        .def_ro("rho", &CEC2013::rho)
        .def("set_optimum", &CEC2013::set_optimum, nb::arg("i"), nb::arg("single_global") = true);

    // As on master: no constructors, Himmelblau has no class of its own, and CompositionFunction is renamed
    nb::class_<cec2013::EqualMaxima, CEC2013>(m, "EqualMaxima", nb::is_final());
    nb::class_<cec2013::FivePeaks, CEC2013>(m, "FivePeaks", nb::is_final());
    nb::class_<cec2013::ModifiedRastrigin, CEC2013>(m, "ModifiedRastrigin", nb::is_final());
    nb::class_<cec2013::Shubert, CEC2013>(m, "Shubert", nb::is_final());
    nb::class_<cec2013::SixHumpCamelback, CEC2013>(m, "SixHumpCamelback", nb::is_final());
    nb::class_<cec2013::UnevenEqualMaxima, CEC2013>(m, "UnevenEqualMaxima", nb::is_final());
    nb::class_<cec2013::Vincent, CEC2013>(m, "Vincent", nb::is_final());
    nb::class_<cec2013::CompositionFunction, CEC2013>(m, "CEC2013CompositionFunction", nb::is_final());
}

void define_cec2022_problems(nb::module_ &m)
{
    using ioh::problem::CEC2022;

    nb::class_<CEC2022, ioh::problem::RealSingleObjective> c(m, "CEC2022",
                                                             R"pbdoc(
            Functions from the CEC2022 2022 conference.
        )pbdoc");
    ioh_python::real::define_factory(c, ioh_python::real::cec_create_name_doc, ioh_python::real::cec_create_id_doc);

    ioh_python::real::define_family<CEC2022>(m, ioh::registry::cec2022_classes{});
}

void define_star_discrepancy_problems(nb::module_ &m)
{
    using ioh_python::real::SamplerType;
    namespace sd = ioh::problem::star_discrepancy;

    // A Python enum (not an int enum, as on master, where StarDiscrepancySampler.UNIFORM == 0 is False); int(...)
    // keeps working through __int__ and __index__
    nb::enum_<SamplerType>(m, "StarDiscrepancySampler",
                           "Methods which can be used to sample the initial grids for the star discrepancy problems")
        .value("UNIFORM", SamplerType::UNIFORM)
        .value("HALTON", SamplerType::HALTON)
        .value("SOBOL", SamplerType::SOBOL)
        .export_values()
        .def("__int__", [](SamplerType s) { return static_cast<int>(s); })
        .def("__index__", [](SamplerType s) { return static_cast<int>(s); });

    ioh_python::real::define_star_discrepancy_problem<sd::real::StarDiscrepancy, ioh::problem::RealSingleObjective>(
        m, "RealStarDiscrepancy");
    ioh_python::real::define_star_discrepancy_problem<sd::integer::StarDiscrepancy,
                                                      ioh::problem::IntegerSingleObjective>(m,
                                                                                            "IntegerStarDiscrepancy");
}

void define_many_affine(nb::module_ &m)
{
    using ioh::problem::bbob::ManyAffine;

    nb::class_<ManyAffine, ioh::problem::RealSingleObjective>(m, "ManyAffine")
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"))
        .def(
            "__init__",
            [](ManyAffine *self, const ioh_python::Array1D<double> &xopt, const std::array<double, 24> &weights,
               const std::array<int, 24> &instances, const int n_variables,
               const std::array<double, 24> &scale_factors) {
                new (self) ManyAffine(ioh_python::to_vector<double>(xopt), weights, instances, n_variables,
                                      scale_factors);
            },
            nb::arg("xopt"), nb::arg("weights"), nb::arg("instances"), nb::arg("n_variables"),
            nb::arg("scale_factors") = ManyAffine::default_scales)
        .def(nb::init<std::vector<double>, std::array<double, 24>, std::array<int, 24>, int, std::array<double, 24>>(),
             nb::arg("xopt"), nb::arg("weights"), nb::arg("instances"), nb::arg("n_variables"),
             nb::arg("scale_factors") = ManyAffine::default_scales)
        .def_prop_ro("weights", &ManyAffine::get_weights)
        .def_prop_ro("instances", &ManyAffine::get_instances)
        .def_prop_ro("scale_factors", &ManyAffine::get_scale_factors)
        .def_prop_ro("sub_problems", &ManyAffine::get_problems)
        .def_prop_ro("function_values", &ManyAffine::get_function_values);
}


void define_funnel(nb::module_ &m)
{
    namespace funnel = ioh::problem::funnel;

    nb::class_<funnel::DoubleFunnel, ioh::problem::RealSingleObjective>(m, "DoubleFunnel")
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
    ioh_python::real::define_bbob_problems<ioh::problem::BBOB>(m);
    ioh_python::real::define_bbob_problems<ioh::problem::SBOX>(m, "SBOX", true);
}


void define_dynamic_bin_val_problem(nb::module_ &m)
{
    namespace dbv = ioh::problem::dynamic_bin_val;
    using ioh::problem::DynamicBinVal;

    nb::class_<DynamicBinVal, ioh::problem::IntegerSingleObjective> c(m, "DynamicBinVal");
    ioh_python::real::define_factory(c, ioh_python::real::dynamic_bin_val_create_name_doc,
                                     ioh_python::real::dynamic_bin_val_create_id_doc);
    c.def("step", &DynamicBinVal::step, R"pbdoc(
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

    nb::class_<dbv::Uniform, DynamicBinVal>(m, "DynamicBinValUniform", doc)
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));
    nb::class_<dbv::PowersOfTwo, DynamicBinVal>(m, "DynamicBinValPowersOfTwo", doc)
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"));
    nb::class_<dbv::Pareto, DynamicBinVal>(m, "DynamicBinValPareto", doc)
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"))
        .def_prop_ro("pareto_shape", &dbv::Pareto::get_pareto_shape);

    const auto rank_doc = R"pbdoc(
            Sort a list of bitstrings in lexicographical order in-place.

            Parameters
            ----------
            bitstrings : list
                A list of bitstrings to sort.
        )pbdoc";

    // rank and rank_indices take a list of bitstrings or a two-dimensional array (one bitstring per row)
    nb::class_<dbv::Ranking, DynamicBinVal>(m, "DynamicBinValRanking", doc)
        .def(nb::init<int, int>(), nb::arg("instance"), nb::arg("n_variables"))
        .def(
            "rank",
            [](dbv::Ranking &self, const ioh_python::IntArray2D &bitstrings) {
                return self.rank(ioh_python::to_vectors<int>(bitstrings));
            },
            ioh_python::array_arg<int>(), rank_doc)
        .def("rank", &dbv::Ranking::rank, rank_doc)
        .def(
            "rank_indices",
            [](dbv::Ranking &self, const ioh_python::IntArray2D &bitstrings) {
                return self.rank_indices(ioh_python::to_vectors<int>(bitstrings));
            },
            ioh_python::array_arg<int>(), rank_doc)
        .def("rank_indices", &dbv::Ranking::rank_indices, rank_doc)
        .def("get_comparison_ordering", &dbv::Ranking::get_comparison_ordering, R"pbdoc(
            Get the current comparison ordering vector.

            Returns
            -------
            list of int
                The current state of the comparison ordering vector.
        )pbdoc");
}
