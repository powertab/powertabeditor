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
                const std::optional<int> &selection,
                const std::vector<FilterRule> &rules,
                const std::vector<std::string> &matches) override
    {
        myFilterNames = names;
        mySelection = selection;
        myRules = rules;
        myMatches = matches;
    }

    ViewFilterPresenter *myPresenter;
    std::vector<std::string> myFilterNames;
    std::optional<int> mySelection;
    std::vector<FilterRule> myRules;
    std::vector<std::string> myMatches;
};

TEST_CASE("Dialogs/ViewFilter")
{
    Score score;

    Player guitar;
    guitar.setDescription("Guitar");
    score.insertPlayer(guitar);

    Player bass;
    bass.setDescription("Bass");
    Tuning tuning;
    tuning.setNotes({ 1, 2, 3, 4 });
    bass.setTuning(tuning);
    score.insertPlayer(bass);

    ViewFilter filter_all;
    filter_all.setDescription("Filter 1");
    score.insertViewFilter(filter_all);

    MockViewFilterView view;
    ViewFilterPresenter presenter(view, score);

    SUBCASE("Init")
    {
        REQUIRE(view.myPresenter != nullptr);
        REQUIRE(view.myFilterNames.size() == 1);
        REQUIRE(view.mySelection);
        REQUIRE(view.myMatches ==
                std::vector{ guitar.getDescription(), bass.getDescription() });
    }

    SUBCASE("Add Filter")
    {
        presenter.addFilter();
        REQUIRE(view.myFilterNames ==
                std::vector<std::string>({ "Filter 1", "Untitled" }));
        REQUIRE(*view.mySelection == 1);
    }

    SUBCASE("Remove Filter")
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

    SUBCASE("Select Filter")
    {
        presenter.addFilter();
        presenter.selectFilter(0);
        REQUIRE(*view.mySelection == 0);
    }

    SUBCASE("Edit Filter Description")
    {
        presenter.addFilter();
        presenter.editFilterDescription("New Filter Name");
        REQUIRE(view.myFilterNames ==
                std::vector<std::string>({ "Filter 1", "New Filter Name" }));
    }

    SUBCASE("Add Rule")
    {
        REQUIRE(view.myRules.empty());
        presenter.addRule();
        REQUIRE(view.myRules.size() == 1);
        REQUIRE(view.myRules[0] == FilterRule());
        REQUIRE(view.myMatches.empty());
    }

    SUBCASE("Remove Rule")
    {
        presenter.addRule();
        presenter.addRule();
        presenter.addRule();
        presenter.removeRule(1);
        REQUIRE(view.myRules.size() == 2);
    }

    SUBCASE("Edit Rule")
    {
        presenter.addRule();

        FilterRule new_rule(FilterRule::NUM_STRINGS, FilterRule::EQUAL, 4);
        presenter.editRule(0, new_rule);
        REQUIRE(view.myRules[0] == new_rule);
        REQUIRE(view.myMatches == std::vector{ bass.getDescription() });
    }
}
