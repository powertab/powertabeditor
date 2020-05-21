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

#include <app/appinfo.h>
#include <boost/algorithm/string/replace.hpp>

#import <Foundation/Foundation.h>

using SettingValue = SettingsTree::SettingValue;
using SettingList = SettingsTree::SettingList;
using SettingMap = SettingsTree::SettingMap;

struct PListValueVisitor
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
        auto array = [NSMutableArray arrayWithCapacity:list.values.size()]; 

        for (auto &&value : list.values)
        {
            auto obj = std::visit(*this, value);
            [array addObject:obj];
        }

        return array;
    }

    NSObject *operator()(const SettingMap &) const
    {
        return nil;
    }
};

struct PListSerializer
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
        for (auto &&pair : map.values)
        {
            std::string prev_key = myKey;
            if (!myKey.empty())
                myKey += ".";
            myKey += pair.first;

            auto obj = std::visit(PListValueVisitor(), pair.second);
            if (obj)
            {
                NSString *key =
                    [NSString stringWithUTF8String:myKey.c_str()];
                [myPrefs setObject:obj forKey:key];
            }
            else
                std::visit(*this, pair.second);

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
    std::visit(serializer, myTree);
}

static SettingValue converToSettingValue(NSObject *obj)
{
    if ([obj isKindOfClass:[NSArray class]])
    {
        auto list = (NSArray *)obj;
        SettingList settings;

        for (NSObject *child in list)
            settings.values.push_back(converToSettingValue(child));

        return settings;
    }
    else if ([obj isKindOfClass:[NSString class]])
    {
        auto str = (NSString *)obj;
        return std::string([str UTF8String]);
    }
    else if ([obj isKindOfClass:[NSNumber class]])
    {
        auto num = (NSNumber *)obj;

        // Determine whether we have a boolean value.
        if ([num isEqualToValue:@YES] || [num isEqualToValue:@NO])
            return static_cast<bool>([num intValue]);
        else
            return [num intValue];
    }
    else
        throw std::runtime_error("Unknown object type.");
}

void SettingsTree::loadFromPlist()
{
    // Only load settings from the application's domain.
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *domain = [bundle bundleIdentifier];
    if (!domain)
        domain = [NSString stringWithUTF8String:AppInfo::APPLICATION_ID];

    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    auto dict = [prefs persistentDomainForName:domain];
    for (NSString *key in [dict allKeys])
    {
        std::string key_str([key UTF8String]);
        boost::replace_all(key_str, ".", "/");

        NSObject *obj = [prefs objectForKey:key];
        setImpl(key_str, converToSettingValue(obj));
    }
}
