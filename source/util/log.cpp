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

void trim(const unsigned int count)
{
    /* a poorman's circular buffer that keeps the last 'count' log lines. */
    std::vector<std::string> v(count);

    std::ifstream input(logPath);
    std::size_t index = 0;
    std::string s;
    while (std::getline(input, s)) {
        v[index % count] = s;
	index++;
    }

    input.close();

    // required because of len/cap discrepancy in vector used as a circular
    // buffer.
    const auto upto = (count == index ? index : count);

    std::ofstream output(logPath, std::ios::trunc);
    if (output.good()) {
        for (size_t i = 0; i < upto; ++i)
            output << v[(index + i) % count] << std::endl;
    } else {
        std::cerr << "could not open log file for trimming" << std::endl;
    }
    output.close();
}

void init(enum Level lvl, std::filesystem::path lp)
{
    FilterLevel = lvl;
    logPath = lp;

    // keep only the last X lines
    trim(1000);

    if (!logFile) {
        const auto mode = std::ios_base::out | std::ios_base::app;
        logFile = std::ofstream(logPath, mode);
    }
}

std::string all()
{
    std::stringstream ss;
    std::ifstream ifs(logPath);
    ss << ifs.rdbuf();
    return ss.str();
}

}
