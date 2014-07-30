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

#ifndef SCORE_PLAYER_H
#define SCORE_PLAYER_H

#include <cstdint>
#include "fileversion.h"
#include <string>
#include "tuning.h"

class Player
{
public:
    Player();

    bool operator==(const Player &other) const;

    template <class Archive>
    void serialize(Archive &ar, const FileVersion version);

    /// Returns a description of the player (e.g. "Rhythm Guitar 1").
    const std::string &getDescription() const;
    /// Sets the description of the player.
    void setDescription(const std::string &description);

    /// Returns the maximum volume for the player's instruments.
    uint8_t getMaxVolume() const;
    /// Sets the maximum volume for the player's instruments.
    void setMaxVolume(uint8_t volume);

    /// Returns the pan value for the player's instruments.
    uint8_t getPan() const;
    /// Sets the pan value for the player's instruments.
    void setPan(uint8_t pan);

    /// Returns the player's tuning.
    const Tuning &getTuning() const;
    /// Sets the player's tuning.
    void setTuning(const Tuning &tuning);

    static const uint8_t MIN_VOLUME;
    static const uint8_t MAX_VOLUME;
    static const uint8_t MIN_PAN;
    static const uint8_t MAX_PAN;

private:
    std::string myDescription;
    uint8_t myMaxVolume;
    uint8_t myPan;
    Tuning myTuning;
};

template <class Archive>
void Player::serialize(Archive &ar, const FileVersion /*version*/)
{
    ar("description", myDescription);
    ar("max_volume", myMaxVolume);
    ar("pan", myPan);
    ar("tuning", myTuning);
}

#endif
