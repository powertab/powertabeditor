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

#include <catch.hpp>

#include <dialogs/viewfilterpresenter.h>
#include <score/score.h>

class MockViewFilterView : public ViewFilterView
{
public:
    MockViewFilterView() : myPresenter(nullptr)
    {
    }

    void setPresenter(ViewFilterPresenter *presenter) override
    {
        myPresenter = presenter;
    }

    void setFilterNames(const std::vector<std::string> &names,
                        const boost::optional<int> &selection) override
    {
        myFilterNames = names;
        mySelection = selection;
    }

    ViewFilterPresenter *myPresenter;
    std::vector<std::string> myFilterNames;
    boost::optional<int> mySelection;
};

TEST_CASE("Dialogs/ViewFilter")
{
    Score score;
    ViewFilter filter_all;
    filter_all.setDescription("Filter 1");
    score.insertViewFilter(filter_all);

    MockViewFilterView view;
    ViewFilterPresenter presenter(view, score);

    SECTION("Init")
    {
        REQUIRE(view.myPresenter != nullptr);
        REQUIRE(view.myFilterNames.size() == 1);
        REQUIRE(view.mySelection);
    }

    SECTION("Add Filter")
    {
        presenter.addFilter();
        REQUIRE(view.myFilterNames ==
                std::vector<std::string>({ "Filter 1", "Untitled" }));
        REQUIRE(*view.mySelection == 1);
    }

    SECTION("Remove Filter")
    {
        presenter.addFilter();
        presenter.addFilter();
        presenter.selectFilter(2);
        presenter.removeSelectedFilter();

        REQUIRE(view.myFilterNames.size() == 2);
        REQUIRE(view.myFilterNames ==
                std::vector<std::string>({ "Filter 1", "Untitled" }));
        REQUIRE(*view.mySelection == 1);

        presenter.removeSelectedFilter();
        presenter.removeSelectedFilter();
        REQUIRE(view.myFilterNames.empty());
        REQUIRE(!view.mySelection);
    }

    SECTION("Select Filter")
    {
        presenter.addFilter();
        presenter.selectFilter(0);
        REQUIRE(*view.mySelection == 0);
    }
}
