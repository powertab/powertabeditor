/*
  * Copyright (C) 2013 Cameron White
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

#ifndef SCORE_SERIALIZATION_H
#define SCORE_SERIALIZATION_H

#include <array>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <bitset>
#include "fileversion.h"
#include <map>
#include <optional>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <stack>
#include <stdexcept>
#include <vector>

namespace ScoreUtils
{
class InputArchive
{
public:
    InputArchive(std::istream &is);

    /// The version of the file being read.
    FileVersion version() const;

    /// Generic function to read a value with the given name.
    template <typename T>
    void operator()(const std::string_view &name, T &obj)
    {
        auto name_ref = rapidjson::StringRef(name.data(), name.size());

        const JSONValue *parent = myValueStack.top();
        auto it = parent->FindMember(name_ref);

        // Field does not exist. It might have been removed in a newer file
        // version.
        if (it == parent->MemberEnd())
            return;

        myValueStack.push(&it->value);
        read(obj);
        myValueStack.pop();
    }

private:
    using JSONValue = rapidjson::GenericValue<rapidjson::UTF8<>>;

    /// The current JSON value.
    const JSONValue &value() const
    {
        return *myValueStack.top();
    }

    inline void read(int &val);
    inline void read(int8_t &val);
    inline void read(unsigned int &val);
    inline void read(uint8_t &val);
    inline void read(bool &val);
    inline void read(std::string &str);

    template <typename T>
    void read(std::vector<T> &vec);

    template <typename K, typename V, typename C>
    void read(std::map<K, V, C> &map);

    template <typename T, size_t N>
    void read(std::array<T, N> &arr);

    template <size_t N>
    void read(std::bitset<N> &bits);

    template <typename T>
    void read(std::optional<T> &val);

    inline void read(boost::gregorian::date &date);

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value>::type read(T &val)
    {
        val = static_cast<T>(value().GetInt());
    }

    template <typename T>
    typename std::enable_if<std::is_class<T>::value>::type read(T &obj)
    {
        obj.serialize(*this, myVersion);
    }

    rapidjson::IStreamWrapper myStream;
    rapidjson::Document myDocument;
    FileVersion myVersion;

    std::stack<const JSONValue *> myValueStack;
};

template <typename T>
void load(std::istream &input, const std::string &name, T &obj)
{
    InputArchive archive(input);
    archive(name, obj);
}

class OutputArchive
{
public:
    OutputArchive(std::ostream &os, FileVersion version);
    ~OutputArchive();

    template <typename T>
    void operator()(const std::string &name, const T &obj)
    {
        write(name);
        write(obj);
    }

private:
    inline void write(int val);
    inline void write(unsigned int val);
    inline void write(bool val);
    inline void write(const std::string &str);

    template <typename T>
    void write(const std::vector<T> &vec);

    template <typename K, typename V, typename C>
    void write(const std::map<K, V, C> &map);

    template <typename T, size_t N>
    void write(const std::array<T, N> &arr);

    template <size_t N>
    void write(const std::bitset<N> &bits);

    template <typename T>
    void write(const std::optional<T> &val);

    inline void write(const boost::gregorian::date &date);

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value>::type write(const T &val)
    {
        myStream.Int(static_cast<int>(val));
    }

    template <typename T>
    typename std::enable_if<std::is_class<T>::value>::type write(const T &obj)
    {
        myStream.StartObject();
        const_cast<T &>(obj).serialize(*this, myVersion);
        myStream.EndObject();
    }

    rapidjson::OStreamWrapper myWriteStream;
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> myStream;
    const FileVersion myVersion;
};

template <typename T>
void save(std::ostream &output, const std::string &name, const T &obj)
{
    OutputArchive ar(output, FileVersion::LATEST_VERSION);
    ar(name, obj);
}

void InputArchive::read(int &val)
{
    val = value().GetInt();
}

void InputArchive::read(int8_t &val)
{
    int int_val = value().GetInt();
    if (int_val > std::numeric_limits<int8_t>::max())
        throw std::overflow_error("Invalid int8_t value");
    val = static_cast<int8_t>(int_val);
}

void InputArchive::read(unsigned int &val)
{
    val = value().GetUint();
}

void InputArchive::read(uint8_t &val)
{
    unsigned int uint_val = value().GetUint();
    if (uint_val > std::numeric_limits<uint8_t>::max())
        throw std::overflow_error("Invalid uint8_t value");
    val = static_cast<uint8_t>(uint_val);
}

void InputArchive::read(bool &val)
{
    val = value().GetBool();
}

void InputArchive::read(std::string &str)
{
    str = value().GetString();
}

template <typename T>
void InputArchive::read(std::vector<T> &vec)
{
    const JSONValue::ConstArray &json_array = value().GetArray();
    vec.resize(json_array.Size());

    size_t i = 0;
    for (const JSONValue &value : json_array)
    {
        myValueStack.push(&value);
        read(vec[i++]);
        myValueStack.pop();
    }
}

template <typename K, typename V, typename C>
void InputArchive::read(std::map<K, V, C> &map)
{
    const JSONValue::ConstObject &obj = value().GetObject();

    for (auto &&member : obj)
    {
        static_assert(std::is_same<K, int>::value,
                      "Only integer keys are currently supported");
        const K key = std::stoi(member.name.GetString());

        myValueStack.push(&member.value);

        V value;
        read(value);
        map[key] = value;

        myValueStack.pop();
    }
}

template <typename T, size_t N>
void InputArchive::read(std::array<T, N> &arr)
{
    for (size_t i = 0; i < N; ++i)
        (*this)(std::to_string(i), arr[i]);
}

template <size_t N>
void InputArchive::read(std::bitset<N> &bits)
{
    std::string data;
    read(data);
    bits = std::bitset<N>(data);
}

template <typename T>
void InputArchive::read(std::optional<T> &val)
{
    if (value().IsNull())
        val.reset();
    else
    {
        T data;
        read(data);
        val = data;
    }
}

void InputArchive::read(boost::gregorian::date &date)
{
    std::string date_str;
    read(date_str);
    date = boost::gregorian::from_undelimited_string(date_str);
}

void OutputArchive::write(int val)
{
    myStream.Int(val);
}

void OutputArchive::write(unsigned int val)
{
    myStream.Uint(val);
}

void OutputArchive::write(bool val)
{
    myStream.Bool(val);
}

void OutputArchive::write(const std::string &str)
{
    myStream.String(str.c_str(),
                    static_cast<rapidjson::SizeType>(str.length()));
}

template <typename T>
void OutputArchive::write(const std::vector<T> &vec)
{
    myStream.StartArray();
    for (const T &obj : vec)
        write(obj);
    myStream.EndArray();
}

template <typename K, typename V, typename C>
void OutputArchive::write(const std::map<K, V, C> &map)
{
    myStream.StartObject();

    for (const auto &pair : map)
        (*this)(std::to_string(pair.first), pair.second);

    myStream.EndObject();
}

template <typename T, size_t N>
void OutputArchive::write(const std::array<T, N> &arr)
{
    myStream.StartObject();

    for (size_t i = 0; i < N; ++i)
        (*this)(std::to_string(i), arr[i]);

    myStream.EndObject();
}

template <size_t N>
void OutputArchive::write(const std::bitset<N> &bits)
{
    write(bits.to_string());
}

template <typename T>
void OutputArchive::write(const std::optional<T> &val)
{
    if (val)
        write(*val);
    else
        myStream.Null();
}

void OutputArchive::write(const boost::gregorian::date &date)
{
    write(boost::gregorian::to_iso_string(date));
}
}

#endif
