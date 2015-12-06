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

#include <boost/optional/optional.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>

#include <iosfwd>
#include <unordered_map>
#include <vector>

class SettingsTree
{
public:
    using SettingValue = boost::make_recursive_variant<
        int,
        unsigned int,
        std::string,
        std::vector<boost::recursive_variant_>,
        std::unordered_map<std::string, boost::recursive_variant_>>::type;

    using SettingList = std::vector<SettingValue>;
    using SettingMap = std::unordered_map<std::string, SettingValue>;

    SettingsTree();

    /// Retrieve the value associated with the given key, or the default value
    /// if not found.
    /// @throws std::exception If T does not match the value's type.
    template <typename T>
    T get(const std::string &key, const T &default_val = T()) const;

    /// Retrieve a list of values associated with the given key.
    template <typename T>
    std::vector<T> getList(const std::string &key) const;

    /// Set the value associated with the key.
    void set(const std::string &key, const SettingValue &value);

    /// Set the value associated with the key to a list of values.
    template <typename T>
    void setList(const std::string &key, const std::vector<T> &values);

    void save(std::ostream &os) const;

private:
    boost::optional<SettingValue> find(const std::string &key) const;

    SettingValue myTree;
};

template <typename T>
T SettingsTree::get(const std::string &key, const T &default_val) const
{
    boost::optional<SettingValue> val = find(key);
    return val ? boost::get<T>(*val) : default_val;
}

template <typename T>
std::vector<T> SettingsTree::getList(const std::string &key) const
{
    auto values = get<std::vector<SettingValue>>(key);

    std::vector<T> ts;
    ts.reserve(values.size());
    for (auto &&val : values)
        ts.push_back(boost::get<T>(val));

    return ts;
}

template <typename T>
void SettingsTree::setList(const std::string &key, const std::vector<T> &values)
{
    std::vector<SettingValue> tmp(values.begin(), values.end());
    set(key, SettingValue(tmp));
}

#endif
