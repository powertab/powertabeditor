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
  
#ifndef BEAMGROUP_H
#define BEAMGROUP_H

#include <vector>
#include <QFont>
#include <powertabdocument/notestem.h>

class QGraphicsItem;
class QPainterPath;
class StaffData;

class BeamGroup
{
public:
    BeamGroup(const StaffData& staffInfo, const std::vector<NoteStem>& noteStems);

    void drawStems(QGraphicsItem* parent) const;
    void drawExtraBeams(QPainterPath& beamPath) const;

    static const double FRACTIONAL_BEAM_WIDTH;

    void copyNoteStems(std::vector<NoteStem>& stems) const;

private:
    void adjustStemHeights();

    QGraphicsItem* createStaccato(const NoteStem& noteStem) const;
    QGraphicsItem* createFermata(const NoteStem& noteStem) const;
    QGraphicsItem* createAccent(const NoteStem& noteStem) const;
    QGraphicsItem* createNoteFlag(const NoteStem& noteStem) const;

    const StaffData& staffInfo;
    std::vector<NoteStem> noteStems;
    NoteStem::StemDirection stemDirection;
    QFont musicNotationFont;
};

#endif // BEAMGROUP_H
