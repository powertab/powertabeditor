/* Copyright (C) 2021-2024 Simon Symeonidis
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

#include <chrono>
#include <iostream>
#include <iomanip> /* put_time */
#include <fstream>
#include <sstream>
#include <mutex>
#include <optional>
#include <vector>

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

void emitLog(enum Level level, const std::string& str)
{
    std::lock_guard<std::mutex> _guard(Log::lock);

    /* formats a timestamp in a rfc3339 fashion; I believe the system clock
     * defaults to epoch, which should be UTC, but I might be wrong.  */
    const auto timestamp_now_str_fn = []() -> std::string {
	auto now = std::chrono::system_clock::now();
	auto now_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&now_t), "%Y-%m-%dT%H:%M:%SZ");
	return ss.str();
    };

    const auto level_str_fn = [](enum Level l) noexcept {
        switch (l) {
        case Level::Debug: return "[debug]";
        case Level::Info: return "[info]";
        case Level::Notify: return "[notify]";
        case Level::Warning: return "[warn]";
        case Level::Error: return "[error]";
        /* unreachable */
	default:
	    return "unknown";
        }
    };

    std::cout
        << timestamp_now_str_fn() << ": "
        << level_str_fn(level) << ": "
        << str
        << std::endl;

    if (logFile.value().good()) {
        logFile.value()
            << timestamp_now_str_fn() << ": "
            << level_str_fn(level) << ": "
            << str
            << std::endl;
    } else {
        std::cout << fmt::format(
            "{} {} could not open file at ({})",
            timestamp_now_str_fn(),
	    level_str_fn(Level::Warning),
	    logPath.generic_string()
	);
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
