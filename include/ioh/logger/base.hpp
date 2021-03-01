#pragma once
#include "observer.hpp"


namespace ioh {
    namespace logger {
        template <typename ProblemType>
        class Base {
        public:
            using InputType = ProblemType;

            virtual void track_problem(const ProblemType& problem) = 0;

            virtual void track_suite(const std::string& suite_name) = 0;

            virtual void do_log(const std::vector<double>& log_info) = 0;
        };
    }
}
