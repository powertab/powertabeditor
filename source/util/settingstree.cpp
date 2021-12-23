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

#include "settingstree.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iomanip>
#include <nlohmann/json.hpp>

using SettingValue = SettingsTree::SettingValue;
using SettingList = SettingsTree::SettingList;
using SettingMap = SettingsTree::SettingMap;

using JSONValue = nlohmann::json;

struct Inserter
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
        SettingValue &value = map.values[myComponents[myIndex]];

        if (myIndex == myComponents.size() - 1)
            value = myNewValue;
        else
        {
            if (!std::holds_alternative<SettingMap>(value))
                value = SettingMap();

            ++myIndex;
            std::visit(*this, value);
        }
    }

private:
    std::vector<std::string> myComponents;
    size_t myIndex;
    SettingValue myNewValue;
};

struct Finder
{
    Finder(const std::string &key)
        : myIndex(0)
    {
        boost::algorithm::split(myComponents, key, boost::is_any_of("/"));
    }

    template <typename T>
    std::optional<SettingValue> operator()(const T &) const
    {
        return std::nullopt;
    }

    std::optional<SettingValue> operator()(const SettingMap &map) const
    {
        auto it = map.values.find(myComponents[myIndex]);
        if (it == map.values.end())
            return std::nullopt;

        const SettingValue &value = it->second;
        if (myIndex == myComponents.size() - 1)
            return value;
        else
        {
            if (!std::holds_alternative<SettingMap>(value))
                return std::nullopt;

            ++myIndex;
            return std::visit(*this, value);
        }
    }

private:
    std::vector<std::string> myComponents;
    mutable size_t myIndex;
};

struct Remover
{
    Remover(const std::string &key)
        : myIndex(0)
    {
        boost::algorithm::split(myComponents, key, boost::is_any_of("/"));
    }

    template <typename T>
    void operator()(T &)
    {
    }

    void operator()(SettingMap &map)
    {
        auto it = map.values.find(myComponents[myIndex]);
        if (it != map.values.end())
        {
            if (myIndex == myComponents.size() - 1)
                map.values.erase(it);
            else
            {
                ++myIndex;
                std::visit(*this, it->second);
            }
        }
    }

private:
    std::vector<std::string> myComponents;
    size_t myIndex;
};

static void parseValue(SettingValue &value, const JSONValue &json_val)
{
    if (json_val.is_number())
        value = json_val.get<int>();
    else if (json_val.is_string())
        value = json_val.get<std::string>();
    else if (json_val.is_boolean())
        value = json_val.get<bool>();
    else if (json_val.is_array())
    {
        SettingList value_list;

        for (auto child_json_val : json_val)
        {
            SettingValue child_val;
            parseValue(child_val, child_json_val);
            value_list.values.push_back(child_val);
        }

        value = value_list;
    }
    else if (json_val.is_object())
    {
        SettingMap value_map;

        for (auto &&[key, child_json_val] : json_val.items())
        {
            SettingValue child_val;
            parseValue(child_val, child_json_val);
            value_map.values[key] = child_val;
        }

        value = value_map;
    }
    else
        throw std::runtime_error("Unexpected JSON value type.");
}

struct JSONSerializer
{
    // Default implementation for int, std::string, etc.
    template <typename T>
    JSONValue operator()(const T &val)
    {
        return val;
    }

    JSONValue operator()(const SettingList &list)
    {
        JSONValue arr;

        for (auto &&value : list.values)
            arr.push_back(std::visit(*this, value));

        return arr;
    }

    JSONValue operator()(const SettingMap &map)
    {
        JSONValue obj;

        for (auto &&[key, val] : map.values)
            obj[key] = std::visit(*this, val);

        return obj;
    }
};

SettingsTree::SettingsTree() : myTree(SettingMap())
{
}

void SettingsTree::setImpl(const std::string &key, const SettingValue &value)
{
    Inserter inserter(key, value);
    std::visit(inserter, myTree);
}

std::optional<SettingValue> SettingsTree::find(
    const std::string &key) const
{
    return std::visit(Finder(key), myTree);
}

void SettingsTree::remove(const std::string &key)
{
    Remover visitor(key);
    return std::visit(visitor, myTree);
}

void SettingsTree::loadFromJSON(std::istream &is)
{
    JSONValue document;
    is >> document;
    parseValue(myTree, document);
}

void SettingsTree::saveToJSON(std::ostream &os) const
{
    JSONValue document = std::visit(JSONSerializer{}, myTree);
    os << std::setw(4) << document;
}
