/*
  * Copyright (C) 2015 Cameron White
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
  
#include <doctest/doctest.h>

#include <app/appinfo.h>
#include <formats/powertab/powertabimporter.h>
#include <score/score.h>
#include <score/viewfilter.h>
#include "test_serialization.h"

TEST_CASE("Score/ViewFilter/FilterRule")
{
    Score score;

    PowerTabImporter importer;
    importer.load(AppInfo::getAbsolutePath("data/test_viewfilter.pt2"), score);

    FilterRule rule(FilterRule::Subject::NumStrings,
                    FilterRule::Operation::Equal, 7);
    REQUIRE(!rule.accept(score.getPlayers()[0]));
    REQUIRE(rule.accept(score.getPlayers()[1]));
    REQUIRE(!rule.accept(score.getPlayers()[2]));

    rule = FilterRule(FilterRule::Subject::PlayerName, "Player [12]");
    REQUIRE(rule.accept(score.getPlayers()[0]));
    REQUIRE(rule.accept(score.getPlayers()[1]));
    REQUIRE(!rule.accept(score.getPlayers()[2]));
}

TEST_CASE("Score/ViewFilter/ViewFilter")
{
    Score score;

    PowerTabImporter importer;
    importer.load(AppInfo::getAbsolutePath("data/test_viewfilter.pt2"), score);

    ViewFilter filter;
    filter.addRule(FilterRule(FilterRule::Subject::NumStrings,
                              FilterRule::Operation::Equal, 7));
    filter.addRule(FilterRule(FilterRule::Subject::NumStrings,
                              FilterRule::Operation::LessThanEqual, 5));

    REQUIRE(!filter.accept(score, 0, 0));
    REQUIRE(filter.accept(score, 0, 1));
    REQUIRE(filter.accept(score, 0, 2));
}

TEST_CASE("Score/ViewFilter/Serialization")
{
    ViewFilter filter;
    filter.addRule(FilterRule(FilterRule::Subject::NumStrings,
                              FilterRule::Operation::LessThan, 5));
    filter.addRule(FilterRule(FilterRule::Subject::PlayerName, "foo"));

    Serialization::test("filter", filter);
}
