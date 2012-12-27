/*
  * Copyright (C) 2012 Cameron White
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

#ifndef DELETENOTE_H
#define DELETENOTE_H

#include <QUndoCommand>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <powertabdocument/note.h>

class Staff;
class Position;
class Note;

class DeleteNote : public QUndoCommand
{
public:
    DeleteNote(boost::shared_ptr<Staff> staff, uint32_t voice,
               Position* position, uint8_t string, bool clearEmptyPositions);
    ~DeleteNote();

    void redo();
    void undo();

    static bool canExecute(Position* position, uint8_t string);

private:
    boost::shared_ptr<Staff> staff;
    const uint32_t voice;
    Position* position;
    const uint8_t string;

    bool deletePosition;

    Note* note;
    Note* prevNote;
    Note origPrevNote;
    Note* nextNote;
    Note origNextNote;
};

#endif // DELETENOTE_H
