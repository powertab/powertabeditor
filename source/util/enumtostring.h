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

#include "enumtostring_fwd.h"

#include <algorithm>
#include <cassert>
#include <optional>
#include <string>
#include <utility>

/// Macro for defining enum to string conversion (and vice versa). Hopefully
/// someday this will be less ugly to do in C++!
/// The implementation is inspired by NLOHMANN_JSON_SERIALIZE_ENUM
///
/// Usage: UTIL_DECLARE_ENUMTOSTRING(MyEnum, {{MyEnum::ValueA, "textA"}, ...})
#define UTIL_DEFINE_ENUMTOSTRING(EnumType, ...)                                \
    namespace Util                                                             \
    {                                                                          \
    template <>                                                                \
    std::string enumToString<EnumType>(EnumType value)                         \
    {                                                                          \
        static const std::pair<EnumType, std::string> map[] = __VA_ARGS__;     \
        auto it =                                                              \
            std::find_if(std::begin(map), std::end(map),                       \
                         [&](auto &entry) { return entry.first == value; });   \
        assert(it != std::end(map));                                           \
        return it->second;                                                     \
    }                                                                          \
    template <>                                                                \
    std::optional<EnumType> toEnum<EnumType>(const std::string &str)           \
    {                                                                          \
        static const std::pair<EnumType, std::string> map[] = __VA_ARGS__;     \
        auto it =                                                              \
            std::find_if(std::begin(map), std::end(map),                       \
                         [&](auto &entry) { return entry.second == str; });    \
        if (it == std::end(map))                                               \
            return std::nullopt;                                               \
                                                                               \
        return it->first;                                                      \
    }                                                                          \
    }

#endif
