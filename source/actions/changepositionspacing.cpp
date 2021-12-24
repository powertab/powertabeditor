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
  
#include "changepositionspacing.h"

#include <powertabdocument/system.h>

using boost::shared_ptr;

ChangePositionSpacing::ChangePositionSpacing(shared_ptr<System> system,
                                             uint8_t spacing) :
    system(system),
    newSpacing(spacing),
    originalSpacing(system->GetPositionSpacing())
{
    if (newSpacing < originalSpacing)
    {
        setText(tr("Decrease Position Spacing"));
    }
    else
    {
        setText(tr("Increase Position Spacing"));
    }
}

void ChangePositionSpacing::redo()
{
    system->SetPositionSpacing(newSpacing);
}

void ChangePositionSpacing::undo()
{
    system->SetPositionSpacing(originalSpacing);
}
