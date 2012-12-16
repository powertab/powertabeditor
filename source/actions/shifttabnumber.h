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
  
#ifndef SHIFTTABNUMBER_H
#define SHIFTTABNUMBER_H

#include <QUndoCommand>
#include <boost/shared_ptr.hpp>

#include <powertabdocument/note.h>
#include <powertabdocument/position.h>

class Note;
class Staff;
class Tuning;

class ShiftTabNumber : public QUndoCommand
{
public:
    ShiftTabNumber(boost::shared_ptr<Staff> staff, Position* position,
                   Note* note, uint32_t voice, Position::ShiftType direction,
                   const Tuning& tuning);
    void undo();
    void redo();

private:
    uint32_t voice;
    boost::shared_ptr<Staff> staff;
    Position* position;
    Note* note;
    const bool shiftUp;
    const Tuning& tuning;

    Note* prevNote;
    Note* nextNote;
    Note origPrevNote;
    Note origNextNote;
    Note origNote;
};

#endif // SHIFTTABNUMBER_H
