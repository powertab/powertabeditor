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

#include <cmath>
#include <painters/layoutinfo.h>
#include <painters/musicfont.h>
#include <painters/simpletextitem.h>
#include <QFontMetricsF>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QPen>

static constexpr double STEM_TO_NOTE_OFFSET = 3;
static const double FRACTIONAL_BEAM_WIDTH = 5.0;

static int getNumExtraBeams(const NoteStem &stem)
{
    // 16th note gets 1 extra beam, 32nd gets two, etc.
    // Calculate log_2 of the note duration, and subtract three (so log_2(16) - 3 = 1).
    return std::log(static_cast<double>(stem.getDurationType())) /
               std::log(2.0) - 3;
}

BeamGroup::BeamGroup(NoteStem::StemType direction,
                     const std::vector<size_t> &stems,
                     const QColor &color)
    : myStemDirection(direction), myStems(stems), myBeamColor(color)
{
}

void
BeamGroup::drawStems(QGraphicsItem *parent, const std::vector<NoteStem> &stems,
                     const QFont &musicFont, const LayoutInfo &layout) const
{
    QList<QGraphicsItem *> symbols;
    QPainterPath stemPath;

    std::vector<NoteStem> group_stems;
    for (size_t i : myStems)
        group_stems.push_back(stems[i]);

    auto begin = group_stems.begin();
    auto end = group_stems.end();
    const NoteStem &firstStem = *begin;
    const NoteStem &lastStem = *(end - 1);

    // Draw each stem.
    for (const NoteStem &stem : group_stems)
    {
        stemPath.moveTo(stem.getX(), stem.getTop());
        stemPath.lineTo(stem.getX(), stem.getBottom());

        // Draw any symbols that use information about the stem, like staccato,
        // fermata, etc.
        if (stem.isStaccato())
            symbols << createStaccato(stem, musicFont, myBeamColor);

        if (stem.hasFermata())
            symbols << createFermata(stem, musicFont, layout, myBeamColor );

        if (stem.hasSforzando() || stem.hasMarcato())
            symbols << createAccent(stem, musicFont, layout, myBeamColor);

        for (QGraphicsItem *&symbol : symbols)
            symbol->setParentItem(parent);

        symbols.clear();
    }

    auto stemPathItem = new QGraphicsPathItem(stemPath);
    stemPathItem->setPen(QPen(myBeamColor, 1.0, Qt::SolidLine));
    stemPathItem->setParentItem(parent);

    QPainterPath beamPath;

    // Draw connecting line.
    if (group_stems.size() > 1)
    {
        const double connectorHeight = firstStem.getStemEdge();

        beamPath.moveTo(firstStem.getX() + 0.5, connectorHeight);
        beamPath.lineTo(lastStem.getX(), connectorHeight);
    }

    drawExtraBeams(beamPath, begin, end);

    auto beams = new QGraphicsPathItem(beamPath);
    beams->setPen(QPen(myBeamColor, 2.0, Qt::SolidLine, Qt::RoundCap));
    beams->setParentItem(parent);

    // Draw a note flag for single notes (eighth notes or less) or grace notes.
    if (group_stems.size() == 1 && NoteStem::canHaveFlag(firstStem))
    {
        QGraphicsItem *flag = createNoteFlag(firstStem, musicFont, myBeamColor);
        flag->setParentItem(parent);
    }
}

