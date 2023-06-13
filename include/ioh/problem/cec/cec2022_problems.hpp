#pragma once

#include "cec2022.hpp"


namespace ioh::problem::cec
{
    class cec2022_01 final : public CEC2022Problem<cec2022_01>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            std::vector<double> sr_x(x);
            double f =
                cec_basic_funcs_.zakharov_func(sr_x, Os_, Mr_, true, true, nx_);
            return f + 300.0;
        }

    public:
        cec2022_01(const int instance, const int n_variables) :
            CEC2022Problem(1, instance, n_variables, "cec2022_01")
        {
        }
    };

    class cec2022_02 final : public CEC2022Problem<cec2022_02>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            std::vector<double> sr_x(x);
            double f = cec_basic_funcs_.rosenbrock_func(sr_x, Os_, Mr_, true,
                                                        true, nx_);
            return f + 400.0;
        }

    public:
        cec2022_02(const int instance, const int n_variables) :
            CEC2022Problem(2, instance, n_variables, "cec2022_02")
        {
        }
    };

    class cec2022_03 final : public CEC2022Problem<cec2022_03>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            std::vector<double> sr_x(x);
            double f = cec_basic_funcs_.schaffer_F7_func(sr_x, Os_, Mr_, true,
                                                         true, nx_);
            return f + 600.0;
        }

    public:
        cec2022_03(const int instance, const int n_variables) :
            CEC2022Problem(3, instance, n_variables, "cec2022_03")
        {
        }
    };

    class cec2022_04 final : public CEC2022Problem<cec2022_04>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            std::vector<double> sr_x(x);
            double f = cec_basic_funcs_.step_rastrigin_func(sr_x, Os_, Mr_,
                                                            true, true, nx_);
            return f + 800.0;
        }

    public:
        cec2022_04(const int instance, const int n_variables) :
            CEC2022Problem(4, instance, n_variables, "cec2022_04")
        {
        }
    };

    class cec2022_05 final : public CEC2022Problem<cec2022_05>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            std::vector<double> sr_x(x);
            double f =
                cec_basic_funcs_.levy_func(sr_x, Os_, Mr_, true, true, nx_);
            return f + 900.0;
        }

    public:
        cec2022_05(const int instance, const int n_variables) :
            CEC2022Problem(5, instance, n_variables, "cec2022_05")
        {
        }
    };

    class cec2022_06 final : public CEC2022Problem<cec2022_06>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            std::vector<double> sr_x(x);
            double f = cec_hybrid_funcs_.cec2022_hf01(sr_x, Os_, Mr_, SS_, true,
                                                      true, nx_);
            return f + 1800.0;
        }

    public:
        cec2022_06(const int instance, const int n_variables) :
            CEC2022Problem(6, instance, n_variables, "cec2022_06")
        {
        }
    };

    class cec2022_07 final : public CEC2022Problem<cec2022_07>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            std::vector<double> sr_x(x);
            double f = cec_hybrid_funcs_.cec2022_hf02(sr_x, Os_, Mr_, SS_, true,
                                                      true, nx_);
            return f + 2000.0;
        }

    public:
        cec2022_07(const int instance, const int n_variables) :
            CEC2022Problem(7, instance, n_variables, "cec2022_07")
        {
        }
    };

    class cec2022_08 final : public CEC2022Problem<cec2022_08>
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            std::vector<double> sr_x(x);
            double f = cec_hybrid_funcs_.cec2022_hf03(sr_x, Os_, Mr_, SS_, true,
                                                      true, nx_);
            return f + 2200.0;
        }

    public:
        cec2022_08(const int instance, const int n_variables) :
            CEC2022Problem(8, instance, n_variables, "cec2022_08")
        {
        }
    };

} // namespace ioh::problem::cec
