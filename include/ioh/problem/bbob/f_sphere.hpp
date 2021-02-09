#pragma once
#include "bbob_base.hpp"


namespace ioh {
    namespace problem {
        namespace bbob {
            class Sphere : public bbob_base {
            public:
                Sphere(int instance_id = IOH_DEFAULT_INSTANCE,
                       int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(1, "Sphere", instance_id, dimension) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    transformation::coco::bbob2009_compute_xopt(
                        xopt_, rseed_, n_);
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    auto result = 0.0;
                    for (size_t i = 0; i < n_; ++i)
                        result += x[i] * x[i];
                    return result;
                }

                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, xopt_);
                }


                static Sphere *create(int instance_id = IOH_DEFAULT_INSTANCE,
                                      int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Sphere(instance_id, dimension);
                }
            };
        }
    }
}
