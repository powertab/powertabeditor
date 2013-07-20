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
  
#include "gp_channel.h"

#include "inputstream.h"

#include <powertabdocument/guitar.h>

Gp::Channel::Channel() :
    instrument(Guitar::DEFAULT_PRESET),
    volume(Guitar::DEFAULT_INITIAL_VOLUME),
    balance(Guitar::DEFAULT_PAN),
    chorus(Guitar::DEFAULT_CHORUS),
    reverb(Guitar::DEFAULT_REVERB),
    phaser(Guitar::DEFAULT_PHASER),
    tremolo(Guitar::DEFAULT_TREMOLO)
{
}

/// For some reason, channel properties (except instrument type) are stored in a different format,
/// where 1 -> 7, 2 -> 15, ... , and 16 -> 127.
/// This function reads the data and performs the necessary conversion
uint8_t Gp::Channel::readChannelProperty(Gp::InputStream& stream)
{
    uint8_t value = stream.read<uint8_t>();

    if (value != 0)
    {
        value = (value * 8) - 1;
    }

    return value;
}
