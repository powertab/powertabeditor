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

#include <actions/editviewfilters.h>
#include <score/score.h>

#include <ranges>

TEST_CASE("Actions/EditViewFilters")
{
    Score score;

    ViewFilter filter1;
    filter1.addRule(FilterRule(FilterRule::Subject::NumStrings,
                               FilterRule::Operation::Equal, 7));
    ViewFilter filter2;
    filter2.addRule(FilterRule(FilterRule::Subject::NumStrings,
                               FilterRule::Operation::LessThanEqual, 5));

    score.insertViewFilter(filter2);

    std::vector<ViewFilter> old_filters = {filter2};
    std::vector<ViewFilter> new_filters = {filter1, filter2};
    EditViewFilters action(score, new_filters);

    action.redo();
    REQUIRE(std::ranges::equal(score.getViewFilters(), new_filters));

    action.undo();
    REQUIRE(std::ranges::equal(score.getViewFilters(), old_filters));
}
