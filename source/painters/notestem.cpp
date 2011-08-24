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
  
#include "notestem.h"

#include "staffdata.h"
#include <algorithm>

#include <powertabdocument/position.h>
#include <powertabdocument/staff.h>
#include <QGraphicsTextItem>
#include <painters/musicfont.h>

namespace {
    const QFont musicNotationFont = MusicFont().getFont();
}

/// Constructor
/// @param position Pointer to the position object
/// @param x X-coordinate of the position
/// @param noteLocations A list of the y-coordinates of each note at the position
NoteStem::NoteStem(const StaffData& staffInfo, const Position* position,
                   double x, const std::vector<double>& noteLocations) :
    position_(position),
    x_(x),
    top_(0),
    bottom_(0),
    stemSize_(0),
    staffInfo_(staffInfo)
{
    stemSize_ = Staff::STD_NOTATION_LINE_SPACING * 3.5;

    if (position->IsAcciaccatura()) // shorter stem for grace notes
    {
        stemSize_ *= 0.75;
    }

    if (!noteLocations.empty())
    {
        top_ = *std::min_element(noteLocations.begin(), noteLocations.end());
        bottom_ = *std::max_element(noteLocations.begin(), noteLocations.end());
    }

    top_ += staffInfo_.getTopStdNotationLine();
    bottom_ += staffInfo_.getTopStdNotationLine();

    direction_ = (staffInfo_.getStdNotationLineHeight(3) < bottom_) ? StemUp : StemDown;
}

const Position* NoteStem::position() const
{
    return position_;
}

double NoteStem::x() const
{
    return x_;
}

void NoteStem::setX(double x)
{
    x_ = x;
}

double NoteStem::top() const
{
    return top_;
}

void NoteStem::setTop(double top)
{
    top_ = top;
}

double NoteStem::bottom() const
{
    return bottom_;
}

void NoteStem::setBottom(double bottom)
{
    bottom_ = bottom;
}

NoteStem::StemDirection NoteStem::direction() const
{
    return direction_;
}

void NoteStem::setDirection(StemDirection direction)
{
    direction_ = direction;
}

double NoteStem::stemSize() const
{
    return stemSize_;
}

QGraphicsItem* NoteStem::createNoteFlag() const
{
    Q_ASSERT(canDrawFlag());

    // choose the flag symbol, depending on duration and stem direction
    QChar symbol = 0;
    if (direction_ == StemUp)
    {
        switch(position_->GetDurationType())
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

        if (position_->IsAcciaccatura())
        {
            symbol = MusicFont::FlagUp1;
        }
    }
    else
    {
        switch(position_->GetDurationType())
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

        if (position_->IsAcciaccatura())
        {
            symbol = MusicFont::FlagDown1;
        }
    }

    // draw the symbol
    const double y = stemEdge() - 35; // adjust for spacing caused by the music symbol font
    QGraphicsTextItem* flag = new QGraphicsTextItem(symbol);
    flag->setFont(musicNotationFont);
    flag->setPos(x_ - 3, y);

    return flag;
}

/// Returns the y-coordinate of the edge of the stem (where it meets the connecting beam)
double NoteStem::stemEdge() const
{
    return (direction_ == StemUp) ? top_ : bottom_;
}

// Creates and positions a staccato symbol
QGraphicsItem* NoteStem::createStaccato() const
{
    // draw the dot near either the top or bottom note of the position, depending on stem direction
    const double yPos = (direction_ == StemUp) ? bottom_ - 25 : top_ - 43;
    const double xPos = (direction_ == StemUp) ? x_ - 8 : x_ - 2;

    QGraphicsTextItem* dot = new QGraphicsTextItem(QChar(MusicFont::Dot));
    dot->setFont(musicNotationFont);
    dot->setPos(xPos, yPos);
    return dot;
}

/// Creates and positions a fermata symbol
QGraphicsItem* NoteStem::createFermata() const
{
    double y = 0;

    // position the fermata directly above/below the staff if possible, unless the note stem extends
    // beyond the std. notation staff.
    // After positioning, offset the height due to the way that QGraphicsTextItem positions text
    if (direction_ == StemUp)
    {
        y = std::min<double>(top_, staffInfo_.getTopStdNotationLine());
        y -= 33;
    }
    else
    {
        y = std::max<double>(bottom_, staffInfo_.getBottomStdNotationLine());
        y -= 25;
    }

    const QChar symbol = (direction_ == StemUp) ? MusicFont::FermataUp : MusicFont::FermataDown;
    QGraphicsSimpleTextItem* fermata = new QGraphicsSimpleTextItem(symbol);
    fermata->setFont(musicNotationFont);
    fermata->setPos(x_, y);

    return fermata;
}

/// Creates and positions an accent symbol
QGraphicsItem* NoteStem::createAccent() const
{
    double y = 0;

    // position the accent directly above/below the staff if possible, unless the note stem extends
    // beyond the std. notation staff.
    // - it should be positioned opposite to the fermata symbols
    // After positioning, offset the height due to the way that QGraphicsTextItem positions text
    if (direction_ == StemDown)
    {
        y = std::min<double>(top_, staffInfo_.getTopStdNotationLine());
        y -= 38;
    }
    else
    {
        y = std::max<double>(bottom_, staffInfo_.getBottomStdNotationLine());
        y -= 20;
    }

    QChar symbol;

    if (position_->HasMarcato())
    {
        symbol = MusicFont::getSymbol(MusicFont::Marcato);
    }
    else if (position_->HasSforzando())
    {
        symbol = MusicFont::getSymbol(MusicFont::Sforzando);
        y += 3;
    }

    if (position_->IsStaccato())
    {
        y += (direction_ == StemUp) ? 7 : -7;
    }

    QGraphicsSimpleTextItem* accent = new QGraphicsSimpleTextItem(symbol);
    accent->setFont(musicNotationFont);
    accent->setPos(x_, y);

    return accent;
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

/// Finds the most common stem direction for a group of NoteStem's
NoteStem::StemDirection findDirectionForGroup(const std::vector<NoteStem>& stems)
{
    // Find how many stem directions of each type we have
    const size_t stemsUp = std::count_if(stems.begin(), stems.end(),
                                         CompareStemDirection(NoteStem::StemUp));

    const size_t stemsDown = std::count_if(stems.begin(), stems.end(),
                                           CompareStemDirection(NoteStem::StemDown));

    return (stemsDown >= stemsUp) ? NoteStem::StemDown : NoteStem::StemUp;
}

/// Returns true if we can draw a flag for this note stem (must be eighth note or higher, or a grace note)
bool NoteStem::canDrawFlag() const
{
    return (position_->GetDurationType() > 4) || position_->IsAcciaccatura();
}
