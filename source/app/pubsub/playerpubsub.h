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

#ifndef APP_PLAYERPUBSUB_H
#define APP_PLAYERPUBSUB_H

#include <app/pubsub/pubsub.h>

class Player;

/// Provides a way to subscribe to or publish notifications about changes
/// to a particular player.
class PlayerEditPubSub : public PubSub<void (int, const Player &, bool)>
{
};

/// Notifications about a player being removed.
class PlayerRemovePubSub : public PubSub<void (int)>
{
};

#endif
