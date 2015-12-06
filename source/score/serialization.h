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
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <bitset>
#include "fileversion.h"
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <stack>
#include <stdexcept>
#include <util/rapidjson_iostreams.h>
#include <vector>

namespace ScoreUtils
{
class InputArchive
{
public:
    InputArchive(std::istream &is);

    FileVersion version() const;

    template <typename T>
    void operator()(const std::string &expectedName, T &obj)
    {
        if (expectedName != name())
        {
            throw std::runtime_error(
                std::string("Unexpected or missing JSON data: found ") +
                name() + ", expected " + expectedName);
        }

        read(obj);
        advance();
    }

private:
    typedef rapidjson::GenericValue<rapidjson::UTF8<> > JSONValue;

    struct ValueVisitor : public boost::static_visitor<const JSONValue &>
    {
        const JSONValue &operator()(
            const JSONValue::ConstMemberIterator &it) const
        {
            return it->value;
        }

        const JSONValue &operator()(
            const JSONValue::ConstValueIterator &it) const
        {
            return *it;
        }
    };

    struct NameVisitor : public boost::static_visitor<std::string>
    {
        std::string operator()(const JSONValue::ConstMemberIterator &it) const
        {
            return it->name.GetString();
        }

        std::string operator()(const JSONValue::ConstValueIterator &) const
        {
            throw std::logic_error("Cannot read the name of an array element");
        }
    };

    struct AdvanceVisitor : public boost::static_visitor<void>
    {
        template <typename Iterator>
        void operator()(Iterator &it) const
        {
            ++it;
        }
    };

    void advance()
    {
        AdvanceVisitor visitor;
        myIterators.top().apply_visitor(visitor);
    }

    std::string name() const
    {
        NameVisitor visitor;
        return myIterators.top().apply_visitor(visitor);
    }

    const JSONValue &value() const
    {
        ValueVisitor visitor;
        return myIterators.top().apply_visitor(visitor);
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
    void read(boost::optional<T> &val);

    inline void read(boost::gregorian::date &date);

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value>::type read(T &val)
    {
        val = static_cast<T>(value().GetInt());
    }

    template <typename T>
    typename std::enable_if<std::is_class<T>::value>::type read(T &obj)
    {
        myIterators.push(value().MemberBegin());
        obj.serialize(*this, myVersion);
        myIterators.pop();
    }

    Util::RapidJSON::IStreamWrapper myStream;
    rapidjson::Document myDocument;
    FileVersion myVersion;

	// Iterate over both objects and arrays in a uniform manner.
    typedef boost::variant<JSONValue::ConstMemberIterator,
                           JSONValue::ConstValueIterator> Iterator;
    std::stack<Iterator> myIterators;
};

template <typename T>
void load(std::istream &input, const std::string &name, T &obj)
{
    InputArchive archive(input);
    if (archive.version() > FileVersion::LATEST_VERSION ||
        archive.version() < FileVersion::INITIAL_VERSION)
    {
        throw std::runtime_error("Invalid file version");
    }

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
    void write(const boost::optional<T> &val);

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

    Util::RapidJSON::OStreamWrapper myWriteStream;
    rapidjson::PrettyWriter<Util::RapidJSON::OStreamWrapper> myStream;
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
    auto size = value().Size();
    myIterators.push(value().Begin());

    vec.resize(size);
    for (unsigned int i = 0; i < size; ++i)
    {
        read(vec[i]);
        advance();
    }

    myIterators.pop();
}

template <typename K, typename V, typename C>
void InputArchive::read(std::map<K, V, C> &map)
{
    auto it = value().MemberBegin();
    const long long size = std::distance(it, value().MemberEnd());
    myIterators.push(it);

    for (long long i = 0; i < size; ++i)
    {
        const K key = boost::lexical_cast<K>(name());

        V value;
        read(value);
        map[key] = value;

        advance();
    }

    myIterators.pop();
}

template <typename T, size_t N>
void InputArchive::read(std::array<T, N> &arr)
{
    myIterators.push(value().MemberBegin());

    for (size_t i = 0; i < N; ++i)
        (*this)(std::to_string(i), arr[i]);

    myIterators.pop();
}

template <size_t N>
void InputArchive::read(std::bitset<N> &bits)
{
    std::string data;
    read(data);
    bits = std::bitset<N>(data);
}

template <typename T>
void InputArchive::read(boost::optional<T> &val)
{
    if (value().IsNull())
        val.reset();
    else
    {
        T data;
        read(data);
        val.reset(data);
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
void OutputArchive::write(const boost::optional<T> &val)
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
