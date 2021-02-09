#include "bbob_base.hpp"

namespace ioh {
    namespace problem {
        namespace bbob {
            class Bent_Cigar : public bbob_base {
            public:
                Bent_Cigar(int instance_id = IOH_DEFAULT_INSTANCE,
                           int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(12, "Bent_Cigar", instance_id, dimension) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    transformation::coco::bbob2009_compute_xopt(
                        xopt_, rseed_ + 1000000, n_);
                    transformation::coco::bbob2009_compute_rotation(
                        rot1_, rseed_ + 1000000, n_);
                    transformation::coco::bbob2009_copy_rotation_matrix(
                        rot1_, m_, b_, n_);
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    static const auto condition = 1.0e6;
                    auto result = x[0] * x[0];
                    for (size_t i = 1; i < n_; ++i) {
                        result += condition * x[i] * x[i];
                    }
                    return result;
                }

                void variables_transformation(std::vector<double> &x,
                                              const int transformation_id,
                                              const int instance_id) override {
                    transformation::coco::transform_vars_shift_evaluate_function(
                        x, xopt_);
                    transformation::coco::transform_vars_affine_evaluate_function(
                        x, m_, b_);
                    transformation::coco::transform_vars_asymmetric_evaluate_function(
                        x, 0.5);
                    transformation::coco::transform_vars_affine_evaluate_function(
                        x, m_, b_);
                }

                static Bent_Cigar *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Bent_Cigar(instance_id, dimension);
                }
            };
        }
    }
}
