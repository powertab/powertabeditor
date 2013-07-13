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

#include "powertaboldimporter.h"

#include <boost/assign/list_of.hpp>
#include "powertabdocument/powertabdocument.h"

PowerTabOldImporter::PowerTabOldImporter()
    : FileFormatImporter(FileFormat("Power Tab Document",
                                    boost::assign::list_of("ptb")))
{
}

void PowerTabOldImporter::load(const std::string &filename, Score &score)
{
    PowerTabDocument::Document document;
    document.Load(filename);

    (void)score;
}
