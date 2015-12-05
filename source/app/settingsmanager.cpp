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

#include "settingsmanager.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

using SettingValue = SettingsManager::SettingValue;
using SettingMap = SettingsManager::SettingMap;

SettingsManager::SettingsManager() : myTree(SettingMap())
{
}

struct IsMap : public boost::static_visitor<bool>
{
    template <typename T>
    bool operator()(const T &) const
    {
        return false;
    }

    bool operator()(const SettingMap &) const
    {
        return true;
    }
};

struct Inserter : public boost::static_visitor<>
{
    Inserter(const std::string &key, SettingValue value)
        : myIndex(0), myNewValue(std::move(value))
    {
        boost::algorithm::split(myComponents, key, boost::is_any_of("/"));
    }

    template <typename T>
    void operator()(T &)
    {
        assert(false);
    }

    void operator()(SettingMap &map)
    {
        SettingValue &value = map[myComponents[myIndex]];

        if (myIndex == myComponents.size() - 1)
            value = myNewValue;
        else
        {
            if (!boost::apply_visitor(IsMap(), value))
                value = SettingMap();

            ++myIndex;
            boost::apply_visitor(*this, value);
        }
    }

private:
    std::vector<std::string> myComponents;
    size_t myIndex;
    SettingValue myNewValue;
};

struct Finder : public boost::static_visitor<boost::optional<SettingValue>>
{
    Finder(const std::string &key)
        : myIndex(0)
    {
        boost::algorithm::split(myComponents, key, boost::is_any_of("/"));
    }

    template <typename T>
    boost::optional<SettingValue> operator()(const T &) const
    {
        return boost::none;
    }

    boost::optional<SettingValue> operator()(const SettingMap &map) const
    {
        auto it = map.find(myComponents[myIndex]);
        if (it == map.end())
            return boost::none;

        const SettingValue &value = it->second;
        if (myIndex == myComponents.size() - 1)
            return value;
        else
        {
            if (!boost::apply_visitor(IsMap(), value))
                return boost::none;

            ++myIndex;
            return boost::apply_visitor(*this, value);
        }
    }

private:
    std::vector<std::string> myComponents;
    mutable size_t myIndex;
};

void SettingsManager::set(const std::string &key, const SettingValue &value)
{
    Inserter inserter(key, value);
    boost::apply_visitor(inserter, myTree);
}

boost::optional<SettingValue> SettingsManager::find(
    const std::string &key) const
{
    return boost::apply_visitor(Finder(key), myTree);
}
