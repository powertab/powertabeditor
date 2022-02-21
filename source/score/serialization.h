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
#include "fileversion.h"
#include <istream>
#include <iomanip>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <stack>
#include <stdexcept>
#include <util/date.h>
#include <util/enumflags.h>
#include <util/enumtostring_fwd.h>
#include <vector>

namespace ScoreUtils
{
namespace detail
{
    using JSONValue = nlohmann::json;

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
            const JSONValue *parent = myValueStack.top();
            auto it = parent->find(name);

            // Field does not exist. It might have been removed in a newer file
            // version.
            if (it == parent->end())
                return;

            myValueStack.push(&*it);
            read(obj);
            myValueStack.pop();
        }

    private:
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

        template <typename EnumT>
        void read(Util::EnumFlags<EnumT> &bits);

        template <typename T>
        void read(std::optional<T> &val);

        void read(Util::Date &date);

        template <typename T>
        void read(T &val)
        {
            if constexpr (std::is_class_v<T>)
                val.serialize(*this, myVersion);
            else if constexpr (std::is_enum_v<T>)
            {
                // Convert from string to enum.
                // In older files, enums were stored as ints.
                if (myVersion < FileVersion::JSON_CLEANUP)
                    val = static_cast<T>(value().get<int>());
                else
                {
                    std::string text = value().get<std::string>();
                    if (auto result = Util::toEnum<T>(text))
                        val = *result;
                    else
                    {
                        std::cerr << "Unknown enum value: " << text
                                  << std::endl;
                    }
                }
            }
            else
                assert(false);
        }

        JSONValue myDocument;
        FileVersion myVersion;

