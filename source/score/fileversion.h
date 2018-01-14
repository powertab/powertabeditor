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

#ifndef SCORE_FILEVERSION_H
#define SCORE_FILEVERSION_H

#include <cstdint>

enum class FileVersion : int {
    INITIAL_VERSION = 1, ///< Initial version from the beginning of development.
    TEXT_ITEMS = 2, ///< Added floating text items.
    VIEW_FILTERS = 3, ///< Removed the Staff::myViewType member and added view filters.
    LEFT_HAND_FINGERING = 4, ///< Added left hand fingering numbers to notes.
    LATEST_VERSION = LEFT_HAND_FINGERING
};

#endif
