/*
  * Copyright (C) 2015 Cameron White
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
  
#include "editviewfilters.h"

#include <score/score.h>

EditViewFilters::EditViewFilters(Score &score,
                                 std::vector<ViewFilter> new_filters)
    : QUndoCommand(tr("Edit View Filters")),
      myScore(score),
      myOriginalFilters(score.getViewFilters().begin(),
                        score.getViewFilters().end()),
      myNewFilters(std::move(new_filters))
{
}

void EditViewFilters::redo()
{
    setViewFilters(myNewFilters);
}

void EditViewFilters::undo()
{
    setViewFilters(myOriginalFilters);
}

void EditViewFilters::setViewFilters(const std::vector<ViewFilter> &filters)
{
    while (!myScore.getViewFilters().empty())
    {
        myScore.removeViewFilter(
            static_cast<int>(myScore.getViewFilters().size()) - 1);
    }    

    for (const ViewFilter &filter : filters)
        myScore.insertViewFilter(filter);
}
