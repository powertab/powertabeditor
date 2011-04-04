#include "beamgroup.h"

#include <algorithm>
#include <functional>

#include "stdnotationpainter.h"
#include <powertabdocument/position.h>

#include <QGraphicsLineItem>
#include <QPen>

#include <cmath> // for log() function

BeamGroup::BeamGroup(const StaffData& staffInfo, const std::vector<NoteStem>& noteStems) :
    staffInfo_(staffInfo),
    noteStems_(noteStems)
{
    setStemDirections();
    adjustStemHeights();
}

/// Functor to compare a NoteStem's stem direction
struct CompareStemDirection
{
    CompareStemDirection(NoteStem::StemDirection direction) : direction_(direction)
    {
    }

    bool operator()(const NoteStem& stem) const
    {
        return stem.direction() == direction_;
    }

    NoteStem::StemDirection direction_;
};

/// Sets the stem directions for all stems in the beam group
/// This depends on how many stems currently point upwards/downwards
void BeamGroup::setStemDirections()
{
    // Find how many stem directions of each type we have
    const size_t stemsUp = std::count_if(noteStems_.begin(), noteStems_.end(),
                                         CompareStemDirection(NoteStem::StemUp));

    const size_t stemsDown = std::count_if(noteStems_.begin(), noteStems_.end(),
                                           CompareStemDirection(NoteStem::StemDown));

    stemDirection_ = (stemsDown >= stemsUp) ? NoteStem::StemDown : NoteStem::StemUp;

    // Assign the new stem direction to each stem
    std::for_each(noteStems_.begin(), noteStems_.end(),
                  std::bind2nd(std::mem_fun_ref(&NoteStem::setDirection), stemDirection_));
}

/// Compares either the top or bottom position of a stem, depending on the function
/// that is passed to the constructor
struct CompareStemPositions
{
    typedef std::function<double (const NoteStem&)> PositionGetter;

    CompareStemPositions(PositionGetter posFn) : posFn(posFn) {}

    bool operator()(const NoteStem& stem1, const NoteStem& stem2) const
    {
        return posFn(stem1) < posFn(stem2);
    }

    PositionGetter posFn;
};

/// Stretches the beams to a common high/low height, depending on stem direction
void BeamGroup::adjustStemHeights()
{
    if (stemDirection_ == NoteStem::StemUp)
    {
        NoteStem highestStem = *std::min_element(noteStems_.begin(), noteStems_.end(),
                                                 CompareStemPositions(&NoteStem::top));

        for (auto stem = noteStems_.begin(); stem != noteStems_.end(); ++stem)
        {
            stem->setX(stem->x() + staffInfo_.getNoteHeadRightEdge() - 1);
            stem->setTop(highestStem.top() - highestStem.stemSize());
        }
    }
    else // stem down
    {
        NoteStem lowestStem = *std::max_element(noteStems_.begin(), noteStems_.end(),
                                                CompareStemPositions(&NoteStem::bottom));

        for (auto stem = noteStems_.begin(); stem != noteStems_.end(); ++stem)
        {
            stem->setX(stem->x() + staffInfo_.getNoteHeadRightEdge() - StdNotationPainter::getNoteHeadWidth());

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
    // Draw each stem
    for (auto stem = noteStems_.begin(); stem != noteStems_.end(); ++stem)
    {
        QGraphicsLineItem* line = new QGraphicsLineItem;
        line->setLine(stem->x(), stem->top(), stem->x(), stem->bottom());
        line->setParentItem(parent);

        // draw any symbols that use information about the stem, like staccato, fermata, etc
        QList<QGraphicsItem*> symbols;

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
    }

    // draw connecting line
    if (noteStems_.size() > 1)
    {
        const double connectorHeight = noteStems_.front().stemEdge();

        QGraphicsLineItem* connector = new QGraphicsLineItem;
        connector->setLine(noteStems_.front().x() + 1, connectorHeight,
                           noteStems_.back().x() - 1, connectorHeight);

        connector->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
        connector->setParentItem(parent);
    }

     // draw a note flag for single notes (eighth notes or less)
    if (noteStems_.size() == 1 && noteStems_.front().position()->GetDurationType() > 4)
    {
        QGraphicsItem* flag = noteStems_.front().createNoteFlag();
        flag->setParentItem(parent);
    }

    drawExtraBeams(parent);
}

/// Draws the extra beams required for sixteenth notes, etc
void BeamGroup::drawExtraBeams(QGraphicsItem* parent) const
{
    for (auto stem = noteStems_.begin(); stem != noteStems_.end(); ++stem)
    {
        // 16th note gets 1 extra beam, 32nd gets two, etc
        // Calculate log_2 of the note duration, and subtract three (so log_2(16) - 3 = 1)
        const int extraBeams = log(stem->position()->GetDurationType()) / log(2) - 3;

        const bool hasFullBeaming = (stem->position()->GetPreviousBeamDurationType() ==
                                     stem->position()->GetDurationType());

        const bool hasFractionalLeft = stem->position()->HasFractionalLeftBeam();
        const bool hasFractionalRight = stem->position()->HasFractionalRightBeam();

        if (hasFullBeaming || hasFractionalLeft || hasFractionalRight)
        {
            for (int i = 1; i <= extraBeams; i++)
            {
                double y = stem->stemEdge();
                y += i * 3 * ((stemDirection_ == NoteStem::StemUp) ? 1 : -1);

                double xStart = 0, xEnd = 0;

                const double FRACTIONAL_BEAM_WIDTH = 5;

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

                QGraphicsLineItem* line = new QGraphicsLineItem;
                line->setLine(xStart, y, xEnd, y);
                line->setPen(QPen(Qt::black, 2.0, Qt::SolidLine, Qt::RoundCap));
                line->setParentItem(parent);
            }
        }
    }

}
