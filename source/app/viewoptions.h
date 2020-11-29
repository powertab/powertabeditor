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
  
#ifndef APP_VIEWOPTIONS_H
#define APP_VIEWOPTIONS_H

#include <optional>
#include <algorithm>

/// Stores any view options that are not saved with the score (e.g. the current
/// zoom level or the active score filter).
class ViewOptions
{
public:
    ViewOptions();

    const std::optional<int> &getFilter() const { return myFilter; }
    void setFilter(int filter) { myFilter = filter; }
    void clearFilter() { myFilter.reset(); }

    double getZoom() const { return myZoom; }
    void setZoom(double percent);

private:
    std::optional<int> myFilter;
    double myZoom;
};

#endif
