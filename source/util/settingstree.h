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

#ifndef APP_SETTINGSTREE_H
#define APP_SETTINGSTREE_H

#include <iosfwd>
#include <optional>
#include <string>
#include <map>
#include <variant>
#include <vector>

template <typename T>
class Setting
{
public:
    Setting(std::string key, T default_value)
        : myKey(std::move(key)), myDefaultValue(std::move(default_value))
    {
    }

    const std::string myKey;
    const T myDefaultValue;
};

class SettingsTree
{
public:
    struct SettingList;
    struct SettingMap;

    using SettingValue =
        std::variant<int, std::string, bool, SettingList, SettingMap>;

    struct SettingList { std::vector<SettingValue> values; };
    struct SettingMap { std::map<std::string, SettingValue> values; };

    SettingsTree();

    /// Retrieve the value associated with the given key, or the default value
    /// if not found.
    /// @throws std::exception If T does not match the value's type.
    template <typename T>
    T get(const std::string &key, const T &default_val = T()) const;

    /// Retrieve a list of values associated with the given key.
    template <typename T>
    std::vector<T> getList(const std::string &key) const;

    template <typename T>
    T get(const Setting<T> &setting) const
    {
        return get<T>(setting.myKey, setting.myDefaultValue);
    }

    template <typename T>
    std::vector<T> get(const Setting<std::vector<T>> &setting) const
    {
        return getList<T>(setting.myKey);
    }

    /// Set the value associated with the key.
    template <typename T>
    void set(const std::string &key, const T &value);

    /// Since the implicit conversion from const char * to bool takes
    /// precedence over the implicit std::string constructor, add an explicit
    /// overload that does the right thing.
    void set(const std::string &key, const char *value)
    {
        set(key, std::string(value));
    }

    /// Set the value associated with the key to a list of values.
    template <typename T>
    void setList(const std::string &key, const std::vector<T> &values);

    template <typename T>
    void set(const Setting<T> &setting, const T &value)
    {
        set(setting.myKey, value);
    }

    template <typename T>
    void set(const Setting<std::vector<T>> &setting,
             const std::vector<T> &value)
    {
        setList(setting.myKey, value);
    }

    /// Removes a setting.
    void remove(const std::string &key);

    void loadFromJSON(std::istream &is);
    void saveToJSON(std::ostream &os) const;

    /// On OSX only, saves the settings using NSUserDefaults.
    void saveToPlist() const;
    void loadFromPlist();

private:
    void setImpl(const std::string &key, const SettingValue &value);
    std::optional<SettingValue> find(const std::string &key) const;

    SettingValue myTree;
};

/// Specialize to support custom types as setting values.
template <typename T>
struct SettingValueConverter
{
    static SettingsTree::SettingValue to(const T &t)
    {
        return t;
    }

    static T from(const SettingsTree::SettingValue &v)
    {
        return std::get<T>(v);
    }
};

template <typename T>
T SettingsTree::get(const std::string &key, const T &default_val) const
{
    std::optional<SettingValue> val = find(key);
    return val ? SettingValueConverter<T>::from(*val) : default_val;
}

template <typename T>
std::vector<T> SettingsTree::getList(const std::string &key) const
{
    auto list = get<SettingList>(key);

    std::vector<T> ts;
    ts.reserve(list.values.size());
    for (auto &&val : list.values)
        ts.push_back(SettingValueConverter<T>::from(val));

    return ts;
}

template <typename T>
void SettingsTree::set(const std::string &key, const T &val)
{
    setImpl(key, SettingValueConverter<T>::to(val));
}

template <typename T>
void SettingsTree::setList(const std::string &key, const std::vector<T> &values)
{
    std::vector<SettingValue> tmp(values.begin(), values.end());
    setImpl(key, SettingList{ tmp });
}

#endif
