#pragma once

#include <limits>
#include <ostream>
#include <string>
#include <utility>
#include <vector>


#include "transformation.hpp"
#include "ioh/common.hpp"


namespace ioh {
    namespace problem {

        /*
        * What blocks has a problem?:
        *  - MetaData:
        *      - suite_related: id, suite_name -> we need an Wrapper for logger, to make a `Dummy Suite` (for info file)
        *      - problem_related: instance, dim~number_of_variables, name, minmax, datatype, objectives -> call constructor)
        *
        *          class Solution(a good name) { -> get target?
        *              x, y
        *          }
        *         (think of paralization in python)
        *  - Constraints
        *  - State:`                                      (protected/private)              (public)
        *      - evaluations, optimalFound(method), current_best_internal (Solution), current_best (Solution) (this is transformed)
        *      - Logger info:                                                      (this is return by evaluate)
        *           R"#("function evaluation" "current f(x)" "best-so-far f(x)" "current af(x)+b" "best af(x)+b")#";
        *                                      values that are the same across instances (iid independent: untransformed); transformed(iid dependent)
        *                                           for BBOB these are also scaled with optimum (goes to zero, not for pbo)
        *          Make a different implementation for current_best & current_best_transformed for COCO and PBO (default)
        *  -
        *
        *  void calc_optimal()
        *  reset
           virtual void prepare() {}
           virtual void customize_optimal() -> child class
           virtual void transform_variables() -> self
           virtual void transform_objective()
           double evaluate -> operator overload ()
           protected virtual double evaluate(const std::vector<InputType>& x) = 0;

           accessor naming:
               named after the private variable
           members:
               not: problem_name but: name

        */


        template <typename T>
        struct Solution {
            std::vector<T> x;
            std::vector<double> y;

            friend std::ostream &operator<<(std::ostream &os, const Solution &obj) {
                return os
                       << "x: " << common::vector_to_string(obj.x)
                       << " y: " << common::vector_to_string(obj.y);
            }
        };

        template <typename T>
        struct Constraint {

            std::vector<T> ub;
            std::vector<T> lb;

            Constraint(const std::vector<T> upper, const std::vector<T> lower)
                : ub(upper), lb(lower) {
            }

            explicit Constraint(const int size = 1,
                                const T upper = std::numeric_limits<T>::infinity(),
                                const T lower = -std::numeric_limits<T>::infinity()
                )
                : Constraint(std::vector<T>(size, upper), std::vector<T>(size, lower)) {
            }


            void check_size(const int s) {
                if (ub.size() == lb.size() == size_t{1}) {
                    ub = std::vector<T>(s, ub.at(0));
                    lb = std::vector<T>(s, lb.at(0));
                }

                if ((ub.size() != static_cast<size_t>(s)) || (ub.size() != lb.size()))
                    std::cout << "Bound dimension is wrong" << std::endl;
            }

            friend std::ostream &operator<<(std::ostream &os, const Constraint &obj) {
                return os
                       << "ub: " << common::vector_to_string(obj.ub)
                       << " lb: " << common::vector_to_string(obj.lb);
            }
        };

        template <typename T>
        struct MetaData {
            int instance;
            std::string name;
            common::OptimizationType optimization_type;
            Solution<T> objective;
            int number_of_variables;
            int number_of_objectives;
            T initial_objective_value;

            MetaData(const int instance, std::string name, const Solution<T> &objective,
                     const common::OptimizationType optimization_type = common::OptimizationType::minimization
                )
                : instance(instance),
                  name(std::move(name)),
                  optimization_type(optimization_type),
                  objective(objective),
                  number_of_variables(static_cast<int>(objective.x.size())),
                  number_of_objectives(static_cast<int>(objective.y.size())),
                  initial_objective_value(optimization_type == common::OptimizationType::minimization
                                              ? std::numeric_limits<T>::infinity()
                                              : -std::numeric_limits<T>::infinity()) {

            }

            friend std::ostream &operator<<(std::ostream &os, const MetaData &obj) {
                return os
                       << "instance: " << obj.instance
                       << " name: " << obj.name
                       << " optimization_type: " << (obj.optimization_type == common::OptimizationType::minimization
                                                         ? "minimization"
                                                         : "maximization")
                       << " number_of_variables: " << obj.number_of_variables
                       << " number_of_objectives: " << obj.number_of_objectives;
            }
        };

        template <typename T>
        struct State {
        private:
            Solution<T> initial_solution;
        public:
            int evaluations = 0;
            bool optimum_found = false;
            Solution<T> current_best_internal;
            Solution<T> current_best;

            State() = default;

            State(Solution<T> initial_solution)
                : initial_solution(std::move(initial_solution)) {
                reset();
            }

            void reset() {
                evaluations = 0;
                current_best = initial_solution;
                current_best_internal = initial_solution;
                optimum_found = false;
            }

