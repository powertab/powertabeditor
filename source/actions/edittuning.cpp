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
  
#include "edittuning.h"

#include <powertabdocument/guitar.h>

EditTuning::EditTuning(boost::shared_ptr<Guitar> guitar, const Tuning& newTuning) :
    guitar(guitar),
    newTuning(newTuning),
    oldTuning(guitar->GetTuning())
{
    setText(QObject::tr("Edit Tuning"));
}

void EditTuning::redo()
{
    guitar->SetTuning(newTuning);
}

void EditTuning::undo()
{
    guitar->SetTuning(oldTuning);
}
