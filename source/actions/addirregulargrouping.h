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

#ifndef ADDIRREGULARGROUPING_H
#define ADDIRREGULARGROUPING_H

#include <QUndoCommand>
#include <vector>
#include <boost/cstdint.hpp>

class Position;

class AddIrregularGrouping : public QUndoCommand
{
public:
    AddIrregularGrouping(const std::vector<Position*>& positions,
                         uint8_t notesPlayed, uint8_t notesPlayedOver);

    void redo();
    void undo();

private:
    const std::vector<Position*> positions;
    const uint8_t notesPlayed;
    const uint8_t notesPlayedOver;
};

#endif // ADDIRREGULARGROUPING_H
