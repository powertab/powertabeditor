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

#ifndef DIALOGS_VIEWFILTERPRESENTER_H
#define DIALOGS_VIEWFILTERPRESENTER_H

#include <optional>
#include <score/viewfilter.h>
#include <string>
#include <vector>

class Score;
class ViewFilterPresenter;

class ViewFilterView
{
public:
    virtual ~ViewFilterView()
    {
    }

    virtual void setPresenter(ViewFilterPresenter *presenter) = 0;

    virtual bool launch() = 0;

    virtual void update(const std::vector<std::string> &names,
                        const std::optional<int> &selection,
                        const std::vector<FilterRule> &rules,
                        const std::vector<std::string> &matches) = 0;
};

class ViewFilterPresenter
{
public:
    ViewFilterPresenter(ViewFilterView &view, const Score &score);

    bool exec();

    const std::vector<ViewFilter> getFilters() const { return myFilters; }

    void addFilter();
    void removeSelectedFilter();
    void selectFilter(int index);
    void editFilterDescription(const std::string &description);

    void addRule();
    void removeRule(int index);
    void editRule(int index, const FilterRule &rule);

private:
    void updateView();

    ViewFilterView &myView;
    const Score &myScore;
    std::vector<ViewFilter> myFilters;
    std::optional<int> mySelection;
};

#endif
