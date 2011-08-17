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

#ifndef ADDVOLUMESWELL_H
#define ADDVOLUMESWELL_H

#include <QUndoCommand>
#include <boost/cstdint.hpp>

class Position;

class AddVolumeSwell : public QUndoCommand
{
public:
    AddVolumeSwell(Position* position, uint8_t startVol,
                   uint8_t endVol, uint8_t duration);

    void redo();
    void undo();

private:
    Position* position;
    const uint8_t startVol;
    const uint8_t endVol;
    const uint8_t duration;
};

#endif // ADDVOLUMESWELL_H
