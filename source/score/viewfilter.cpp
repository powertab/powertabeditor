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

FilterRule::FilterRule()
    : mySubject(Subject::PLAYER_NAME),
      myOperation(Operation::EQUAL),
      myIntValue(0)
{
}

FilterRule::FilterRule(Subject subject, std::string value)
    : mySubject(subject),
      myOperation(Operation::EQUAL),
      myIntValue(0),
      myStrValue(std::move(value)),
      myRegex(myStrValue)
{
}

FilterRule::FilterRule(Subject subject, Operation op, int value)
    : mySubject(subject), myOperation(op), myIntValue(value)
{
}

bool FilterRule::operator==(const FilterRule &other) const
{
    return mySubject == other.mySubject && myOperation == other.myOperation &&
           myIntValue == other.myIntValue && myStrValue == other.myStrValue;
}

bool FilterRule::accept(const Score &score, int system_index,
                        int staff_index) const
{
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
        for (const ActivePlayer &player : change->getActivePlayers(staff_index))
        {
            has_active_players = true;

            if (accept(score, player))
                return true;
        }
    }

    // The filter should always accept empty staves.
    return !has_active_players;
}

bool FilterRule::accept(const Score &score, const ActivePlayer &p) const
{
    const Player &player = score.getPlayers()[p.getPlayerNumber()];

    switch (mySubject)
    {
    case PLAYER_NAME:
        return boost::regex_match(player.getDescription(), myRegex);
    case NUM_STRINGS:
    {
        const int value = player.getTuning().getStringCount();

        switch (myOperation)
        {
        case LESS_THAN:
            return value < myIntValue;
        case LESS_THAN_EQUAL:
            return value <= myIntValue;
        case EQUAL:
            return value == myIntValue;
        case GREATER_THAN:
            return value > myIntValue;
        case GREATER_THAN_EQUAL:
            return value >= myIntValue;
        }
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

    for (const FilterRule &rule : myRules)
    {
        if (rule.accept(score, system_index, staff_index))
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
