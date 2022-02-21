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

#include "viewfilter.h"

#include <score/score.h>
#include <regex>
#include <ostream>
#include <util/enumtostring.h>

struct FilterRule::RegexImpl
{
    RegexImpl(const std::string &s) : myRegex(s)
    {
    }

    std::regex myRegex;
};

FilterRule::FilterRule()
    : mySubject(Subject::PlayerName),
      myOperation(Operation::Equal),
      myIntValue(0)
{
}

FilterRule::FilterRule(Subject subject, std::string value)
    : mySubject(subject),
      myOperation(Operation::Equal),
      myIntValue(0),
      myStrValue(std::move(value)),
      myRegexImpl(std::make_unique<RegexImpl>(myStrValue))
{
}

FilterRule::FilterRule(Subject subject, Operation op, int value)
    : mySubject(subject), myOperation(op), myIntValue(value)
{
}

FilterRule::~FilterRule() = default;

FilterRule::FilterRule(const FilterRule &other)
{
    *this = other;
}

FilterRule &
FilterRule::operator=(const FilterRule &other)
{
    mySubject = other.mySubject;
    myOperation = other.myOperation;
    myIntValue = other.myIntValue;
    myStrValue = other.myStrValue;
    myRegexImpl = std::make_unique<RegexImpl>(myStrValue);
    return *this;
}

FilterRule::FilterRule(FilterRule &&) = default;
FilterRule &FilterRule::operator=(FilterRule &&) = default;

bool FilterRule::operator==(const FilterRule &other) const
{
    return mySubject == other.mySubject && myOperation == other.myOperation &&
           myIntValue == other.myIntValue && myStrValue == other.myStrValue;
}

bool FilterRule::accept(const Player &player) const
{
    switch (mySubject)
    {
        case Subject::PlayerName:
            return std::regex_match(player.getDescription(),
                                    myRegexImpl->myRegex);
        case Subject::NumStrings:
        {
            const int value = player.getTuning().getStringCount();

            switch (myOperation)
            {
                case Operation::LessThan:
                    return value < myIntValue;
                case Operation::LessThanEqual:
                    return value <= myIntValue;
                case Operation::Equal:
                    return value == myIntValue;
                case Operation::GreaterThan:
                    return value > myIntValue;
                case Operation::GreaterThanEqual:
                    return value >= myIntValue;
            }

            assert(false);
            return false;
        }
        default:
            assert(!"Unexpected subject for filter.");
            return false;
    }
}

ViewFilter::ViewFilter()
{
}

bool ViewFilter::operator==(const ViewFilter &other) const
{
    return myDescription == other.myDescription && myRules == other.myRules;
}

const std::string &ViewFilter::getDescription() const
{
    return myDescription;
}

void ViewFilter::setDescription(const std::string &description)
{
    myDescription = description;
}

void ViewFilter::addRule(const FilterRule &rule)
{
    myRules.push_back(rule);
}

void ViewFilter::removeRule(int index)
{
    myRules.erase(myRules.begin() + index);
}

boost::iterator_range<ViewFilter::RuleIterator> ViewFilter::getRules()
{
    return boost::make_iterator_range(myRules);
}

boost::iterator_range<ViewFilter::RuleConstIterator>
ViewFilter::getRules() const
{
    return boost::make_iterator_range(myRules);
}

bool ViewFilter::accept(const Score &score, int system_index,
                        int staff_index) const
{
    if (myRules.empty())
        return true;

    std::vector<const PlayerChange *> player_changes;

    const PlayerChange *current_players =
        ScoreUtils::getCurrentPlayers(score, system_index, 0);
    if (current_players)
        player_changes.push_back(current_players);

    for (const PlayerChange &change :
         score.getSystems()[system_index].getPlayerChanges())
    {
        player_changes.push_back(&change);
    }

    bool has_active_players = false;
    for (const PlayerChange *change : player_changes)
    {
        for (const ActivePlayer &active_player :
             change->getActivePlayers(staff_index))
        {
            has_active_players = true;

            const Player &player =
                score.getPlayers()[active_player.getPlayerNumber()];
            for (const FilterRule &rule : myRules)
            {
                if (rule.accept(player))
                    return true;
            }
        }
    }

    // The filter should always accept empty staves.
    return !has_active_players;
}

bool ViewFilter::accept(const Player &player) const
{
    if (myRules.empty())
        return true;

    for (const FilterRule &rule : myRules)
    {
        if (rule.accept(player))
            return true;
    }

    return false;
}

std::ostream &operator<<(std::ostream &os, const ViewFilter &filter)
{
    os << filter.getDescription() << ": " << filter.getRules().size()
       << " rules";

    return os;
}

using Subject = FilterRule::Subject;
using Operation = FilterRule::Operation;

UTIL_DEFINE_ENUMTOSTRING(Subject, {
    { Subject::PlayerName, "PlayerName" },
    { Subject::NumStrings, "NumStrings" }
})

UTIL_DEFINE_ENUMTOSTRING(Operation, {
    { Operation::LessThan, "LessThan" },
    { Operation::LessThanEqual, "LessThanEqual" },
    { Operation::Equal, "Equal" },
    { Operation::GreaterThanEqual, "GreaterThanEqual" },
    { Operation::GreaterThan, "GreaterThan" },
})
