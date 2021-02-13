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

ViewOptions::ViewOptions() : myZoom(100.0)
{
}

bool ViewOptions::setZoom(double percent)
{
    myZoom = std::clamp(percent, MIN_ZOOM, MAX_ZOOM);
    return myZoom == percent;
}

bool ViewOptions::increaseZoom()
{
    short nextZoom = *std::upper_bound(ZOOM_LEVELS.begin(), ZOOM_LEVELS.end() - 1, getZoom());

    return setZoom(nextZoom);
}

bool ViewOptions::decreaseZoom()
{
    short prevZoom = *std::upper_bound(ZOOM_LEVELS.rbegin(), ZOOM_LEVELS.rend() - 1, getZoom(), std::greater<short>());

    return setZoom(prevZoom);
}
