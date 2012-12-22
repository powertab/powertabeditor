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

#ifndef REMOVEIRREGULARGROUPING_H
#define REMOVEIRREGULARGROUPING_H

#include <QUndoCommand>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

class Staff;
class Position;

class RemoveIrregularGrouping : public QUndoCommand
{
public:
    RemoveIrregularGrouping(boost::shared_ptr<Staff> staff, Position* position,
                            uint32_t voice);

    void redo();
    void undo();

private:
    boost::shared_ptr<Staff> staff;
    Position* position;
    const uint32_t voice;
    const std::pair<size_t, size_t> irregularGroupBounds;
    uint8_t notesPlayed, notesPlayedOver;

    std::pair<size_t, size_t> findIrregularGroup() const;
    size_t findGroupEnd(size_t startIndex) const;
    size_t findGroupStart(size_t startIndex) const;
};

#endif // REMOVEIRREGULARGROUPING_H
