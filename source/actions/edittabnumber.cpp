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
  
#include "edittabnumber.h"

EditTabNumber::EditTabNumber(const ScoreLocation &location, int typedNumber)
    : QUndoCommand(tr("Edit Tab Number")),
      myLocation(location),
      myOriginalNumber(location.getNote()->getFretNumber()),
      myTappedHarmonicOffset(location.getNote()->hasTappedHarmonic()
                                 ? location.getNote()->getTappedHarmonicFret() -
                                       myOriginalNumber
                                 : 0)
{
    const int prevNumber = location.getNote()->getFretNumber();
    // If the old fret number was a 1 or 2 then we are typing a double digit
    // number.
    if (prevNumber == 1 || prevNumber == 2)
        myNewNumber = prevNumber * 10 + typedNumber;
    else
        myNewNumber = typedNumber;
}

void EditTabNumber::redo()
{
    // TODO - update the previous and next notes if there are slides, etc.
    myLocation.getNote()->setFretNumber(myNewNumber);

    // Update tapped harmonics.
    if (myTappedHarmonicOffset)
    {
        const int tappedFret = myNewNumber + myTappedHarmonicOffset;

        if (tappedFret <= Note::MAX_FRET_NUMBER)
            myLocation.getNote()->setTappedHarmonicFret(tappedFret);
        else
            myLocation.getNote()->clearTappedHarmonic();
    }
}

void EditTabNumber::undo()
{
    myLocation.getNote()->setFretNumber(myOriginalNumber);

    if (myTappedHarmonicOffset)
    {
        myLocation.getNote()->setTappedHarmonicFret(myOriginalNumber +
                                                    myTappedHarmonicOffset);
    }
}
