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
  
#ifndef CHANGEBARLINETYPE_H
#define CHANGEBARLINETYPE_H

#include <QUndoCommand>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

class Barline;
class System;

/// Modifies a barline's properties.
class ChangeBarLineType : public QUndoCommand
{
public:
    ChangeBarLineType(boost::shared_ptr<System> system,
                      boost::shared_ptr<Barline> bar, uint8_t barType,
                      uint8_t repeats);
    virtual void undo();
    virtual void redo();

private:
    boost::shared_ptr<System> system;
    boost::shared_ptr<Barline> barLine;
    const uint8_t type;
    const uint8_t repeatCount;
    const uint8_t originalType;
    const uint8_t originalRepeatCount;
};

#endif // CHANGEBARLINETYPE_H
