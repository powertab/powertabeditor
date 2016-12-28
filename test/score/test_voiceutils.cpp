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
  
#include <catch.hpp>

#include <score/voiceutils.h>
#include <score/voice.h>

TEST_CASE("Score/VoiceUtils/GetDurationTime", "")
{
    Voice voice;
    voice.insertPosition(Position(7));
    Position &position = voice.getPositions().front();

    position.setDurationType(Position::QuarterNote);
    REQUIRE(VoiceUtils::getDurationTime(voice, position) == 1);

    position.setDurationType(Position::EighthNote);
    REQUIRE(VoiceUtils::getDurationTime(voice, position) ==
            boost::rational<int>(1, 2));

    position.setDurationType(Position::WholeNote);
    REQUIRE(VoiceUtils::getDurationTime(voice, position) == 4);

    position.setProperty(Position::Dotted);
    REQUIRE(VoiceUtils::getDurationTime(voice, position) == 6);

    voice.insertIrregularGrouping(IrregularGrouping(7, 1, 3, 2));
    REQUIRE(VoiceUtils::getDurationTime(voice, position) == 4);
}
