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

#include <score/scoreinfo.h>
#include "test_serialization.h"

using namespace Score; 

TEST_CASE("Score/ScoreInfo/ChangeType", "")
{
    ScoreInfo info;

    REQUIRE(info.getScoreType() == ScoreInfo::Song);
    REQUIRE_NOTHROW(info.getSongData());
    REQUIRE_THROWS(info.getLessonData());

    info.setLessonData(LessonData());
    REQUIRE(info.getScoreType() == ScoreInfo::Lesson);
    REQUIRE_THROWS(info.getSongData());
    REQUIRE_NOTHROW(info.getLessonData());
}

TEST_CASE("Score/ScoreInfo/Serialization", "")
{
    ScoreInfo info;

    Serialization::test(info);
}
