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
#include <boost/cstdint.hpp>

class Barline;
class RehearsalSign;
class Score;

class EditRehearsalSign : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    EditRehearsalSign(Score* score, RehearsalSign& rehearsalSign, bool isShown,
                      const std::string& description = "");

    virtual void undo();
    virtual void redo();

signals:
    void triggered();

private:
    Score* score;
    RehearsalSign& rehearsalSign;
    bool isShown;
    std::string description;

    void showHide(bool show);
};

#endif // EDITREHEARSALSIGN_H
