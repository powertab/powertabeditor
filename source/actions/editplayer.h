/*
  * Copyright (C) 2013 Cameron White
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

#ifndef ACTIONS_EDITPLAYER_H
#define ACTIONS_EDITPLAYER_H

#include <QCoreApplication>
#include <QUndoCommand>
#include <score/player.h>
#include <score/playerchange.h>
#include <vector>

class Score;

class EditPlayer : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(EditPlayer)

public:
    EditPlayer(Score &score, int playerIndex, const Player &player);

    virtual void redo() override;
    virtual void undo() override;

private:
    Score &myScore;
    const int myPlayerIndex;
    const Player myNewPlayer;
    const Player myOriginalPlayer;
    std::vector<PlayerChange> myOriginalChanges;
};

#endif
