/* Copyright (C) 2021-2023 Simon Symeonidis
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
#include "log.h"

namespace Log {

enum Level FilterLevel = Level::Debug;

std::optional<std::ofstream> logFile;

std::filesystem::path logPath;

std::mutex lock;

void init(enum Level lvl, std::filesystem::path lp)
{
    FilterLevel = lvl;
    logPath = lp;
}

std::string all()
{
    std::stringstream ss;
    std::ifstream ifs(logPath);
    ss << ifs.rdbuf();
    return ss.str();
}

}
