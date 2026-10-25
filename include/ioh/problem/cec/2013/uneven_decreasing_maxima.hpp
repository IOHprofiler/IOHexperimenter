#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class UnevenEqualMaxima final : public CEC2013
        {
        public:
            //! The registry entries (see registry_entries_of): the dimension and the instance are fixed
            static std::vector<RegistryEntry<UnevenEqualMaxima>> registry_entries();

        protected:
            double inner_evaluate(const std::vector<double> &z) override
            {
                return std::exp(-2.0 * std::log(2) * std::pow((z[0] - 0.08) / 0.854, 2)) *
                    std::pow(std::sin(5 * IOH_PI * (std::pow(z[0], 0.75) - 0.05)), 6);
            }

        public:
            UnevenEqualMaxima(const int instance, const int n_variables) :
                CEC2013(1103, instance, n_variables, "CEC2013UnevenEqualMaxima", 0, 1, {
                    {{0.07969978}, 0.999999828},
                })
            {
            }

        };
    } // namespace cec2013


    inline std::vector<RegistryEntry<cec2013::UnevenEqualMaxima>> cec2013::UnevenEqualMaxima::registry_entries()
    {
        return {{1103, "UnevenEqualMaxima1103", [](int, int) { return std::make_shared<cec2013::UnevenEqualMaxima>(1, 1); }}};
    }

} // namespace ioh::problem