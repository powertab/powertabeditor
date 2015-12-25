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

#include <boost/filesystem/path.hpp>
#include <vector>

namespace Paths {
    using path = boost::filesystem::path;

    /// Return a path to a directory where config files should be written to.
    path getConfigDir();

    /// Return a path to a directory where persistent application data should
    /// be written to.
    path getUserDataDir();

    /// Return a list of paths where persistent application data should be read
    /// from, ordered from highest to lowest priority.
    std::vector<path> getDataDirs();

    /// Return a path to the user's home directory.
    path getHomeDir();
}
