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

#import <Foundation/Foundation.h>

using SettingList = SettingsTree::SettingList;
using SettingMap = SettingsTree::SettingMap;

struct PListValueVisitor : public boost::static_visitor<NSObject *>
{
    NSObject *operator()(int x) const
    {
        return [NSNumber numberWithInt:x];
    }

    NSObject *operator()(bool x) const
    {
        return [NSNumber numberWithBool:x];
    }

    NSObject *operator()(const std::string &s) const
    {
        return [NSString stringWithUTF8String:s.c_str()];
    }

    NSObject *operator()(const SettingList &list) const
    {
        auto array = [NSMutableArray arrayWithCapacity:list.size()]; 

        for (auto &&value : list)
        {
            auto obj = boost::apply_visitor(*this, value);
            [array addObject:obj];
        }

        return array;
    }

    NSObject *operator()(const SettingMap &) const
    {
        return nil;
    }
};

struct PListSerializer : public boost::static_visitor<void>
{
    PListSerializer() : myPrefs([NSUserDefaults standardUserDefaults])
    {
        // Clear any old settings.
        auto dict = [myPrefs dictionaryRepresentation];
        for (NSString *key in [dict allKeys])
            [myPrefs removeObjectForKey:key];
    }

    ~PListSerializer()
    {
        [myPrefs synchronize];
    }

    template <typename T>
    void operator()(const T &)
    {
    }

    void operator()(const SettingMap &map)
    {
        for (auto &&pair : map)
        {
            std::string prev_key = myKey;
            if (!myKey.empty())
                myKey += ".";
            myKey += pair.first;

            auto obj = boost::apply_visitor(PListValueVisitor(), pair.second);
            if (obj)
            {
                NSString *key =
                    [NSString stringWithUTF8String:myKey.c_str()];
                [myPrefs setObject:obj forKey:key];
            }
            else
                boost::apply_visitor(*this, pair.second);

            myKey = prev_key;
        }
    }

private:
    NSUserDefaults *myPrefs;
    std::string myKey;
};

void SettingsTree::saveToPlist() const
{
    PListSerializer serializer;
    boost::apply_visitor(serializer, myTree);
}
