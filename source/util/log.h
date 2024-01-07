/*
 * Copyright (C) 2021-2024 Simon Symeonidis
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

#ifndef UTIL_LOG_H
#define UTIL_LOG_H

#include <filesystem>
#include <string>

/* TODO: fmtlib: replace with std, when all compilers support `format'. */
#include <fmt/core.h>

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

void init(enum Level lvl, std::filesystem::path logPath);

/**
 * return the full contents of the log file
 */
std::string all();

/**
 * takes a string, and emits it.  Our emitters consider only a log file on the
 * platform, and print to console if there is a problem if the file can not be
 * opened.  The emitters will also add the current timestamp, and the log level
 * of the message.
 */
void emitLog(enum Level level, const std::string& str);

template <typename... Args>
void backend(const enum Level level, std::string_view fmt, Args&&... args)
{
    emitLog(level, fmt::vformat(fmt, fmt::make_format_args(args...)));
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
