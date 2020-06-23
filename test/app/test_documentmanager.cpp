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
  
#include <catch2/catch.hpp>

#include <app/documentmanager.h>

TEST_CASE("App/DocumentManager")
{
    DocumentManager manager;

    REQUIRE(!manager.hasOpenDocuments());

    manager.addDocument();
    manager.addDocument();
    manager.addDocument();

    REQUIRE(manager.hasOpenDocuments());
    REQUIRE(manager.getCurrentDocumentIndex() == 2);

    manager.removeDocument(1);
    REQUIRE(manager.getCurrentDocumentIndex() == 1);

    manager.removeDocument(0);
    REQUIRE(manager.getCurrentDocumentIndex() == 0);
    manager.removeDocument(0);

    REQUIRE(!manager.hasOpenDocuments());
}

TEST_CASE("App/Document")
{
    Document document;

    REQUIRE(!document.hasFilename());
}

