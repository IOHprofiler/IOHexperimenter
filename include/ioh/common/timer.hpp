#pragma once

#include <chrono>
#include <string>
#include <fmt/format.h>

#include "log.hpp"

namespace ioh
{
    namespace common
    {

        /**
         * \brief A simple timer class, logging elapsed CPU time to stdout
         */
        class CpuTimer
        {
            /**
             * \brief A info message to be logged to stdout when the timer completes
             */
            std::string info_msg_;
            /**
             * \brief The start time of the timer
             */
            using Clock = std::chrono::high_resolution_clock;

            Clock::time_point start_time_;

        public:
            /**
             * \brief Constructs a timer, sets start time
             * \param info_msg The value for info_msg_
             */
            explicit CpuTimer(std::string info_msg = "") : info_msg_(std::move(info_msg)), start_time_(Clock::now()) {}

            /**
             * \brief Destructs a timer, prints time elapsed to stdout
             */
            ~CpuTimer()
            {
                IOH_DBG(progress,
                        fmt::format(
                            "{}CPU Time: {:d} ms", info_msg_,
                            std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start_time_).count())
                    );
            }
        };

    } // namespace common
} // namespace ioh