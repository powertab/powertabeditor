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

#include "document.h"

/// Utility function to add a node with a CDATA child, i.e. <![CDATA[text]]>.
static void
addCDataNode(pugi::xml_node &node, const char *name, const std::string &value)
{
    node.append_child(name)
        .append_child(pugi::node_cdata)
        .set_value(value.c_str());
}

/// Utility function to add a node with a plain character data child.
static void
addValueNode(pugi::xml_node &node, const char *name, const std::string &value)
{
    node.append_child(name)
        .append_child(pugi::node_pcdata)
        .set_value(value.c_str());
}

namespace Gp7
{
static void
saveScoreInfo(pugi::xml_node &node, const Gp7::ScoreInfo &info)
{
    addCDataNode(node, "Title", info.myTitle);
    addCDataNode(node, "SubTitle", info.mySubtitle);
    addCDataNode(node, "Artist", info.myArtist);
    addCDataNode(node, "Album", info.myAlbum);
    addCDataNode(node, "Words", info.myWords);
    addCDataNode(node, "Music", info.myMusic);
    addCDataNode(node, "Copyright", info.myCopyright);
    addCDataNode(node, "Tabber", info.myTabber);
    addCDataNode(node, "Instructions", info.myInstructions);
    addCDataNode(node, "Notices", info.myNotices);
}

pugi::xml_document
to_xml(const Document &doc)
{
    pugi::xml_document root;

    auto gpif = root.append_child("GPIF");
    addValueNode(gpif, "GPVersion", "7.6.0");

    auto score = gpif.append_child("Score");
    saveScoreInfo(score, doc.myScoreInfo);

    return root;
}
} // namespace Gp7
