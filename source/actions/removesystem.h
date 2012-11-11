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
  
#ifndef REMOVESYSTEM_H
#define REMOVESYSTEM_H

#include <QUndoCommand>

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

class AlternateEnding;
class Dynamic;
class Score;
class System;
class TempoMarker;

class RemoveSystem : public QUndoCommand
{
public:
    RemoveSystem(Score* score, uint32_t index);
    void redo();
    void undo();

protected:
    Score* score;
    const uint32_t index;
    boost::shared_ptr<System> system;

    std::vector<boost::shared_ptr<TempoMarker> > tempoMarkers;
    std::vector<boost::shared_ptr<Dynamic> > dynamics;
    std::vector<boost::shared_ptr<AlternateEnding> > altEndings;
};

#endif // REMOVESYSTEM_H
