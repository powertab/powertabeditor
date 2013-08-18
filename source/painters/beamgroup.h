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
  
#ifndef PAINTERS_BEAMGROUP_H
#define PAINTERS_BEAMGROUP_H

#include <painters/notestem.h>
#include <QFont>
#include <vector>

struct LayoutInfo;
class QFontMetricsF;
class QGraphicsItem;

class BeamGroup
{
public:
    BeamGroup(const LayoutInfo &layout, const std::vector<NoteStem> &stems);

    /// Draws the stems for each note in the group.
    void drawStems(QGraphicsItem *parent, const QFont &musicFont,
                   const QFontMetricsF &fm) const;

private:
    /// Compute a common direction for the stems.
    static NoteStem::StemType computeStemDirection(std::vector<NoteStem> &stems);

    static NoteStem findHighestStem(const std::vector<NoteStem> &stems);
    static NoteStem findLowestStem(const std::vector<NoteStem> &stems);

    /// Stretches the beams to a common high/low height, depending on stem
    /// direction.
    void adjustStemHeights();

    QGraphicsItem *createStaccato(const NoteStem& noteStem) const;
    QGraphicsItem *createFermata(const NoteStem& noteStem) const;
    QGraphicsItem *createAccent(const NoteStem& noteStem) const;

    QGraphicsItem *createNoteFlag(const NoteStem& stem,
                                  const QFont &musicFont,
                                  const QFontMetricsF &fm) const;

    std::vector<NoteStem> myNoteStems;
    NoteStem::StemType myStemDirection;
};

#endif
