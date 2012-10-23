/*
  * Copyright (C) 2012 Cameron White
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

#ifndef REMOVETEMPOMARKER_H
#define REMOVETEMPOMARKER_H

#include <QUndoCommand>
#include <boost/shared_ptr.hpp>

class Score;
class TempoMarker;

class RemoveTempoMarker : public QUndoCommand
{
public:
    RemoveTempoMarker(Score* score, boost::shared_ptr<TempoMarker> marker);

    virtual void redo();
    virtual void undo();

private:
    Score* score;
    boost::shared_ptr<TempoMarker> marker;
};

#endif // REMOVETEMPOMARKER_H
