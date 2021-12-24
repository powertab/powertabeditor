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
  
#ifndef CHANGEPOSITIONSPACING_H
#define CHANGEPOSITIONSPACING_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

class System;

class ChangePositionSpacing : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(ChangePositionSpacing)

public:
    ChangePositionSpacing(boost::shared_ptr<System> system, uint8_t spacing);
    virtual void undo();
    virtual void redo();

private:
    boost::shared_ptr<System> system;
    const uint8_t newSpacing;
    const uint8_t originalSpacing;
};

#endif // CHANGEPOSITIONSPACING_H
