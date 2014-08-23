/*
  * Copyright (C) 2014 Cameron White
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

#include <formats/gpx/gpximporter.h>
#include <QCoreApplication>
#include <score/score.h>

static void loadTest(GpxImporter &importer, const char *filename, Score &score)
{
    QString path = QCoreApplication::applicationDirPath();
    path += "/";
    path += filename;

    importer.load(path.toStdString(), score);
}

TEST_CASE("Formats/GpxImport/Text", "")
{
    Score score;
    GpxImporter importer;
    loadTest(importer, "data/text.gpx", score);

    const System &system = score.getSystems()[0];

    REQUIRE(system.getTextItems().size() == 1);
    REQUIRE(system.getTextItems()[0].getPosition() == 9);
    REQUIRE(system.getTextItems()[0].getContents() == "foo");
}