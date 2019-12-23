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

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

using SettingValue = SettingsTree::SettingValue;
using SettingList = SettingsTree::SettingList;
using SettingMap = SettingsTree::SettingMap;

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
    boost::optional<SettingValue> operator()(const T &) const
    {
        return boost::none;
    }

    boost::optional<SettingValue> operator()(const SettingMap &map) const
    {
        auto it = map.values.find(myComponents[myIndex]);
        if (it == map.values.end())
            return boost::none;

        const SettingValue &value = it->second;
        if (myIndex == myComponents.size() - 1)
            return value;
        else
        {
            if (!std::holds_alternative<SettingMap>(value))
                return boost::none;

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

static void parseValue(SettingValue &value, const rapidjson::Value &json_val)
{
    if (json_val.IsInt())
        value = json_val.GetInt();
    else if (json_val.IsString())
        value = std::string(json_val.GetString(), json_val.GetStringLength());
    else if (json_val.IsBool())
        value = json_val.GetBool();
    else if (json_val.IsArray())
    {
        SettingList value_list;

        for (auto it = json_val.Begin(); it != json_val.End(); ++it)
        {
            SettingValue child_val;
            parseValue(child_val, *it);
            value_list.values.push_back(child_val);
        }

        value = value_list;
    }
    else if (json_val.IsObject())
    {
        SettingMap value_map;

        for (auto it = json_val.MemberBegin(); it != json_val.MemberEnd(); ++it)
        {
            SettingValue child_val;
            parseValue(child_val, it->value);
            value_map.values[it->name.GetString()] = child_val;
        }

        value = value_map;
    }
    else
        throw std::runtime_error("Unexpected JSON value type.");
}

struct JSONSerializer
{
    JSONSerializer(std::ostream &os) : myStream(os), myWriter(myStream)
    {
    }

    void operator()(int x)
    {
        myWriter.Int(x);
    }

    void operator()(bool x)
    {
        myWriter.Bool(x);
    }

    void operator()(const std::string &s)
    {
        myWriter.String(s.c_str(),
                        static_cast<rapidjson::SizeType>(s.length()));
    }

    void operator()(const SettingList &list)
    {
        myWriter.StartArray();
        for (auto &&value : list.values)
            std::visit(*this, value);
        myWriter.EndArray();
    }

    void operator()(const SettingMap &map)
    {
        myWriter.StartObject();

        // Output in sorted order.
        std::vector<std::string> keys;
        keys.reserve(map.values.size());
        for (auto &&pair : map.values)
            keys.push_back(pair.first);

        std::sort(keys.begin(), keys.end());

        for (auto &&key : keys)
        {
            myWriter.Key(key.c_str());
            std::visit(*this, map.values.at(key));
        }

        myWriter.EndObject();
    }

private:
    rapidjson::OStreamWrapper myStream;
    rapidjson::PrettyWriter<decltype(myStream)> myWriter;
};

SettingsTree::SettingsTree() : myTree(SettingMap())
{
}

void SettingsTree::setImpl(const std::string &key, const SettingValue &value)
{
    Inserter inserter(key, value);
    std::visit(inserter, myTree);
}

boost::optional<SettingValue> SettingsTree::find(
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
    rapidjson::IStreamWrapper stream(is);

    rapidjson::Document document;
    document.ParseStream(stream);

    if (document.HasParseError())
    {
        throw std::runtime_error("Parse error at offset " +
                                 std::to_string(document.GetErrorOffset()) +
                                 ": " +
                                 GetParseError_En(document.GetParseError()));
    }

    parseValue(myTree, document);
}

void SettingsTree::saveToJSON(std::ostream &os) const
{
    JSONSerializer serializer(os);
    std::visit(serializer, myTree);
}
