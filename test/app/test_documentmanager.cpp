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

#include <app/documentmanager.h>

TEST_CASE("App/DocumentManager", "")
{
    DocumentManager manager;

    REQUIRE(manager.getCurrentDocumentIndex() == -1);

    manager.createDocument();
    REQUIRE(manager.getCurrentDocumentIndex() == 0);

    // should not be able to set document index to an invalid number
    manager.setCurrentDocumentIndex(1);
    REQUIRE(manager.getCurrentDocumentIndex() == 0);

    // should be able to set document index to a valid number
    manager.createDocument();
    manager.createDocument();
    manager.setCurrentDocumentIndex(1);
    REQUIRE(manager.getCurrentDocumentIndex() == 1);
}
