/*
  * Copyright (C) 2011 Cameron White
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
  
#ifndef IRREGULARNOTEGROUP_H
#define IRREGULARNOTEGROUP_H

#include "notestem.h"

#include <vector>
#include <cstdint>

class QGraphicsItem;

class IrregularNoteGroup
{
public:
    IrregularNoteGroup(const std::vector<NoteStem>& noteStems);

    void draw(QGraphicsItem* parent);

    static const uint8_t GROUP_HEIGHT = 6;
    static const uint8_t GROUP_SPACING = 3;

private:
    std::vector<NoteStem> noteStems;
};

#endif // IRREGULARNOTEGROUP_H
