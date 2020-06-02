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

#include "parser.h"

static Gp7::ScoreInfo parseScoreInfo(const pugi::xml_node &node)
{
    Gp7::ScoreInfo info;
    info.myTitle = node.child_value("Title");
    info.mySubtitle = node.child_value("SubTitle");
    info.myArtist = node.child_value("Artist");
    info.myAlbum = node.child_value("Album");
    // Skipping the 'WordsAndMusic' node, which seems to be unused.
    info.myWords = node.child_value("Words");
    info.myMusic = node.child_value("Music");
    info.myCopyright = node.child_value("Copyright");
    info.myTabber = node.child_value("Tabber");
    info.myInstructions = node.child_value("Instructions");
    info.myNotices = node.child_value("Notices");

    // Skipping FirstPageHeader, FirstPageFooter, PageHeader, PageFooter
    // These seem to be specific to Guitar Pro's text formatting.

    // Skipping ScoreSystemsDefaultLayout, ScoreSystemsLayout, ScoreZoomPolicy,
    // ScoreZoom, MultiVoice.
    // TODO - will any of these help to preserve the original file's layout?

    return info;
}

Gp7::Document Gp7::parse(const pugi::xml_document &root)
{
    Gp7::Document doc;

    // Note: the following children are skipped:
    // - GPVersion
    // - GPRevision
    // - Encoding
    // These probably aren't needed unless the Guitar Pro 6 parser is merged
    // with this one.

    const pugi::xml_node gpif = root.child("GPIF");
    doc.myScoreInfo = parseScoreInfo(gpif.child("Score"));

    return doc;
}