        std::stack<const JSONValue *> myValueStack;
    };

    class OutputArchive
    {
    public:
        OutputArchive(FileVersion version) : myVersion(version)
        {
        }

        template <typename T>
        void operator()(const std::string &name, const T &obj)
        {
            myValue[name] = convert(obj);
        }

        const JSONValue &value() const
        {
            return myValue;
        }

    private:
        template <typename T>
        JSONValue convert(const std::vector<T> &vec);

        template <typename K, typename V, typename C>
        JSONValue convert(const std::map<K, V, C> &map);

        template <typename T, size_t N>
        JSONValue convert(const std::array<T, N> &arr);

        template <typename EnumT>
        JSONValue convert(const Util::EnumFlags<EnumT> &flags);

        template <typename T>
        JSONValue convert(const std::optional<T> &val);

        JSONValue convert(const Util::Date &date);

        template <typename T>
        JSONValue convert(const T &obj)
        {
            // Save enums as strings. The exception is the FileVersion enum
            // which is left as an integer.
            if constexpr (std::is_enum_v<T> && !std::is_same_v<T, FileVersion>)
            {
                return Util::enumToString(obj);
            }
            // Save ints / bools / etc or strings as-is.
            else if constexpr (std::is_pod_v<T> ||
                               std::is_same_v<T, std::string>)
            {
                return obj;
            }
            else // score objects.
            {
                OutputArchive ar(myVersion);
                const_cast<T &>(obj).serialize(ar, myVersion);
                return std::move(ar.myValue);
            }
        }

        const FileVersion myVersion;
        JSONValue myValue;
    };

    void InputArchive::read(int &val)
    {
        val = value().get<int>();
    }

    void InputArchive::read(int8_t &val)
    {
        int int_val = value().get<int>();
        if (int_val > std::numeric_limits<int8_t>::max())
            throw std::overflow_error("Invalid int8_t value");
        val = static_cast<int8_t>(int_val);
    }

    void InputArchive::read(unsigned int &val)
    {
        val = value().get<unsigned int>();
    }

    void InputArchive::read(uint8_t &val)
    {
        unsigned int uint_val = value().get<unsigned int>();
        if (uint_val > std::numeric_limits<uint8_t>::max())
            throw std::overflow_error("Invalid uint8_t value");
        val = static_cast<uint8_t>(uint_val);
    }

    void InputArchive::read(bool &val)
    {
        val = value().get<bool>();
    }

    void InputArchive::read(std::string &str)
    {
        str = value().get<std::string>();
    }

    template <typename T>
    void InputArchive::read(std::vector<T> &vec)
    {
        const JSONValue &json_array = value();
        vec.resize(json_array.size());

        size_t i = 0;
        for (const JSONValue &entry : json_array)
        {
            myValueStack.push(&entry);
            read(vec[i++]);
            myValueStack.pop();
        }
    }

    template <typename K, typename V, typename C>
    void InputArchive::read(std::map<K, V, C> &map)
    {
        const JSONValue &json_obj = value();

        for (auto &&member : json_obj.items())
        {
            static_assert(std::is_same<K, int>::value,
                          "Only integer keys are currently supported");
            const K key = std::stoi(member.key());

            myValueStack.push(&member.value());

            V val;
            read(val);
            map[key] = val;

            myValueStack.pop();
        }
    }

    template <typename T, size_t N>
    void InputArchive::read(std::array<T, N> &arr)
    {
        for (size_t i = 0; i < N; ++i)
            (*this)(std::to_string(i), arr[i]);
    }

    template <typename EnumT>
    void InputArchive::read(Util::EnumFlags<EnumT> &flags)
    {
        // In old files, the flags were stored as a string of 0's or 1's
        // In new file, they are stored as a string array containing the active
        // enum values.
        if (myVersion < FileVersion::JSON_CLEANUP)
        {
            std::string data;
            read(data);
            flags.setFromString(data);
        }
        else
        {
            const JSONValue &json_array = value();
            for (const JSONValue &entry : json_array)
            {
                auto flag = Util::toEnum<EnumT>(entry.get<std::string>());
                if (flag) // Ignore any unknown flags from future file versions.
                    flags.setFlag(*flag, true);
            }
        }
    }

    template <typename T>
    void InputArchive::read(std::optional<T> &val)
    {
        if (value() == nullptr)
            val.reset();
        else
        {
            T data;
            read(data);
            val = data;
        }
    }

    template <typename T>
    JSONValue OutputArchive::convert(const std::vector<T> &vec)
    {
        JSONValue arr;

        for (const T &obj : vec)
            arr.push_back(convert(obj));

        return arr;
    }

    template <typename K, typename V, typename C>
    JSONValue OutputArchive::convert(const std::map<K, V, C> &map)
    {
        JSONValue obj;

        for (auto &&[key, value] : map)
            obj[std::to_string(key)] = convert(value);

        return obj;
    }

    template <typename T, size_t N>
    JSONValue OutputArchive::convert(const std::array<T, N> &arr)
    {
        JSONValue obj;

        for (size_t i = 0; i < N; ++i)
            obj[std::to_string(i)] = convert(arr[i]);

        return obj;
    }

    template <typename EnumT>
    JSONValue OutputArchive::convert(const Util::EnumFlags<EnumT> &flags)
    {
        JSONValue arr = JSONValue::array();

        // Convert to an array of strings, with the enum values for the active
        // flags.
        for (size_t i = 0; i < Util::EnumFlags<EnumT>::NumFlags; ++i)
        {
            auto flag = static_cast<EnumT>(i);
            if (flags.getFlag(flag))
                arr.push_back(Util::enumToString(flag));
        }

        return arr;
    }

    template <typename T>
    JSONValue OutputArchive::convert(const std::optional<T> &val)
    {
        return val ? convert(*val) : JSONValue(nullptr);
    }
} // namespace detail

template <typename T>
void
load(std::istream &input, const std::string &name, T &obj)
{
    detail::InputArchive archive(input);
    archive(name, obj);
}

template <typename T>
void
save(std::ostream &output, const std::string &name, const T &obj,
     bool pretty = true)
{
    FileVersion version = FileVersion::LATEST_VERSION;
    detail::OutputArchive ar(version);
    ar("version", version);
    ar(name, obj);

    // Pretty print.
    if (pretty)
        output << std::setw(4);

    output << ar.value();
}
} // namespace ScoreUtils

#endif
