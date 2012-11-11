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
  
#include "removesystem.h"

#include <boost/foreach.hpp>

#include <powertabdocument/score.h>

RemoveSystem::RemoveSystem(Score* score, uint32_t index) :
    QUndoCommand(QObject::tr("Remove System")),
    score(score),
    index(index),
    system(score->GetSystem(index))
{
    score->GetTempoMarkersInSystem(tempoMarkers, system);
    score->GetDynamicsInSystem(dynamics, system);
    score->GetAlternateEndingsInSystem(altEndings, system);
}

void RemoveSystem::redo()
{
    score->RemoveSystem(index);
}

void RemoveSystem::undo()
{
    score->InsertSystem(system, index);

    BOOST_FOREACH(boost::shared_ptr<TempoMarker> tempoMarker, tempoMarkers)
    {
        score->InsertTempoMarker(tempoMarker);
    }
    BOOST_FOREACH(boost::shared_ptr<Dynamic> dynamic, dynamics)
    {
        score->InsertDynamic(dynamic);
    }
    BOOST_FOREACH(boost::shared_ptr<AlternateEnding> ending, altEndings)
    {
        score->InsertAlternateEnding(ending);
    }
}
