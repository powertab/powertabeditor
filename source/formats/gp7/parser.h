/*
 * Copyright (C) 2020 Cameron White
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

#ifndef FORMATS_GP7_PARSER_H
#define FORMATS_GP7_PARSER_H

#include <pugixml.hpp>
#include <string>

namespace Gp7
{
/// Contains metadata about the score.
struct ScoreInfo
{
    std::string myTitle;
    std::string mySubtitle;
    std::string myArtist;
    std::string myAlbum;
    std::string myWords;
    std::string myMusic;
    std::string myCopyright;
    std::string myTabber;
    std::string myInstructions;
    std::string myNotices;
};

/// Container for a Guitar Pro 7 document.
struct Document
{
    ScoreInfo myScoreInfo;
};

/// Parses the score.gpif XML file.
Document parse(const pugi::xml_document &root);

} // namespace Gp7

#endif
