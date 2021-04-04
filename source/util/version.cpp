/*
  * Copyright (C) 2020 Cameron White
  * Copyright (C) 2021 Simon Symeonidis
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

#include "version.h"

#include <boost/preprocessor/stringize.hpp>

namespace Version {
    std::string get() {
        std::string ret;
#ifdef PTE_VERSION
        ret = " (v" + std::string(BOOST_PP_STRINGIZE(PTE_VERSION)) + ")";
#else
#error A PTE_VERSION must be set. Make sure nothing has broken in the build process.
#endif
        return ret;
    }
}
