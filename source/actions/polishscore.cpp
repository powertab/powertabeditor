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
  
#include "polishscore.h"

#include <score/score.h>
#include <score/utils/scorepolisher.h>

PolishScore::PolishScore(Score &score)
    : QUndoCommand(tr("Polish Score")), myScore(score)
{
}

void PolishScore::redo()
{
    for (const System &system : myScore.getSystems())
        myOriginalSystems.push_back(system);

    ScoreUtils::polishScore(myScore);
}

void PolishScore::undo()
{
    std::copy(myOriginalSystems.begin(), myOriginalSystems.end(),
              myScore.getSystems().begin());

    myOriginalSystems.clear();
}
