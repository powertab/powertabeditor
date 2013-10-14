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

#include "editinstrument.h"

#include <score/score.h>
#include <widgets/instruments/instrumentpanel.h>

EditInstrument::EditInstrument(Score &score, InstrumentPanel *panel, int index,
                               const Instrument &instrument)
    : QUndoCommand(QObject::tr("Edit Instrument")),
      myScore(score),
      myInstrumentPanel(panel),
      myInstrumentIndex(index),
      myNewInstrument(instrument),
      myOriginalInstrument(score.getInstruments()[index])
{
}

void EditInstrument::redo()
{
    myScore.getInstruments()[myInstrumentIndex] = myNewInstrument;
    if (myInstrumentPanel)
        myInstrumentPanel->update(myScore);
}

void EditInstrument::undo()
{
    myScore.getInstruments()[myInstrumentIndex] = myOriginalInstrument;
    if (myInstrumentPanel)
        myInstrumentPanel->update(myScore);
}