            void update(const std::vector<T> &x, const std::vector<T> &x_internal, const std::vector<double> &y,
                        const std::vector<double> &y_internal, const MetaData<T> &meta_data) {
                ++evaluations;
                if (common::compare_objectives(y, current_best.y, meta_data.optimization_type)) {
                    current_best_internal = {x_internal, y_internal};
                    current_best = {x, y};

                    if (common::compare_vector(meta_data.objective.y, y))
                        optimum_found = true;
                }
            }

            friend std::ostream &operator<<(std::ostream &os, const State &obj) {
                return os
                       << "evaluations: " << obj.evaluations
                       << " optimum_found: " << std::boolalpha << obj.optimum_found
                       << " current_best: " << obj.current_best;
            }
        };

        template <typename T>
        class Problem {

            bool check_input(const std::vector<T> &x) {
                if (x.empty()) {
                    common::log::warning("The solution is empty.");
                    return false;
                }
                if (x.size() != meta_data_.number_of_variables) {
                    common::log::warning("The dimension of solution is incorrect.");
                    return false;
                }

                if (common::all_finite<T>(x))
                    return true;

                if (common::has_nan<T>(x)) {
                    common::log::warning("The solution contains NaN.");
                    return false;
                }
                if (common::has_inf<T>(x)) {
                    common::log::warning("The solution contains Inf.");
                    return false;
                }

                common::log::warning("The solution contains invalid values.");
                return false;
            }

        protected:
            MetaData<T> meta_data_;
            Constraint<T> constraint_;
            State<T> state_;

            [[nodiscard]]
            virtual std::vector<double> evaluate(std::vector<T> &x) = 0;

        public:
            Problem(const int instance, const std::string &name, Solution<T> objective,
                    const common::OptimizationType optimization_type = common::OptimizationType::minimization,
                    Constraint<T> constraint = Constraint<T>()
                )
                : meta_data_(instance, name, std::move(objective), optimization_type),
                  constraint_(std::move(constraint)) {
                state_ = {{
                    std::vector<T>(meta_data_.number_of_variables, std::numeric_limits<T>::signaling_NaN()),
                    std::vector<double>(meta_data_.number_of_objectives, meta_data_.initial_objective_value)
                }};
                constraint_.check_size(meta_data_.number_of_variables);
            }

            virtual ~Problem() = default;

            virtual void reset() {
                state_.reset();
            }

            virtual std::vector<T> transform_variables(std::vector<T> x) {
                return x;
            }

            virtual std::vector<T> transform_objectives(std::vector<double> y) {
                return y;
            }

            std::vector<double> operator()(const std::vector<T> &x) {
                if (!check_input(x)) {
                    return std::vector<T>(meta_data_.number_of_objectives, std::numeric_limits<T>::signaling_NaN());
                }
                auto x_internal = transform_variables(x);
                auto y_internal = evaluate(x_internal);
                auto y = transform_objectives(y_internal);
                state_.update(x, x_internal, y, y_internal, meta_data_);
                return y;

            }

            [[nodiscard]]
            MetaData<T> meta_data() const {
                return meta_data_;
            }

            [[nodiscard]]
            State<T> state() const {
                return state_;
            }

            friend std::ostream &operator<<(std::ostream &os, const Problem &obj) {
                return os
                       << "Problem(\n\t" << obj.meta_data_
                       << "\n\tconstraint: " << obj.constraint_
                       << "\n\tstate: " << obj.state_ << "\n)";
            }
        };


        class COCO : public Problem<double> {

        protected:
            long seed_;

        public:
            COCO(const int problem_id, const int instance, const std::string &name, Solution<double> objective,
                 const common::OptimizationType optimization_type = common::OptimizationType::minimization)
                : Problem(instance, name, std::move(objective), optimization_type),
                  seed_((problem_id == 4 || problem_id == 18 ? problem_id - 1 : problem_id) + 10000 * instance) {

            }
        };

        class Sphere : public COCO {

        protected:
            std::vector<double> evaluate(std::vector<double> &x) override {
                std::vector<double> result{0.0};
                for (const auto i : x)
                    result[0] += i * i;
                return result;
            }

        public:
            Sphere(const int instance, const int dimension)
                : COCO(1, instance, "Sphere", calculate_objective(dimension, instance)) {
            }

            std::vector<double> transform_objectives(std::vector<double> y) override {
                transformation::coco::transform_obj_shift_evaluate_function(y, meta_data_.objective.y.at(0));
                return y;
            }

            std::vector<double> transform_variables(std::vector<double> x) override {
                transformation::coco::transform_vars_shift_evaluate_function(x, meta_data_.objective.x);
                return x;
            }

            [[nodiscard]]
            Solution<double> calculate_objective(const int dimension, const int instance) const {
                std::vector<double> x(dimension, 0);
                transformation::coco::bbob2009_compute_xopt(x, seed_, dimension);
                return Solution<double>{
                    x, {transformation::coco::bbob2009_compute_fopt(1, instance)}
                };
            }
        };
    }
}
