#pragma once

#include <algorithm>
#include <string>
#include <cmath>
#include <map>
#include <utility>
#include <vector>

#include <cctype>
#include <locale>

#include "ioh/common/random.hpp"

namespace ioh
{
    namespace common
    {
        /**
         * \brief Checks a vector of doubles for nan values
         * \param x vector to be checked
         * \return true if x contains a nan value
         */
        inline bool has_nan(const std::vector<double> &x)
        {
            for (const auto &e : x)
                if (std::isnan(e))
                    return true;
            return false;
        }

        /**
         * \brief Checks a vector of doubles for nan values
         * \param x vector to be checked
         * \return true if x contains a nan value
         */
        inline bool all_finite(const std::vector<double> &x)
        {
            for (const auto &e : x)
                if (!std::isfinite(e))
                    return false;
            return true;
        }

        /**
         * \brief Checks a vector of doubles for inf values
         * \param x vector to be checked
         * \return true if x contains a nan value
         */
        inline bool has_inf(const std::vector<double> &x)
        {
            for (const auto &e : x)
                if (std::isinf(e))
                    return true;
            return false;
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
        template <typename K, typename V>
        std::vector<K> keys(const std::map<K, V> &m)
        {
            std::vector<K> keys;
            for (const auto &p : m)
                keys.push_back(p.first);
            return keys;
        }

        //! Retrieve the values from a map
        template <typename K, typename V>
        std::vector<V> values(const std::map<K, V> &m)
        {
            std::vector<V> values;
            for (const auto &p : m)
                values.push_back(p.second);
            return values;
        }

        //! Return a vector of key-value pairs for a given map
        template <typename K, typename V, typename P = std::pair<K, V>>
        std::vector<P> as_vector(const std::map<K, V> &m)
        {
            std::vector<P> values;
            for (const auto &[first, second] : m)
                values.emplace_back(first, second);
            return values;
        }

        //! Return a vector of key-value pairs for a given map, where V is a pointer
        template <typename K, typename V, typename P = std::pair<K, V>>
        std::vector<P> as_vector(const std::map<K, V *> &m)
        {
            std::vector<P> values;
            for (const auto &[first, second] : m)
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
        inline std::vector<int> range(const int start, const int stop, const int step = 1)
        {
            std::vector<int> v((stop - start) / step);
            std::generate(v.begin(), v.end(), [=, c = start - step]() mutable
            {
                c += step;
                return c;
            });
            return v;
        }

        /**
         * @brief Helper to print the contents of iterator to stdout
         * 
         * @tparam Iterator 
         * @param x the iterators to print, for example a std::vector
         * @param del the delimeter between each element
         */
        template <typename Iterator>
        void print(const Iterator x, const std::string &del = " ")
        {
            for (auto e : x)
            {
                std::cout << e << del;
            }
            std::cout << std::endl;
        }

        
        /**
         * \brief trim leading whitespace (in place)
         * \param s the string to trim
         */
        inline void ltrim(std::string &s)
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](const unsigned char ch) { return !std::isspace(ch); }));
        }

      
        /**
         * \brief trim from end (in place)
         * \param s the string to trim
         */
        inline void rtrim(std::string &s)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](const unsigned char ch) { return !std::isspace(ch); }).base(),
                    s.end());
        }

        /**
         * \brief trim from both ends (in pla0ce)
         * \param s the string to trim
         */
        inline void trim(std::string &s)
        {
            ltrim(s);
            rtrim(s);
        }


        //! Permutation struct
        struct Permutation
        {
            //! value
            double value;

            //! index
            int index;

            //! sort operator
            bool operator<(const Permutation &b) const { return value < b.value; }

            //! sort a set of random permutations
            static std::vector<Permutation> sorted(const int n, const int seed)
            {
                const auto random_numbers = random::bbob2009::uniform(n, seed);
                std::vector<Permutation> permutations(n);

                for (auto i = 0; i < n; ++i)
                    permutations[i] = {random_numbers.at(i), i};

                std::sort(permutations.begin(), permutations.end());
                return permutations;
            }
        };
    } // namespace common
} // namespace ioh
