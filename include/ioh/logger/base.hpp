#pragma once
#include "observer.hpp"


namespace ioh {
    namespace logger {
        template <typename ProblemType>
        class Base {
        public:

            using input_type = ProblemType;

            virtual void track_problem(const ProblemType& problem) = 0;

            virtual void track_suite(const suite::base<ProblemType>& suite) = 0;

            virtual void do_log(const std::vector<double>& log_info) = 0;
        };
    }
}
