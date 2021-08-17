#pragma once

#include <string>
#include <ostream>
#include <type_traits>
#include <fmt/format.h> 

namespace ioh
{
    namespace common
    {
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
