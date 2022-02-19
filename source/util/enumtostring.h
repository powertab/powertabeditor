/*
  * Copyright (C) 2022 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef UTIL_ENUMTOSTRING_H
#define UTIL_ENUMTOSTRING_H

#include <exception>
#include <string>
#include <utility>

namespace Util
{
// Declaration for template specializations added via the macro.
template <typename Enum>
Enum toEnum(const std::string &str);
}

/// Forward declaration for the conversion functions.
#define UTIL_DECLARE_ENUMTOSTRING(EnumType)                                    \
    namespace Util                                                             \
    {                                                                          \
        std::string toString(EnumType value) noexcept;                         \
        template <>                                                            \
        EnumType toEnum<EnumType>(const std::string &str);                     \
    }

/// Macro for defining enum to string conversion (and vice versa). Hopefully
/// someday this will be less ugly to do in C++!
/// toEnum() will throw an exception if the string is invalid.
/// The implementation is inspired by NLOHMANN_JSON_SERIALIZE_ENUM
///
/// Usage: UTIL_DECLARE_ENUMTOSTRING(MyEnum, {{MyEnum::ValueA, "textA"}, ...})
#define UTIL_DEFINE_ENUMTOSTRING(EnumType, ...)                                \
    namespace Util                                                             \
    {                                                                          \
        static const std::pair<EnumType, std::string> theMap##__LINE__[] =     \
            __VA_ARGS__;                                                       \
        std::string toString(EnumType value) noexcept                          \
        {                                                                      \
            auto it = std::find_if(                                            \
                std::begin(theMap##__LINE__), std::end(theMap##__LINE__),      \
                [&](auto &entry) { return entry.first == value; });            \
            assert(it != std::end(theMap##__LINE__));                          \
            return it->second;                                                 \
        }                                                                      \
        template <>                                                            \
        EnumType toEnum<EnumType>(const std::string &str)                      \
        {                                                                      \
            auto it = std::find_if(                                            \
                std::begin(theMap##__LINE__), std::end(theMap##__LINE__),      \
                [&](auto &entry) { return entry.second == str; });             \
            if (it == std::end(theMap##__LINE__))                              \
                throw std::invalid_argument(str);                              \
                                                                               \
            return it->first;                                                  \
        }                                                                      \
    }

#endif
