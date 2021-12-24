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

#include "editplayer.h"

#include <score/score.h>

EditPlayer::EditPlayer(Score &score, int playerIndex, const Player &player)
    : QUndoCommand(tr("Edit Player")),
      myScore(score),
      myPlayerIndex(playerIndex),
      myNewPlayer(player),
      myOriginalPlayer(score.getPlayers()[playerIndex])
{
}

void EditPlayer::redo()
{
    if (myScore.getPlayers()[myPlayerIndex].getTuning().getStringCount() !=
        myNewPlayer.getTuning().getStringCount())
    {
        // If the number of strings changed, remove the player from any staves
        // it was assigned to.
        for (System &system : myScore.getSystems())
        {
            for (PlayerChange &change : system.getPlayerChanges())
            {
                myOriginalChanges.push_back(change);

                for (unsigned int i = 0; i < system.getStaves().size(); ++i)
                {
                    for (const ActivePlayer &activePlayer :
                         change.getActivePlayers(i))
                    {
                        if (activePlayer.getPlayerNumber() == myPlayerIndex)
                            change.removeActivePlayer(i, activePlayer);
                    }
                }
            }
        }
    }

    myScore.getPlayers()[myPlayerIndex] = myNewPlayer;
}

void EditPlayer::undo()
{
    myScore.getPlayers()[myPlayerIndex] = myOriginalPlayer;

    // Restore the original player changes.
    if (!myOriginalChanges.empty())
    {
        int i = 0;
        for (System &system : myScore.getSystems())
        {
            for (PlayerChange &change : system.getPlayerChanges())
            {
                change = myOriginalChanges[i];
                ++i;
            }
        }

        myOriginalChanges.clear();
    }
}
