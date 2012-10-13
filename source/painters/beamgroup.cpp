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
#include <app/common.h>

#include "staffdata.h"
#include "stdnotationpainter.h"
#include "musicfont.h"
#include <powertabdocument/position.h>

#include <QGraphicsPathItem>
#include <QPen>

#include <cmath> // for log() function

const double BeamGroup::FRACTIONAL_BEAM_WIDTH = 5;

namespace {
    const QFont musicNotationFont = MusicFont().getFont();
}

BeamGroup::BeamGroup(const StaffData& staffInfo, const std::vector<NoteStem>& noteStems) :
    staffInfo(staffInfo),
    noteStems(noteStems)
{
    // adjust top/bottom of stems to use absolute coordianates
    for (size_t i = 0; i < this->noteStems.size(); i++)
    {
        this->noteStems[i].stemTop += staffInfo.getTopStdNotationLine();
        this->noteStems[i].stemBottom += staffInfo.getTopStdNotationLine();
    }

    setStemDirections();
    adjustStemHeights();
}


/// Sets the stem directions for all stems in the beam group
/// This depends on how many stems currently point upwards/downwards
void BeamGroup::setStemDirections()
{
    stemDirection = NoteStem::findDirectionForGroup(noteStems);

    // Assign the new stem direction to each stem
    for (size_t i = 0; i < noteStems.size(); i++)
    {
        noteStems[i].stemDirection = stemDirection;
    }
}

