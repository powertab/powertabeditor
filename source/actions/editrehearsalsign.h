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
  
#ifndef EDITREHEARSALSIGN_H
#define EDITREHEARSALSIGN_H

#include <QUndoCommand>

class Barline;
class RehearsalSign;

class EditRehearsalSign : public QUndoCommand
{
public:
    EditRehearsalSign(RehearsalSign& rehearsalSign, bool isShown,
                      uint8_t letter = 0, const std::string& description = "");

    virtual void undo();
    virtual void redo();

protected:
    RehearsalSign& rehearsalSign;
    bool isShown;
    quint8 letter;
    std::string description;

    void showHide(bool show);
};

#endif // EDITREHEARSALSIGN_H
