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
  
#ifndef ADDSYSTEM_H
#define ADDSYSTEM_H

#include <QUndoCommand>

class Score;

class AddSystem : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    AddSystem(Score* score, quint32 index);
    void redo();
    void undo();

signals:
    // need to trigger a full redraw of the whole score when adding a system
    void triggered();

protected:
    Score* score;
    const quint32 index;
};

#endif // ADDSYSTEM_H
