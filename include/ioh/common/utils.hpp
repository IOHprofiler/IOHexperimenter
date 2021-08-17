#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <array>
#include <type_traits>
#include <limits>
#include <ostream>
#include <functional>
#include <numeric>
#include <cstdlib>
#include <utility>
#include <cstdio>
#include <optional>

#include "fmt/format.h"

#if defined(__GNUC__) || defined(__GNUG__)
#include <cxxabi.h>
#endif

#include "log.hpp"

namespace ioh
{
    namespace common
    {
        /**
         * \brief Enum containing minimization = 0 and maximization = 1 flags
         * 
         * @todo FIXME this should really be a class with two instances holding comparison operators, so as to avoid tests.
         */
        enum class OptimizationType
        {
            Minimization,
            Maximization
        };

        /**
         * \brief Compares two values v1 and v2, and returns true if v1 is better
         * \tparam T Type of v1 and v2
         * \param v1 The first value
         * \param v2 The second value
         * \param optimization_type Used to determine which value is better,
         * when optimization_type == \ref common::OptimizationType::Minimization lower elements are better,
         * otherwise higher elements are better.
         * \return true if v1 is better than v2
         */
        template <typename T>
        bool compare_objectives(const T v1, const T v2,
                                const OptimizationType optimization_type)
        {
            if (optimization_type == OptimizationType::Maximization)
                return v1 > v2;
            return v1 < v2;
        }

        /**
         * \brief Get the type name of a given template T
         * \tparam T a type 
         * \return the name of T
         */
        template <typename T>
        std::string type_name()
        {
            std::string name = typeid(T).name();
#if defined(__GNUC__) || defined(__GNUG__)
            int status;
            auto demangled_name = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
            if (status == 0) {
                name = demangled_name;
                std::free(demangled_name);
            }
#endif
            return name;
        }

        /**
         * \brief Get the type name of the problem
         * \tparam T the template of the Problem
         * \return the name of T
         */
        template <typename T>
        std::string class_name()
        {
            auto name = type_name<T>();
            name = name.substr(name.find_last_of(' ') + 1);
            name = name.substr(name.find_last_of("::") + 1);
            return name = name.substr(0, name.find_first_of(">"));
        }

        /**
         * \brief concatenates two vectors
         * \tparam T the type of the vectors
         * \param a the first vector
         * \param b the second vector
         * \return the resulting vector, containing the elements of both a and b
         */
        template <typename T>
        std::vector<T> concatenate(std::vector<T> a, const std::vector<T> &b)
        {
            a.insert(a.end(), b.begin(), b.end());
            return a;
        }

        //! Retrieve the keys from a map
        template<typename K, typename V>
        inline std::vector<K> keys(const std::map<K,V>& m){
            std::vector<K> keys;
            for (const auto &[first, _]: m)
                keys.push_back(first);
            return keys;
        }

        //! Retrieve the values from a map
        template<typename K, typename V>
        inline std::vector<V> values(const std::map<K,V>& m){
            std::vector<V> values;
            for (const auto &[_, second]: m)
                values.push_back(second);
            return values;
        }

        //! Return a vector of key-value pairs for a given map
        template<typename K, typename V, typename P = std::pair<K,V>>
        inline std::vector<P> as_vector(const std::map<K,V>& m){
            std::vector<P> values;
            for (const auto &[first, second]: m)
                values.emplace_back(first, second);
            return values;
        }

        //! Return a vector of key-value pairs for a given map, where V is a pointer
        template<typename K, typename V, typename P = std::pair<K,V>>
        inline std::vector<P> as_vector(const std::map<K,V*>& m){
            std::vector<P> values;
            for (const auto &[first, second]: m)
                values.emplace_back(first, *second);
            return values;
        }

        /**
         * \brief Returns a range of integers
         * \param start start of the range
         * \param stop end of the range
         * \param step stepsize of the range
         * \return a vector filled with numbers 
         */
        inline std::vector<int> range(const int start, const int stop, const int step = 1){
            std::vector<int> v((stop - start) / step);
            std::generate(v.begin(), v.end(), [=, c=start - step] () mutable { c += step; return c; });
            return v;
        }       

        /**
         * \brief A simple timer class, logging elapsed CPU time to stdout
         */
        class CpuTimer
        {
            /**
             * \brief A info message to be printed to stdout when the timer completes
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
             * \param info_msg The value for \ref info_msg_ 
             */
            explicit CpuTimer(std::string info_msg = "") :
                info_msg_(std::move(info_msg)), start_time_(Clock::now())
            {
            }

            /**
             * \brief Destructs a timer, prints time elapsed to stdout
             */
            ~CpuTimer()
            {
                IOH_DBG(progress,fmt::format(
                    "{}CPU Time: {:d} ms", info_msg_,
                    std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start_time_).count()));
            }
        };


        /**
         * @brief Interface which defines a repr method
         */
        struct HasRepr {
            //! Representation of the object
            virtual std::string repr() const = 0;

            //! Default string stream method
            friend std::ostream &operator<<(std::ostream &os, const HasRepr &obj) {
                return os << obj.repr();
            }
        };
    }
}

/**
 * @brief Formatter for classes with repr method
 */
template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<ioh::common::HasRepr, T>::value, char>> :
    fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const ioh::common::HasRepr &a, FormatContext &ctx)
    {
        return formatter<std::string>::format(a.repr(), ctx);
    }
};

