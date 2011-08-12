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
  
#ifndef GP_CHANNEL_H
#define GP_CHANNEL_H

#include <cstdint>

namespace Gp {

class InputStream;

/// Holds information about a Midi Channel in a Guitar Pro document
/// - Used as a temporary structure while importing files, and handles the formatting conversions
class Channel
{
public:
    Channel();

    static uint8_t readChannelProperty(Gp::InputStream& stream);

    uint32_t instrument;
    uint8_t volume;
    uint8_t balance;
    uint8_t chorus;
    uint8_t reverb;
    uint8_t phaser;
    uint8_t tremolo;
};

}

#endif // GP_CHANNEL_H
