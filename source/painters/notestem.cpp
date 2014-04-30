/*
  * Copyright (C) 2013 Cameron White
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

#include <painters/layoutinfo.h>
#include <score/position.h>
#include <score/voiceutils.h>

NoteStem::NoteStem(const Voice &voice, const Position &pos, double x,
                   double noteHeadWidth,
                   const std::vector<double> &noteLocations)
    : myVoice(&voice),
      myPosition(&pos),
      myX(x),
      myNoteHeadWidth(noteHeadWidth),
      myTop(0),
      myBottom(0)
{
    std::vector<double>::const_iterator it;

    it = std::min_element(noteLocations.begin(), noteLocations.end());
    if (it != noteLocations.end())
        myTop = *it;

    it = std::max_element(noteLocations.begin(), noteLocations.end());
    if (it != noteLocations.end())
        myBottom = *it;

    myStemType = LayoutInfo::STD_NOTATION_LINE_SPACING * 2 < myBottom ?
                StemUp : StemDown;
}

double NoteStem::getX() const
{
    return myX;
}

void NoteStem::setX(double x)
{
    myX = x;
}

double NoteStem::getTop() const
{
    return myTop;
}

void NoteStem::setTop(double top)
{
    myTop = top;
}

double NoteStem::getBottom() const
{
    return myBottom;
}

void NoteStem::setBottom(double bottom)
{
    myBottom = bottom;
}

double NoteStem::getStemEdge() const
{
    return (myStemType == StemUp) ? myTop : myBottom;
}

double NoteStem::getStemHeight() const
{
    double height = LayoutInfo::STD_NOTATION_LINE_SPACING * 3.5;

    if (isGraceNote())
        height *= 0.75;

    return height;
}

double NoteStem::getNoteHeadRightEdge() const
{
    return myX + myNoteHeadWidth;
}

double NoteStem::getNoteHeadWidth() const
{
    return myNoteHeadWidth;
}

double NoteStem::getDurationTime() const
{
    return VoiceUtils::getDurationTime(*myVoice, *myPosition);
}

int NoteStem::getPositionIndex() const
{
    return myPosition->getPosition();
}

Position::DurationType NoteStem::getDurationType() const
{
    return myPosition->getDurationType();
}

bool NoteStem::isStaccato() const
{
    return myPosition->hasProperty(Position::Staccato);
}

bool NoteStem::isBeamable(const NoteStem &stem)
{
    return !stem.myPosition->isRest() && !stem.myPosition->hasMultiBarRest() &&
            stem.myPosition->getDurationType() >= Position::EighthNote;
}

bool NoteStem::needsStem(const NoteStem &stem)
{
    return !stem.myPosition->isRest() && !stem.myPosition->hasMultiBarRest() &&
           (stem.isGraceNote() ||
            stem.myPosition->getDurationType() != Position::WholeNote);
}

bool NoteStem::canHaveFlag(const NoteStem &stem)
{
    return stem.myPosition->getDurationType() > Position::QuarterNote ||
            stem.isGraceNote();
}

NoteStem::StemType NoteStem::getStemType() const
{
    return myStemType;
}

void NoteStem::setStemType(StemType type)
{
    myStemType = type;
}

bool NoteStem::isGraceNote() const
{
    return myPosition->hasProperty(Position::Acciaccatura);
}

bool NoteStem::hasFermata() const
{
    return myPosition->hasProperty(Position::Fermata);
}

bool NoteStem::hasSforzando() const
{
    return myPosition->hasProperty(Position::Sforzando);
}

bool NoteStem::hasMarcato() const
{
    return myPosition->hasProperty(Position::Marcato);
}
