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

#include <doctest/doctest.h>

#include <util/enumtostring.h>

enum class MyEnum
{
    ValueA,
    ValueB,
    ValueC,
};

UTIL_DECLARE_ENUMTOSTRING(MyEnum)

UTIL_DEFINE_ENUMTOSTRING(MyEnum, { { MyEnum::ValueA, "textA" },
                                   { MyEnum::ValueB, "textB" },
                                   { MyEnum::ValueC, "textC" } })

TEST_CASE("Util/EnumToString/Basic")
{
    REQUIRE(Util::enumToString(MyEnum::ValueA) == "textA");
    REQUIRE(Util::enumToString(MyEnum::ValueC) == "textC");

    REQUIRE(Util::toEnum<MyEnum>("textA") == MyEnum::ValueA);
    REQUIRE(Util::toEnum<MyEnum>("textB") == MyEnum::ValueB);
    REQUIRE(!Util::toEnum<MyEnum>("invalid").has_value());
}
