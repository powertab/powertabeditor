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
  
#include "addguitar.h"

#include <boost/make_shared.hpp>

#include <powertabdocument/score.h>
#include <powertabdocument/guitar.h>
#include <widgets/mixer/mixer.h>

AddGuitar::AddGuitar(Score* score, Mixer* mixer) :
    score(score),
    mixer(mixer)
{
    newGuitar = boost::make_shared<Guitar>();
    newGuitar->GetTuning().SetToStandard();

    setText(QObject::tr("Add Guitar"));
}

void AddGuitar::redo()
{
    score->InsertGuitar(newGuitar);
    mixer->addInstrument(newGuitar);

    emit triggered();
}

void AddGuitar::undo()
{
    const size_t index = score->GetGuitarCount() - 1;

    score->RemoveGuitar(index);
    mixer->removeInstrument(index);

    emit triggered();
}