void BeamGroup::drawExtraBeams(QPainterPath &path,
                               std::vector<NoteStem>::const_iterator begin,
                               std::vector<NoteStem>::const_iterator end) const
{
    for (auto stem = begin; stem != end; ++stem)
    {
        std::vector<NoteStem>::const_iterator prevStem;
        if (stem != begin)
			prevStem = std::prev(stem);

        auto nextStem = std::next(stem);
        const Position::DurationType duration = stem->getDurationType();
        const Position::DurationType prevDuration = (stem != begin) ?
                    prevStem->getDurationType() : Position::SixtyFourthNote;
        const Position::DurationType nextDuration = (nextStem != end) ?
                    nextStem->getDurationType() : Position::SixtyFourthNote;

        const int extraBeams = getNumExtraBeams(*stem);
        if (extraBeams < 1)
            continue;

        const bool hasFractionalLeft =
            stem != begin &&
            ((duration > prevDuration &&
              (nextStem == end || nextDuration == Position::EighthNote)) ||
             nextStem == end || !nextStem->hasFullBeaming());
        const bool hasFractionalRight =
            !hasFractionalLeft &&
            (duration > nextDuration ||
             (nextStem != end && !stem->hasFullBeaming()));

        if (stem->hasFullBeaming() || hasFractionalLeft || hasFractionalRight)
        {
            for (int i = 1; i <= extraBeams; i++)
            {
                double y = stem->getStemEdge();
                y += i * 3 * ((myStemDirection == NoteStem::StemUp) ? 1 : -1);

                double xStart = 0, xEnd = 0;

                if (stem->hasFullBeaming() && i <= getNumExtraBeams(*prevStem))
                {
                    xStart = prevStem->getX() + 0.5;
                    xEnd = stem->getX() - 1;
                }
                else if (hasFractionalRight)
                {
                    xStart = stem->getX() + 0.5;
                    xEnd = xStart + FRACTIONAL_BEAM_WIDTH;
                }
                else if (hasFractionalLeft)
                {
                    xEnd = stem->getX();
                    xStart = xEnd - FRACTIONAL_BEAM_WIDTH;
                }

                path.moveTo(xStart, y);
                path.lineTo(xEnd, y);
            }
        }
    }

}

QGraphicsItem *BeamGroup::createStaccato(const NoteStem &stem,
                                         const QFont &musicFont,
                                         const QColor &color)
{
    // Draw the dot near either the top or bottom note of the position,
    // depending on stem direction.
    static constexpr double VERTICAL_SPACING = 8;
    const double y = (stem.getStemType() == NoteStem::StemUp)
                         ? stem.getBottom() + VERTICAL_SPACING
                         : stem.getTop() - VERTICAL_SPACING;

    const double x = (stem.getStemType() == NoteStem::StemUp)
                         ? stem.getX() - STEM_TO_NOTE_OFFSET
                         : stem.getX() + STEM_TO_NOTE_OFFSET;

    auto dot = new SimpleTextItem(QChar(MusicFont::Dot), musicFont, 
                                    TextAlignment::Baseline,QPen(color));
    dot->setPos(x, y);
    return dot;
}

QGraphicsItem *BeamGroup::createFermata(const NoteStem &stem,
                                        const QFont &musicFont,
                                        const LayoutInfo &layout,
                                        const QColor &color)
{
    static constexpr double padding = 4;
    // Position the fermata directly above/below the staff if possible, unless
    // the note stem extends beyond the standard notation staff.
    double y = 0;
    if (stem.getStemType() == NoteStem::StemUp)
    {
        y = std::min(stem.getTop(), layout.getTopStdNotationLine()) - padding;
    }
    else
    {
        y = std::max(stem.getBottom(), layout.getBottomStdNotationLine()) +
            padding;
    }

    const QChar symbol = (stem.getStemType() == NoteStem::StemUp) ?
                MusicFont::FermataUp : MusicFont::FermataDown;
    auto fermata = new SimpleTextItem(symbol, musicFont, TextAlignment::Baseline,QPen(color));
    fermata->setPos(stem.getX(), y);

    return fermata;
}

