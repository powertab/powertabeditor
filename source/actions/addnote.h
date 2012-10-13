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
  
#ifndef ADDNOTE_H
#define ADDNOTE_H

#include <QUndoCommand>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

class Staff;
class Position;
class Note;

class AddNote : public QUndoCommand
{
public:
    AddNote(uint8_t stringNum, uint8_t fretNumber, uint32_t positionIndex,
            uint32_t voice, boost::shared_ptr<Staff> staff);
    ~AddNote();

    void redo();
    void undo();

private:
    const uint8_t stringNum;
    uint32_t positionIndex;
    uint32_t voice;
    boost::shared_ptr<Staff> staff;
    Note* note;
    Position* position;

    const bool wasRest; ///< Whether the position was originally a rest.
    bool newPositionAdded;
    bool undone;
};

#endif // ADDNOTE_H
