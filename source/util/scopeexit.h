/*
  * Copyright (C) 2020 Cameron White
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

#ifndef UTIL_SCOPEEXIT_H
#define UTIL_SCOPEEXIT_H

#include <utility>

namespace Util
{
/// Simple replacement for Boost scope_exit, which calls the provided function
/// upon destruction.
template <typename F>
class ScopeExit
{
public:
    ScopeExit(F f) : myFunction(std::move(f))
    {
    }

    ScopeExit(const ScopeExit &) = delete;
    ScopeExit &operator=(const ScopeExit &) = delete;
    ScopeExit(const ScopeExit &&) = delete;
    ScopeExit &operator=(const ScopeExit &&) = delete;

    ~ScopeExit()
    {
        myFunction();
    }

private:
    F myFunction;
};
} // namespace Util

#endif

