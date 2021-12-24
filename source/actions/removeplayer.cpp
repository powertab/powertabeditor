/*
  * Copyright (C) 2014 Cameron White
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
  
#include "removeplayer.h"

#include <score/score.h>

RemovePlayer::RemovePlayer(Score &score, int index)
    : QUndoCommand(tr("Remove Player")),
      myScore(score),
      myPlayer(score.getPlayers()[index]),
      myPlayerIndex(index)
{
}

void RemovePlayer::redo()
{
    myScore.removePlayer(myPlayerIndex);

    // Remove the player from any player changes that it was involved in.
    myOriginalChanges.clear();
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
                    if (activePlayer.getPlayerNumber() >= myPlayerIndex)
                    {
                        change.removeActivePlayer(i, activePlayer);

                        // Shift player numbers.
                        if (activePlayer.getPlayerNumber() > myPlayerIndex)
                        {
                            change.insertActivePlayer(
                                i, ActivePlayer(
                                       activePlayer.getPlayerNumber() - 1,
                                       activePlayer.getInstrumentNumber()));
                        }
                    }
                }
            }
        }
    }
}

void RemovePlayer::undo()
{
    myScore.insertPlayer(myPlayer, myPlayerIndex);

    // Restore the original player changes.
    int i = 0;
    for (System &system : myScore.getSystems())
    {
        for (PlayerChange &change : system.getPlayerChanges())
        {
            change = myOriginalChanges[i];
            ++i;
        }
    }
}
