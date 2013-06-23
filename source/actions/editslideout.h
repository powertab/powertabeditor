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
  
#ifndef EDITSLIDEOUT_H
#define EDITSLIDEOUT_H

#include <QUndoCommand>

class Note;

class EditSlideOut : public QUndoCommand
{
public:
    EditSlideOut(Note* note, quint8 slideType, qint8 steps);
    void redo();
    void undo();

private:
    Note* note;
    const quint8 newSlideType;
    quint8 oldSlideType;
    const qint8 newSteps;
    qint8 oldSteps;

    bool oldHammerOn;
    bool oldPullOff;
};

#endif // EDITSLIDEOUT_H