/// Stretches the beams to a common high/low height, depending on stem direction
void BeamGroup::adjustStemHeights()
{
    if (stemDirection == NoteStem::StemUp)
    {
        NoteStem highestStem = *std::min_element(noteStems.begin(), noteStems.end(),
                                                 &compareStemTopPositions);

        for (std::vector<NoteStem>::iterator stem = noteStems.begin(); stem != noteStems.end(); ++stem)
        {
            stem->xPosition += stem->noteHeadRightEdge - 1;
            stem->stemTop = highestStem.stemTop - highestStem.stemSize();
        }
    }
    else // stem down
    {
        NoteStem lowestStem = *std::max_element(noteStems.begin(), noteStems.end(),
                                                &compareStemBottomPositions);

        for (std::vector<NoteStem>::iterator stem = noteStems.begin(); stem != noteStems.end(); ++stem)
        {
            stem->xPosition += stem->noteHeadRightEdge - stem->noteHeadWidth;
            stem->stemBottom = lowestStem.stemBottom + lowestStem.stemSize();
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
        stemPath.moveTo(stem->xPosition, stem->stemTop);
        stemPath.lineTo(stem->xPosition, stem->stemBottom);

        // draw any symbols that use information about the stem, like staccato, fermata, etc
        if (stem->position->IsStaccato())
            symbols << createStaccato(*stem);

        if (stem->position->HasFermata())
            symbols << createFermata(*stem);

        if (stem->position->HasSforzando() || stem->position->HasMarcato())
            symbols << createAccent(*stem);

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

        beamPath.moveTo(noteStems.front().xPosition + 0.5, connectorHeight);
        beamPath.lineTo(noteStems.back().xPosition, connectorHeight);
    }

    drawExtraBeams(beamPath);

    QGraphicsPathItem* beams = new QGraphicsPathItem(beamPath);
    beams->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
    beams->setParentItem(parent);

     // draw a note flag for single notes (eighth notes or less) or grace notes
    if (noteStems.size() == 1 && noteStems.front().canDrawFlag())
    {
        QGraphicsItem* flag = createNoteFlag(noteStems.front());
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
        const int extraBeams = Common::log2(stem->position->GetDurationType()) - 3;

        const bool hasFullBeaming = (stem->position->GetPreviousBeamDurationType() ==
                                     stem->position->GetDurationType());

        const bool hasFractionalLeft = stem->position->HasFractionalLeftBeam();
        const bool hasFractionalRight = stem->position->HasFractionalRightBeam();

        if (hasFullBeaming || hasFractionalLeft || hasFractionalRight)
        {
            for (int i = 1; i <= extraBeams; i++)
            {
                double y = stem->stemEdge();
                y += i * 3 * ((stemDirection == NoteStem::StemUp) ? 1 : -1);

                double xStart = 0, xEnd = 0;

                if (hasFullBeaming)
                {
                    xStart = (stem - 1)->xPosition + 0.5;
                    xEnd = stem->xPosition - 1;
                }
                else if (hasFractionalLeft)
                {
                    xStart = stem->xPosition + 0.5;
                    xEnd = xStart + FRACTIONAL_BEAM_WIDTH;
                }
                else if (hasFractionalRight)
                {
                    xEnd = stem->xPosition - 1;
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

/// Creates and positions a staccato symbol
QGraphicsItem* BeamGroup::createStaccato(const NoteStem& noteStem) const
{
    // draw the dot near either the top or bottom note of the position, depending on stem direction
    const double yPos = (noteStem.stemDirection == NoteStem::StemUp) ? noteStem.stemBottom - 25 :
                                                                       noteStem.stemTop - 43;

    const double xPos = (noteStem.stemDirection == NoteStem::StemUp) ? noteStem.xPosition - 8 :
                                                                       noteStem.xPosition - 2;

    QGraphicsTextItem* dot = new QGraphicsTextItem(QChar(MusicFont::Dot));
    dot->setFont(musicNotationFont);
    dot->setPos(xPos, yPos);
    return dot;
}

/// Creates and positions a fermata symbol
QGraphicsItem* BeamGroup::createFermata(const NoteStem& noteStem) const
{
    int y = 0;

    // position the fermata directly above/below the staff if possible, unless the note stem extends
    // beyond the std. notation staff.
    // After positioning, offset the height due to the way that QGraphicsTextItem positions text
    if (noteStem.stemDirection == NoteStem::StemUp)
    {
        y = std::min<double>(noteStem.stemTop, staffInfo.getTopStdNotationLine());
        y -= 33;
    }
    else
    {
        y = std::max<double>(noteStem.stemBottom, staffInfo.getBottomStdNotationLine());
        y -= 25;
    }

    const QChar symbol = (noteStem.stemDirection == NoteStem::StemUp) ? MusicFont::FermataUp : MusicFont::FermataDown;
    QGraphicsSimpleTextItem* fermata = new QGraphicsSimpleTextItem(symbol);
    fermata->setFont(musicNotationFont);
    fermata->setPos(noteStem.xPosition, y);

    return fermata;
}

/// Creates and positions an accent symbol
QGraphicsItem* BeamGroup::createAccent(const NoteStem& noteStem) const
{
    double y = 0;

    // position the accent directly above/below the staff if possible, unless the note stem extends
    // beyond the std. notation staff.
    // - it should be positioned opposite to the fermata symbols
    // After positioning, offset the height due to the way that QGraphicsTextItem positions text
    if (noteStem.stemDirection == NoteStem::StemDown)
    {
        y = std::min<double>(noteStem.stemTop, staffInfo.getTopStdNotationLine());
        y -= 38;
    }
    else
    {
        y = std::max<double>(noteStem.stemBottom, staffInfo.getBottomStdNotationLine());
        y -= 20;
    }

    QChar symbol;

    if (noteStem.position->HasMarcato())
    {
        symbol = MusicFont::getSymbol(MusicFont::Marcato);
    }
    else if (noteStem.position->HasSforzando())
    {
        symbol = MusicFont::getSymbol(MusicFont::Sforzando);
        y += 3;
    }

    if (noteStem.position->IsStaccato())
    {
        y += (noteStem.stemDirection == NoteStem::StemUp) ? 7 : -7;
    }

    QGraphicsSimpleTextItem* accent = new QGraphicsSimpleTextItem(symbol);
    accent->setFont(musicNotationFont);
    accent->setPos(noteStem.xPosition, y);

    return accent;
}

QGraphicsItem* BeamGroup::createNoteFlag(const NoteStem& noteStem) const
{
    Q_ASSERT(noteStem.canDrawFlag());

    // choose the flag symbol, depending on duration and stem direction
    QChar symbol = 0;
    if (noteStem.stemDirection == NoteStem::StemUp)
    {
        switch(noteStem.position->GetDurationType())
        {
        case 8:
            symbol = MusicFont::FlagUp1;
            break;
        case 16:
            symbol = MusicFont::FlagUp2;
            break;
        case 32:
            symbol = MusicFont::FlagUp3;
            break;
        default: // 64
            symbol = MusicFont::FlagUp4;
            break;
        }

        if (noteStem.position->IsAcciaccatura())
        {
            symbol = MusicFont::FlagUp1;
        }
    }
    else
    {
        switch(noteStem.position->GetDurationType())
        {
        case 8:
            symbol = MusicFont::FlagDown1;
            break;
        case 16:
            symbol = MusicFont::FlagDown2;
            break;
        case 32:
            symbol = MusicFont::FlagDown3;
            break;
        default: // 64
            symbol = MusicFont::FlagDown4;
            break;
        }

        if (noteStem.position->IsAcciaccatura())
        {
            symbol = MusicFont::FlagDown1;
        }
    }

    // draw the symbol
    const double y = noteStem.stemEdge() - 35; // adjust for spacing caused by the music symbol font
    QGraphicsTextItem* flag = new QGraphicsTextItem(symbol);
    flag->setFont(musicNotationFont);
    flag->setPos(noteStem.xPosition - 3, y);

    return flag;
}
