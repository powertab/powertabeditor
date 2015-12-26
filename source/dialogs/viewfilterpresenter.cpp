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

#include "viewfilterpresenter.h"

#include <score/score.h>

ViewFilterPresenter::ViewFilterPresenter(ViewFilterView &view,
                                         const Score &score)
    : myView(view),
      myScore(score),
      myFilters(score.getViewFilters().begin(), score.getViewFilters().end())
{
    myView.setPresenter(this);

    if (!score.getViewFilters().empty())
        mySelection = 0;

    updateView();
}

void ViewFilterPresenter::addFilter()
{
    ViewFilter filter;
    filter.setDescription("Untitled");

    myFilters.push_back(filter);
    mySelection = myFilters.size() - 1;

    updateView();
}

void ViewFilterPresenter::removeSelectedFilter()
{
    assert(mySelection);

    myFilters.erase(myFilters.begin() + *mySelection);
    if (myFilters.empty())
        mySelection.reset();
    else if (*mySelection == static_cast<int>(myFilters.size()))
        *mySelection = myFilters.size() - 1;

    updateView();
}

void ViewFilterPresenter::selectFilter(int index)
{
    assert(index >= 0);
    assert(index < static_cast<int>(myFilters.size()));

    if (mySelection != index)
    {
        mySelection = index;
        updateView();
    }
}

void ViewFilterPresenter::updateView()
{
    std::vector<std::string> filter_names;
    for (auto &&filter : myFilters)
        filter_names.push_back(filter.getDescription());

    myView.setFilterNames(filter_names, mySelection);
}
