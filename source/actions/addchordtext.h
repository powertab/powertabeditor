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
  
#ifndef ADDCHORDTEXT_H
#define ADDCHORDTEXT_H

#include <QUndoCommand>
#include <memory>

class ChordText;
class System;

class AddChordText : public QUndoCommand
{
public:
    AddChordText(std::shared_ptr<System> system, std::shared_ptr<ChordText> chordText, quint32 index);
    virtual void undo();
    virtual void redo();

protected:
    std::shared_ptr<ChordText> chordText;
    std::shared_ptr<System> system;
    const quint32 index;
};

#endif // ADDCHORDTEXT_H
