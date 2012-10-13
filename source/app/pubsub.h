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

#ifndef PUBSUB_H
#define PUBSUB_H

#include <boost/signals2/signal.hpp>

/// A thin wrapper around boost::signal.
/// Provides a loosely-coupled way of sending and receiving messages (anyone
/// can send a message without explicitly connecting to each subscriber).
template<typename Signature>
class PubSub
{
public:
    typedef boost::signals2::signal<Signature> message_t;

    /// Subscribe to any messages.
    boost::signals2::connection subscribe(
            const typename message_t::slot_type& subscriber)
    {
        return publish.connect(subscriber);
    }

    /// Call PubSub.publish(args) to publish a message.
    message_t publish;
};

#endif // PUBSUB_H
