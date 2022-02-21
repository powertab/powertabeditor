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

#ifndef UTIL_ENUMTOSTRING_FWD_H
#define UTIL_ENUMTOSTRING_FWD_H

#include <optional>
#include <string>

namespace Util
{
// Declaration for the template specializations added via the macro.
template <typename Enum>
std::optional<Enum> toEnum(const std::string &str);

template <typename Enum>
std::string enumToString(Enum value);                              \
}

/// Forward declaration for the conversion functions.
#define UTIL_DECLARE_ENUMTOSTRING(EnumType)                                    \
    namespace Util                                                             \
    {                                                                          \
        template <>                                                            \
        std::string enumToString<EnumType>(EnumType value);                    \
                                                                               \
        template <>                                                            \
        std::optional<EnumType> toEnum<EnumType>(const std::string &str);      \
    }

#endif
