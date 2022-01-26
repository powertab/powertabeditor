/*
  * Copyright (C) 2020 Cameron White
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

#ifndef APP_SCORECLICKEVENT_H
#define APP_SCORECLICKEVENT_H

#include <boost/signals2/signal.hpp>

class ConstScoreLocation;

/// Item types that can be clicked.
enum class ScoreItem
{
    AlterationOfPace,
    AlternateEnding,
    Barline,
    Bend,
    ChordDiagram,
    ChordText,
    Clef,
    Direction,
    Dynamic,
    KeySignature,
    MultiBarRest,
    PlayerChange,
    RehearsalSign,
    ScoreInfo,
    Staff,
    TempoMarker,
    TextItem,
    TimeSignature,
    TremoloBar,
    VolumeSwell,
};

enum class ScoreItemAction
{
    Selected,
    DoubleClicked
};

/// QGraphicsItem doesn't use QObject signals / slots, so just use a simple
/// signal / slot for mouse event handlers.
class ScoreClickEvent
{
public:
    using MessageType = boost::signals2::signal<void(
        ScoreItem, const ConstScoreLocation &, ScoreItemAction)>;

    /// Connect an event listener.
    boost::signals2::connection connect(
            const typename MessageType::slot_type& slot)
    {
        return signal.connect(slot);
    }

    /// Call signal(args) to invoke the slots.
    MessageType signal;
};

#endif