QGraphicsItem *BeamGroup::createAccent(const NoteStem &stem,
                                       const QFont &musicFont,
                                       const LayoutInfo &layout,
                                       const QColor &color)
{
    static constexpr double padding = 7;
    static constexpr double staccato_offset = padding;
    double x = stem.getX();
    double y = 0;

    // Position the accent directly above/below the staff if possible, unless
    // the note stem extends beyond the std. notation staff.
    // - It should be positioned opposite to the fermata symbols.
    // After positioning, offset the height due to the way that
    // QGraphicsTextItem positions text.
    if (stem.getStemType() == NoteStem::StemDown)
    {
        y = std::min(stem.getTop(), layout.getTopStdNotationLine()) - padding;
        if (stem.isStaccato() &&
            (stem.getTop() - padding) < layout.getTopStdNotationLine())
        {
            y -= staccato_offset;
        }

        x += STEM_TO_NOTE_OFFSET;
    }
    else
    {
        y = std::max(stem.getBottom(), layout.getBottomStdNotationLine()) +
            padding;
        if (stem.isStaccato() &&
            (stem.getBottom() + padding) > layout.getBottomStdNotationLine())
        {
            y += staccato_offset;
        }

        x -= STEM_TO_NOTE_OFFSET;
    }

    QChar symbol;
    if (stem.hasMarcato())
        symbol = MusicFont::Marcato;
    else if (stem.hasSforzando())
    {
        symbol = MusicFont::Sforzando;
        // The bottom of this symbol is aligned with the baseline, so shift
        // down a bit to align with the marcato symbol.
        y += 3;
        // Shift slightly right to be more horizontally centered.
        x += 1;
    }

    auto accent =
	new SimpleTextItem(symbol, musicFont, TextAlignment::Baseline, QPen(color));
    accent->setPos(x, y);

    return accent;
}

QGraphicsItem *
BeamGroup::createNoteFlag(const NoteStem &stem, const QFont &musicFont,
                                         const QColor &flagColor)
{
    Q_ASSERT(NoteStem::canHaveFlag(stem));

    QChar symbol = 0;

    // Choose the flag symbol, depending on duration and stem direction.
    if (stem.getStemType() == NoteStem::StemUp)
    {
        switch (stem.getDurationType())
        {
        case Position::EighthNote:
            symbol = MusicFont::FlagUp1;
            break;
        case Position::SixteenthNote:
            symbol = MusicFont::FlagUp2;
            break;
        case Position::ThirtySecondNote:
            symbol = MusicFont::FlagUp3;
            break;
        case Position::SixtyFourthNote:
            symbol = MusicFont::FlagUp4;
            break;
        default:
            break;
        }

        if (stem.isGraceNote())
            symbol = MusicFont::FlagUp1;
    }
    else
    {
        switch (stem.getDurationType())
        {
        case Position::EighthNote:
            symbol = MusicFont::FlagDown1;
            break;
        case Position::SixteenthNote:
            symbol = MusicFont::FlagDown2;
            break;
        case Position::ThirtySecondNote:
            symbol = MusicFont::FlagDown3;
            break;
        case Position::SixtyFourthNote:
            symbol = MusicFont::FlagDown4;
            break;
        default:
            break;
        }

        if (stem.isGraceNote())
            symbol = MusicFont::FlagDown1;
    }

    // Draw the symbol.
    const double y = stem.getStemEdge();
    auto flag = new SimpleTextItem(symbol, musicFont, TextAlignment::Baseline,
        QPen(flagColor, 1.0));
    flag->setPos(stem.getX(), y);

    // For grace notes, add a slash through the stem.
    if (stem.isGraceNote())
    {
        auto group = new QGraphicsItemGroup();
        group->addToGroup(flag);

        const QChar slash_symbol = stem.getStemType() == NoteStem::StemUp
                                       ? MusicFont::GraceNoteSlashUp
                                       : MusicFont::GraceNoteSlashDown;

        auto slash = new SimpleTextItem(slash_symbol, musicFont, 
	        TextAlignment::Baseline, QPen(flagColor));
        slash->setPos(stem.getX() + 1, y);
        group->addToGroup(slash);

        return group;
    }
    else
        return flag;
}
