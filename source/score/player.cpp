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

#include "player.h"

#include <stdexcept>
#include "generalmidi.h"

const uint8_t Player::MIN_VOLUME = Midi::MIN_MIDI_CHANNEL_VOLUME;
const uint8_t Player::MAX_VOLUME = Midi::MAX_MIDI_CHANNEL_VOLUME;
const uint8_t Player::MIN_PAN = Midi::MIN_MIDI_CHANNEL_EFFECT_LEVEL;
const uint8_t Player::MAX_PAN = Midi::MAX_MIDI_CHANNEL_EFFECT_LEVEL;

Player::Player() : myDescription("Untitled"), myMaxVolume(MAX_VOLUME), myPan(64)
{
}

bool Player::operator==(const Player &other) const
{
    return myDescription == other.myDescription &&
           myMaxVolume == other.myMaxVolume && myPan == other.myPan &&
           myTuning == other.myTuning;
}

const std::string &Player::getDescription() const
{
    return myDescription;
}

void Player::setDescription(const std::string &description)
{
    myDescription = description;
}

uint8_t Player::getMaxVolume() const
{
    return myMaxVolume;
}

void Player::setMaxVolume(uint8_t volume)
{
    if (volume > MAX_VOLUME)
        throw std::out_of_range("Invalid volume");

    myMaxVolume = volume;
}

uint8_t Player::getPan() const
{
    return myPan;
}

void Player::setPan(uint8_t pan)
{
    if (pan > MAX_PAN)
        throw std::out_of_range("Invalid pan");

    myPan = pan;
}

const Tuning &Player::getTuning() const
{
    return myTuning;
}

void Player::setTuning(const Tuning &tuning)
{
    myTuning = tuning;
}

