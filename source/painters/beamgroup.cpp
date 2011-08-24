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
  
#include "beamgroup.h"

#include <algorithm>

#include "staffdata.h"
#include "stdnotationpainter.h"
#include <powertabdocument/position.h>

#include <QGraphicsPathItem>
#include <QPen>

#include <cmath> // for log() function

const double BeamGroup::FRACTIONAL_BEAM_WIDTH = 5;

BeamGroup::BeamGroup(const StaffData& staffInfo, const std::vector<NoteStem>& noteStems) :
    staffInfo(staffInfo),
    noteStems(noteStems)
{
    setStemDirections();
    adjustStemHeights();
}


/// Sets the stem directions for all stems in the beam group
/// This depends on how many stems currently point upwards/downwards
void BeamGroup::setStemDirections()
{
    stemDirection = findDirectionForGroup(noteStems);

    // Assign the new stem direction to each stem
    std::for_each(noteStems.begin(), noteStems.end(),
                  std::bind2nd(std::mem_fun_ref(&NoteStem::setDirection), stemDirection));
}

/// Stretches the beams to a common high/low height, depending on stem direction
void BeamGroup::adjustStemHeights()
{
    if (stemDirection == NoteStem::StemUp)
    {
        NoteStem highestStem = *std::min_element(noteStems.begin(), noteStems.end(),
                                                 CompareStemPositions(&NoteStem::top));

        for (std::vector<NoteStem>::iterator stem = noteStems.begin(); stem != noteStems.end(); ++stem)
        {
            stem->setX(stem->x() + staffInfo.getNoteHeadRightEdge() - 1);
            stem->setTop(highestStem.top() - highestStem.stemSize());
        }
    }
    else // stem down
    {
        NoteStem lowestStem = *std::max_element(noteStems.begin(), noteStems.end(),
                                                CompareStemPositions(&NoteStem::bottom));

        for (std::vector<NoteStem>::iterator stem = noteStems.begin(); stem != noteStems.end(); ++stem)
        {
            stem->setX(stem->x() + staffInfo.getNoteHeadRightEdge() - StdNotationPainter::getNoteHeadWidth());

            if (stem->position()->GetDurationType() == 2) // visual adjustment for half notes
            {
                stem->setX(stem->x() - 2);
            }

            stem->setBottom(lowestStem.bottom() + lowestStem.stemSize());
        }
    }
}

/// Draws the stems for each note in the group
void BeamGroup::drawStems(QGraphicsItem* parent) const
{
    QList<QGraphicsItem*> symbols;

    QPainterPath stemPath;

    // Draw each stem
    for (std::vector<NoteStem>::const_iterator stem = noteStems.begin(); stem != noteStems.end(); ++stem)
    {
        stemPath.moveTo(stem->x(), stem->top());
        stemPath.lineTo(stem->x(), stem->bottom());

        /*QGraphicsLineItem* line = new QGraphicsLineItem;
        line->setLine(stem->x(), stem->top(), stem->x(), stem->bottom());
        line->setParentItem(parent);*/

        // draw any symbols that use information about the stem, like staccato, fermata, etc
        if (stem->position()->IsStaccato())
            symbols << stem->createStaccato();

        if (stem->position()->HasFermata())
            symbols << stem->createFermata();

        if (stem->position()->HasSforzando() || stem->position()->HasMarcato())
            symbols << stem->createAccent();

        foreach (QGraphicsItem* symbol, symbols)
        {
            symbol->setParentItem(parent);
        }
        symbols.clear();
    }

    QGraphicsPathItem* stems = new QGraphicsPathItem(stemPath);
    stems->setParentItem(parent);

    QPainterPath beamPath;

    // draw connecting line
    if (noteStems.size() > 1)
    {
        const double connectorHeight = noteStems.front().stemEdge();

        beamPath.moveTo(noteStems.front().x() + 1, connectorHeight);
        beamPath.lineTo(noteStems.back().x() - 1, connectorHeight);
    }

    drawExtraBeams(beamPath);

    QGraphicsPathItem* beams = new QGraphicsPathItem(beamPath);
    beams->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
    beams->setParentItem(parent);

     // draw a note flag for single notes (eighth notes or less) or grace notes
    if (noteStems.size() == 1 && noteStems.front().canDrawFlag())
    {
        QGraphicsItem* flag = noteStems.front().createNoteFlag();
        flag->setParentItem(parent);
    }
}

/// Draws the extra beams required for sixteenth notes, etc
void BeamGroup::drawExtraBeams(QPainterPath& beamPath) const
{
    for (std::vector<NoteStem>::const_iterator stem = noteStems.begin();
         stem != noteStems.end(); ++stem)
    {
        // 16th note gets 1 extra beam, 32nd gets two, etc
        // Calculate log_2 of the note duration, and subtract three (so log_2(16) - 3 = 1)
        const int extraBeams = log2(stem->position()->GetDurationType()) - 3;

        const bool hasFullBeaming = (stem->position()->GetPreviousBeamDurationType() ==
                                     stem->position()->GetDurationType());

        const bool hasFractionalLeft = stem->position()->HasFractionalLeftBeam();
        const bool hasFractionalRight = stem->position()->HasFractionalRightBeam();

        if (hasFullBeaming || hasFractionalLeft || hasFractionalRight)
        {
            for (int i = 1; i <= extraBeams; i++)
            {
                double y = stem->stemEdge();
                y += i * 3 * ((stemDirection == NoteStem::StemUp) ? 1 : -1);

                double xStart = 0, xEnd = 0;

                if (hasFullBeaming)
                {
                    xStart = (stem - 1)->x() + 1;
                    xEnd = stem->x() - 1;
                }
                else if (hasFractionalLeft)
                {
                    xStart = stem->x() + 1;
                    xEnd = xStart + FRACTIONAL_BEAM_WIDTH;
                }
                else if (hasFractionalRight)
                {
                    xEnd = stem->x() - 1;
                    xStart = xEnd - FRACTIONAL_BEAM_WIDTH;
                }

                beamPath.moveTo(xStart, y);
                beamPath.lineTo(xEnd, y);
            }
        }
    }

}

/// Copies all of the note stems in the group
void BeamGroup::copyNoteSteams(std::vector<NoteStem>& stems) const
{
    stems = this->noteStems;
}
