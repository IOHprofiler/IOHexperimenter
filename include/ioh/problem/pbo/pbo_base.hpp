#pragma once

#include "ioh/problem/base.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class pbo_base : public base<int> {
            protected:
                int n_;
            public:
                pbo_base() = default;

                pbo_base(int problem_id, std::string problem_name,
                         int instance_id = IOH_DEFAULT_INSTANCE,
                         int dimension = IOH_DEFAULT_DIMENSION
                    ): n_(dimension) {
                    set_problem_id(problem_id);
                    set_instance_id(instance_id);
                    set_problem_name(problem_name);
                    set_problem_type("pseudo_Boolean_problem");
                    set_number_of_objectives(1);
                    set_lowerbound(0);
                    set_upperbound(1);
                    set_number_of_variables(dimension);
                }

                void objectives_transformation(const std::vector<int> &x,
                                               std::vector<double> &y,
                                               const int transformation_id,
                                               const int instance_id) override {
                    if (instance_id > 1) {
                        transformation::methods::transform_obj_scale(
                            y, instance_id);
                        transformation::methods::transform_obj_shift(
                            y, instance_id);
                    }
                }

                void variables_transformation(std::vector<int> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    if (instance_id > 1 && instance_id <= 50) {
                        transformation::methods::transform_vars_xor(
                            x, instance_id);
                    } else if (instance_id > 50 && instance_id <= 100) {
                        transformation::methods::transform_vars_sigma(
                            x, instance_id);
                    }
                }
            };
        }
    }
}
