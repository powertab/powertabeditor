/*
 * Copyright (C) 2021-2023 Simon Symeonidis
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
#ifndef APP_LOG_H
#define APP_LOG_H

#include <chrono>
#include <iostream>
#include <iomanip> /* put_time */
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>

#include <vector>


namespace Log {

enum class Level {
    Debug = 0,
    Info = 1,
    Notify = 2,
    Warning = 3,
    Error = 4,
};

/**
 * toggleable log level - set this to the minimum level to filter from a la
 * rfc5424.
 */
extern enum Level CurrentLevel;

extern std::filesystem::path logPath;

extern std::optional<std::ofstream> logFile;

void init(enum Level lvl, std::filesystem::path logPath);

/**
 * return the full contents of the log file
 */
std::string all();

template <typename... Args>
void backend(enum Level level, std::string_view fmt, Args&&... args)
{
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
        << std::vformat(fmt, std::make_format_args(args...))
        << std::endl;

    if (!logFile) {
        const auto mode = std::ios_base::out | std::ios_base::app;
        logFile = std::ofstream(logPath, mode);
    }

    if (logFile.value().good()) {
        logFile.value()
            << timestamp_now_str_fn() << ": "
            << level_str_fn(level) << ": "
            << std::vformat(fmt, std::make_format_args(args...))
            << std::endl;
    } else {
        std::cout << std::format(
            "{} {} could not open file at ({})",
            timestamp_now_str_fn(),
	    level_str_fn(Level::Warning),
	    logPath.generic_string()
	);
    }
}

template <typename... Args>
void d(std::string_view fmt, Args&&... args) { backend(Level::Debug, fmt, std::forward<Args>(args)...); }

template <typename... Args>
void i(std::string_view fmt, Args&&... args) { backend(Level::Info, fmt, std::forward<Args>(args)...); }

template <typename... Args>
void n(std::string_view fmt, Args&&... args) { backend(Level::Notify, fmt, std::forward<Args>(args)...); }

template <typename... Args>
void w(std::string_view fmt, Args&&... args) { backend(Level::Warning, fmt, std::forward<Args>(args)...); }

template <typename... Args>
void e(std::string_view fmt, Args&&... args) { backend(Level::Error, fmt, std::forward<Args>(args)...); }

}
#endif
