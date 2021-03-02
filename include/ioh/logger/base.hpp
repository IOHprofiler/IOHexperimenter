#pragma once
#include "observer.hpp"

namespace ioh {
    namespace problem
    {
        struct MetaData;
    }
    
    namespace logger {
        struct LogInfo
        {
            size_t evaluations;
            double y;
            double y_best;
            double transformed_y;
            double transformed_y_best;
            std::vector<double> x;
        };

        struct Base {
            virtual void track_problem(const problem::MetaData& problem) = 0;

            virtual void track_suite(const std::string& suite_name) = 0;

            virtual void log(const LogInfo& log_info) = 0;

            virtual void flush() = 0;

        };
    }
}
