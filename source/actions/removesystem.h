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
  
#ifndef REMOVESYSTEM_H
#define REMOVESYSTEM_H

#include <QUndoCommand>
#include <boost/shared_ptr.hpp>

class Score;
class System;

class RemoveSystem : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    RemoveSystem(Score* score, quint32 index);
    void redo();
    void undo();

signals:
    void triggered();

protected:
    Score* score;
    boost::shared_ptr<System> systemCopy;
    const quint32 index;
};

#endif // REMOVESYSTEM_H
