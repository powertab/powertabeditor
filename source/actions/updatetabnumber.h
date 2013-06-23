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
  
#ifndef UPDATETABNUMBER_H
#define UPDATETABNUMBER_H

#include <QUndoCommand>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

class Staff;
class Position;
class Note;

class UpdateTabNumber : public QUndoCommand
{
public:
    UpdateTabNumber(uint8_t typedNumber, Note* note, Position* position,
                    uint32_t voice, boost::shared_ptr<Staff> staff);
    ~UpdateTabNumber();
    void undo();
    void redo();

private:
    const uint32_t voice;
    Note* note;
    Position* position;
    boost::shared_ptr<Staff> staff;
    uint8_t newFretNumber;
    uint8_t prevFretNumber;
    Note *origPrevNote;
    Note *origNextNote;
    Note *origNote;
};

#endif // UPDATETABNUMBER_H
