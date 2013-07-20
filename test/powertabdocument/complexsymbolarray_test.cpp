/*
  * Copyright (C) 2011 Cameron White
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

#include <catch.hpp>

#include <powertabdocument/complexsymbolarray.h>
#include <powertabdocument/position.h>

struct ComplexSymbolFixture
{
    ComplexSymbolFixture()
    {
        array[0] = MAKELONG(MAKEWORD(0, 0),
                            MAKEWORD(0, Position::volumeSwell));
        array[1] = ComplexSymbols::notUsed;
        array[2] = MAKELONG(MAKEWORD(0, 0),
                            MAKEWORD(0, Position::tremoloBar));
    }

    boost::array<uint32_t, 3> array;
};

TEST_CASE_METHOD(ComplexSymbolFixture, "PowerTabDocument/ComplexSymbols/FindComplexSymbol", "")
{
    REQUIRE(ComplexSymbols::findComplexSymbol(array, Position::multibarRest) == -1u);

    REQUIRE(ComplexSymbols::findComplexSymbol(array, Position::volumeSwell) == 0u);

    REQUIRE(ComplexSymbols::findComplexSymbol(array, Position::tremoloBar) == 2u);
}

TEST_CASE_METHOD(ComplexSymbolFixture, "PowerTabDocument/ComplexSymbols/GetComplexSymbolCount", "")
{
    REQUIRE(ComplexSymbols::getComplexSymbolCount(array) == 2u);

    boost::array<uint32_t, 3> emptyArray;
    std::fill(emptyArray.begin(), emptyArray.end(), 0);
    REQUIRE(ComplexSymbols::getComplexSymbolCount(emptyArray) == 0u);
}

TEST_CASE_METHOD(ComplexSymbolFixture, "PowerTabDocument/ComplexSymbols/ClearComplexSymbols", "")
{
    REQUIRE(ComplexSymbols::getComplexSymbolCount(array) == 2u);

    ComplexSymbols::clearComplexSymbols(array);
    REQUIRE(ComplexSymbols::getComplexSymbolCount(array) == 0u);
}

TEST_CASE_METHOD(ComplexSymbolFixture, "PowerTabDocument/ComplexSymbols/RemoveComplexSymbol", "")
{
    ComplexSymbols::removeComplexSymbol(array, Position::volumeSwell);
    REQUIRE(ComplexSymbols::findComplexSymbol(array, Position::volumeSwell) == -1u);
    REQUIRE(ComplexSymbols::getComplexSymbolCount(array) == 1u);
}

TEST_CASE("PowerTabDocument/ComplexSymbols/AddComplexSymbol", "")
{
    boost::array<uint32_t, 3> array;
    ComplexSymbols::clearComplexSymbols(array);

    // add a volume swell
    ComplexSymbols::addComplexSymbol(array, MAKELONG(MAKEWORD(0, 0), MAKEWORD(0, Position::volumeSwell)));
    REQUIRE(ComplexSymbols::getComplexSymbolCount(array) == 1u);
    REQUIRE(ComplexSymbols::findComplexSymbol(array, Position::volumeSwell) == 0u);
}
