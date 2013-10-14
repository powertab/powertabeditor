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

#ifndef ACTIONS_EDITINSTRUMENT_H
#define ACTIONS_EDITINSTRUMENT_H

#include <QUndoCommand>
#include <score/player.h>
#include <score/score.h>

class InstrumentPanel;

class EditInstrument : public QUndoCommand
{
public:
    EditInstrument(Score &score, InstrumentPanel *panel, int index,
                   const Instrument &instrument);

    virtual void redo() override;
    virtual void undo() override;

private:
    Score &myScore;
    InstrumentPanel *myInstrumentPanel;
    const int myInstrumentIndex;
    const Instrument myNewInstrument;
    const Instrument myOriginalInstrument;
};

#endif
