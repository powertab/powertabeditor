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
  
#include "deletebarline.h"

#include <powertabdocument/system.h>
#include <powertabdocument/barline.h>

DeleteBarline::DeleteBarline(std::shared_ptr<System> system, std::shared_ptr<Barline> barline) :
    system(system),
    barline(barline)
{
    setText(QObject::tr("Delete Barline"));
}

void DeleteBarline::redo()
{
    system->RemoveBarline(barline->GetPosition());
}

void DeleteBarline::undo()
{
    system->InsertBarline(barline);
}
