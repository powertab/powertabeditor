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
#include <vector>

struct LayoutInfo;
class QFont;
class QFontMetricsF;
class QGraphicsItem;
class QPainterPath;

class BeamGroup
{
public:
    BeamGroup(NoteStem::StemType direction, const std::vector<size_t> &stems);

    /// Draws the stems for each note in the group.
    void drawStems(QGraphicsItem *parent, const std::vector<NoteStem> &stems,
                   const QFont &musicFont, const QFontMetricsF &fm,
                   const LayoutInfo &layout) const;

private:
    /// Draws the extra beams required for sixteenth notes, etc.
    void drawExtraBeams(QPainterPath &path,
                        std::vector<NoteStem>::const_iterator begin,
                        std::vector<NoteStem>::const_iterator end) const;

    /// Creates and positions a staccato symbol.
    static QGraphicsItem *createStaccato(const NoteStem& stem,
                                         const QFont &musicFont,
                                         const QFontMetricsF &fm);

    /// Creates and positions a fermata symbol.
    static QGraphicsItem *createFermata(const NoteStem& noteStem,
                                        const QFont &musicFont,
                                        const LayoutInfo &layout);

    /// Creates and positions an accent symbol.
    static QGraphicsItem *createAccent(const NoteStem& stem,
                                       const QFont &musicFont,
                                       const LayoutInfo &layout);

    static QGraphicsItem *createNoteFlag(const NoteStem& stem,
                                         const QFont &musicFont,
                                         const QFontMetricsF &fm);

    NoteStem::StemType myStemDirection;
    std::vector<size_t> myStems;
};

#endif
