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

#ifndef UTIL_RAPIDJSON_IOSTREAMS_H
#define UTIL_RAPIDJSON_IOSTREAMS_H

#include <cassert>
#include <cstddef>
#include <istream>
#include <string>

namespace Util
{
namespace RapidJSON
{
    /// Wrapper class to use a std::istream with RapidJSON.
    class IStreamWrapper
    {
    public:
        typedef char Ch;
        typedef std::istream::traits_type traits_type;

        IStreamWrapper(std::istream &stream);

        Ch Peek() const
        {
            // We cannot check eof() here due to a bug in earlier versions of
            // libc++ (http://stackoverflow.com/a/14150918).
            const char ch = myStream.peek();
            return traits_type::eq_int_type(ch, traits_type::eof()) ? '\0' : ch;
        }

        Ch Take()
        {
            if (myStream.eof())
                return '\0';
            return static_cast<Ch>(myStream.get());
        }

        size_t Tell() const
        {
            return static_cast<size_t>(myStream.tellg());
        }

        Ch *PutBegin()
        {
            assert(false);
            return 0;
        }

        void Put(Ch)
        {
            assert(false);
        }

        void Flush()
        {
            assert(false);
        }

        size_t PutEnd(Ch *)
        {
            assert(false);
            return 0;
        }

    private:
        std::istream &myStream;
    };

    /// Wrapper class to use a std::ostream with RapidJSON.
    class OStreamWrapper
    {
    public:
        typedef char Ch;

        OStreamWrapper(std::ostream &stream);

        Ch Peek() const
        {
            assert(false);
            return '\0';
        }

        Ch Take() const
        {
            assert(false);
            return '\0';
        }

        size_t Tell() const
        {
            return static_cast<size_t>(myStream.tellp());
        }

        Ch *PutBegin()
        {
            assert(false);
            return 0;
        }

        void Put(Ch c)
        {
            myStream.put(c);
        }

        void Flush()
        {
            myStream.flush();
        }

        size_t PutEnd(Ch *)
        {
            assert(false);
            return 0;
        }

    private:
        std::ostream &myStream;
    };
}
}

#endif
