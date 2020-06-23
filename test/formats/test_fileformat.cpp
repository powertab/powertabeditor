/*
  * Copyright (C) 2013 Cameron White
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

#include <formats/fileformat.h>

TEST_CASE("Formats/FileFormat/FileFilterSingle")
{
    FileFormat format("Test Format", std::vector<std::string>(1, "ptb"));

    CHECK(format.fileFilter() == "Test Format (*.ptb)");
}

TEST_CASE("Formats/FileFormat/FileFilterMulti")
{
    FileFormat format("Test Format", { "gp3", "gp4", "gp5" });

    CHECK(format.fileFilter() == "Test Format (*.gp3 *.gp4 *.gp5)");
}
