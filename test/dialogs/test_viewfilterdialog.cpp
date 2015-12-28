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

    bool launch() override { return false; }

    void update(const std::vector<std::string> &names,
                const boost::optional<int> &selection,
                const std::vector<FilterRule> &rules) override
    {
        myFilterNames = names;
        mySelection = selection;
        myRules = rules;
    }

    ViewFilterPresenter *myPresenter;
    std::vector<std::string> myFilterNames;
    boost::optional<int> mySelection;
    std::vector<FilterRule> myRules;
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

    SECTION("Edit Filter Description")
    {
        presenter.addFilter();
        presenter.editFilterDescription("New Filter Name");
        REQUIRE(view.myFilterNames ==
                std::vector<std::string>({ "Filter 1", "New Filter Name" }));
    }

    SECTION("Add Rule")
    {
        REQUIRE(view.myRules.empty());
        presenter.addRule();
        REQUIRE(view.myRules.size() == 1);
        REQUIRE(view.myRules[0] == FilterRule());
    }

    SECTION("Remove Rule")
    {
        presenter.addRule();
        presenter.addRule();
        presenter.addRule();
        presenter.removeRule(1);
        REQUIRE(view.myRules.size() == 2);
    }

    SECTION("Edit Rule")
    {
        presenter.addRule();

        FilterRule new_rule(FilterRule::NUM_STRINGS, FilterRule::EQUAL, 4);
        presenter.editRule(0, new_rule);
        REQUIRE(view.myRules[0] == new_rule);
    }
}
