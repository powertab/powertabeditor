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

bool ViewFilterPresenter::exec()
{
    return myView.launch();
}

void ViewFilterPresenter::addFilter()
{
    ViewFilter filter;
    filter.setDescription("Untitled");
    // Add a rule by default so that the user sees something.
    filter.addRule(FilterRule());

    myFilters.push_back(filter);
    mySelection = static_cast<int>(myFilters.size()) - 1;

    updateView();
}

void ViewFilterPresenter::removeSelectedFilter()
{
    assert(mySelection);

    myFilters.erase(myFilters.begin() + *mySelection);
    if (myFilters.empty())
        mySelection.reset();
    else if (*mySelection == static_cast<int>(myFilters.size()))
        *mySelection = static_cast<int>(myFilters.size()) - 1;

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

void ViewFilterPresenter::editFilterDescription(const std::string &description)
{
    myFilters[*mySelection].setDescription(description);
    updateView();
}

void ViewFilterPresenter::addRule()
{
    myFilters[*mySelection].addRule(FilterRule());
    updateView();
}

void ViewFilterPresenter::removeRule(int index)
{
    myFilters[*mySelection].removeRule(index);
    updateView();
}

void ViewFilterPresenter::editRule(int index, const FilterRule &rule)
{
    myFilters[*mySelection].getRules()[index] = rule;
    updateView();
}

void ViewFilterPresenter::updateView()
{
    std::vector<std::string> filter_names;
    for (auto &&filter : myFilters)
        filter_names.push_back(filter.getDescription());

    std::vector<FilterRule> rules;
    if (mySelection)
    {
        auto &filter = myFilters[*mySelection];
        rules.insert(rules.begin(), filter.getRules().begin(),
                     filter.getRules().end());
    }

    // Build a list of the players that match the selected filter.
    std::vector<std::string> matches;
    if (mySelection)
    {
        auto &filter = myFilters[*mySelection];
        for (const Player &player : myScore.getPlayers())
        {
            if (filter.accept(player))
                matches.push_back(player.getDescription());
        }
    }

    myView.update(filter_names, mySelection, rules, matches);
}
