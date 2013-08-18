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

#include <boost/foreach.hpp>
#include <painters/layoutinfo.h>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QPen>

static bool compareStemTopPositions(const NoteStem &stem1,
                                    const NoteStem &stem2)
{
    return stem1.getTop() < stem2.getTop();
}

static bool compareStemBottomPositions(const NoteStem &stem1,
                                       const NoteStem &stem2)
{
    return stem1.getBottom() < stem2.getBottom();
}

/// Functor to compare a NoteStem's stem direction for equality.
struct CompareStemType
{
    CompareStemType(NoteStem::StemType type)
        : myStemType(type)
    {
    }

    bool operator()(const NoteStem& stem) const
    {
        return stem.getStemType() == myStemType;
    }

private:
    NoteStem::StemType myStemType;
};

#if 0
const double BeamGroup::FRACTIONAL_BEAM_WIDTH = 5;
#endif
BeamGroup::BeamGroup(const LayoutInfo &layout,
                     const std::vector<NoteStem>& stems)
    : myNoteStems(stems)
{
    assert(!myNoteStems.empty());

    // Adjust top/bottom of stems to use absolute coordianates.
    BOOST_FOREACH(NoteStem &stem, myNoteStems)
    {
        stem.setTop(stem.getTop() + layout.getTopStdNotationLine());
        stem.setBottom(stem.getBottom() + layout.getTopStdNotationLine());
    }

    myStemDirection = computeStemDirection(myNoteStems);
    adjustStemHeights();
}

void BeamGroup::adjustStemHeights()
{
    if (myStemDirection == NoteStem::StemUp)
    {
        NoteStem highestStem = findHighestStem(myNoteStems);

        BOOST_FOREACH(NoteStem &stem, myNoteStems)
        {
            stem.setX(stem.getNoteHeadRightEdge() - 1);
            stem.setTop(highestStem.getTop() - highestStem.getStemHeight());
        }
    }
    else // Stem down.
    {
        NoteStem lowestStem = findLowestStem(myNoteStems);

        BOOST_FOREACH(NoteStem &stem, myNoteStems)
        {
            stem.setBottom(lowestStem.getBottom() + lowestStem.getStemHeight());
        }
    }
}

void BeamGroup::drawStems(QGraphicsItem *parent) const
{
    QList<QGraphicsItem *> symbols;
    QPainterPath stemPath;

    // Draw each stem.
    BOOST_FOREACH(const NoteStem &stem, myNoteStems)
    {
        stemPath.moveTo(stem.getX(), stem.getTop());
        stemPath.lineTo(stem.getX(), stem.getBottom());

        // TODO
#if 0
        // Draw any symbols that use information about the stem, like staccato,
        // fermata, etc.
        if (stem.isStaccato())
            symbols << createStaccato(stem);

        if (stem.hasFermata())
            symbols << createFermata(stem);

        if (stem.hasSforzando() || stem.hasMarcato())
            symbols << createAccent(stem);
#endif

        BOOST_FOREACH(QGraphicsItem *symbol, symbols)
            symbol->setParentItem(parent);

        symbols.clear();
    }

    QGraphicsPathItem *stems = new QGraphicsPathItem(stemPath);
    stems->setParentItem(parent);

    QPainterPath beamPath;

    // Draw connecting line.
    if (myNoteStems.size() > 1)
    {
        const double connectorHeight = myNoteStems.front().getStemEdge();

        beamPath.moveTo(myNoteStems.front().getX() + 0.5, connectorHeight);
        beamPath.lineTo(myNoteStems.back().getX(), connectorHeight);
    }

    // TODO
#if 0
    drawExtraBeams(beamPath);
#endif

    QGraphicsPathItem *beams = new QGraphicsPathItem(beamPath);
    beams->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
    beams->setParentItem(parent);

    // TODO
#if 0
    // Draw a note flag for single notes (eighth notes or less) or grace notes.
    if (myNoteStems.size() == 1 && myNoteStems.front().canDrawFlag())
    {
        QGraphicsItem* flag = createNoteFlag(myNoteStems.front());
        flag->setParentItem(parent);
    }
#endif
}

NoteStem::StemType BeamGroup::computeStemDirection(std::vector<NoteStem> &stems)
{
    // Find how many stem directions of each type we have.
    const size_t stemsUp = std::count_if(stems.begin(), stems.end(),
                                         CompareStemType(NoteStem::StemUp));

    const size_t stemsDown = std::count_if(stems.begin(), stems.end(),
                                           CompareStemType(NoteStem::StemDown));

    NoteStem::StemType stemType = (stemsDown >= stemsUp) ? NoteStem::StemDown :
                                                           NoteStem::StemUp;

    // Assign the new stem direction to each stem.
    BOOST_FOREACH(NoteStem &stem, stems)
    {
        stem.setStemType(stemType);
    }

    return stemType;
}

NoteStem BeamGroup::findHighestStem(const std::vector<NoteStem> &stems)
{
    return *std::min_element(stems.begin(), stems.end(),
                             &compareStemTopPositions);
}

NoteStem BeamGroup::findLowestStem(const std::vector<NoteStem> &stems)
{
    return *std::max_element(stems.begin(), stems.end(),
                             &compareStemBottomPositions);
}

#if 0
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
void BeamGroup::copyNoteStems(std::vector<NoteStem>& stems) const
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
#endif
