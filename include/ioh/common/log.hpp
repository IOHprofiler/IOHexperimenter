#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace ioh::common::log {
    /**
     * \brief Enum containing log levels; info = 0, warning = 1, error = 2
     */
    enum class Level {
        info,
        warning,
        error
    };

    /**
     * \brief Setting for the log level
     */
    inline Level log_level = Level::info;

    /**
     * \brief Function for logging errors, causes a system exit
     * \param error_msg The error message
     */
    inline void error(const std::string &error_msg) {
        if (log_level <= Level::error) {
            std::cerr << "IOH_ERROR_INFO : " << error_msg << std::endl;
            std::flush(std::cerr);
            std::flush(std::cout);
            exit(1);
        }
    }

    /**
     * \brief Function for logging warnings
     * \param warning_msg The error message
     */
    inline void warning(const std::string &warning_msg) {
        if (log_level <= Level::warning)
            std::cout << "IOH_WARNING_INFO : " << warning_msg << std::endl;
    }

    /**
     * \brief Function for logging info messages
     * \param log_msg The info message
     */
    inline void info(const std::string &log_msg) {
        if (log_level <= Level::info)
            std::cout << "IOH_LOG_INFO : " << log_msg << std::endl;
    }

    /**
     * \brief Function for logging info messages to a stream
     * \param log_msg The info message
     * \param log_stream The stream to log the messages to
     */
    inline void info(const std::string &log_msg, std::ofstream &log_stream) {
        if (log_level >= Level::info)
            log_stream << "IOH_LOG_INFO : " << log_msg << std::endl;
    }
}
