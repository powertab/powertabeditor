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

#include "viewoptions.h"

#include <algorithm>
#include <functional> /* std::greater */
#include <score/score.h>

ViewOptions::ViewOptions() : myZoom(100)
{
}

void
ViewOptions::ensureValid(const Score &score)
{
    if (mySelectedFilter)
    {
        if (score.getViewFilters().empty())
            mySelectedFilter = std::nullopt;
        else if (*mySelectedFilter >=
                 static_cast<int>(score.getViewFilters().size()))
        {
            mySelectedFilter = 0;
        }
    }
    else if (myPlayerFilterIndex)
    {
        if (*myPlayerFilterIndex >= static_cast<int>(score.getPlayers().size()))
            myPlayerFilterIndex = std::nullopt;
        else
        {
            // Update for any changes to the player's name.
            setPlayerFilter(score, *myPlayerFilterIndex);
        }
    }
}

const ViewFilter *
ViewOptions::getFilter(const Score &score) const
{
    if (mySelectedFilter.has_value())
        return &score.getViewFilters()[*mySelectedFilter];
    else if (myPlayerFilterIndex.has_value())
        return &myPlayerFilter;
    else
        return nullptr;
}

void
ViewOptions::setSelectedFilter(int filter)
{
    mySelectedFilter = filter;
    myPlayerFilterIndex = std::nullopt;
}

void
ViewOptions::setPlayerFilter(const Score &score, int player_idx)
{
    mySelectedFilter = std::nullopt;

    const Player &player = score.getPlayers()[player_idx];
    myPlayerFilterIndex = player_idx;
    myPlayerFilter = ViewFilter();
    myPlayerFilter.addRule(
        FilterRule(FilterRule::Subject::PlayerName, player.getDescription()));
}

bool ViewOptions::setZoom(int percent)
{
    myZoom = std::clamp(percent, MIN_ZOOM, MAX_ZOOM);
    return myZoom == percent;
}

bool ViewOptions::increaseZoom()
{
    int nextZoom = *std::upper_bound(ZOOM_LEVELS.begin(), ZOOM_LEVELS.end() - 1, getZoom());
    return setZoom(nextZoom);
}

bool ViewOptions::decreaseZoom()
{
    int prevZoom = *std::upper_bound(ZOOM_LEVELS.rbegin(), ZOOM_LEVELS.rend() - 1, getZoom(), std::greater<int>());
    return setZoom(prevZoom);
}
