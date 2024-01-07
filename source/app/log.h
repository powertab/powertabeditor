/*
 * Copyright (C) 2024 Simon Symeonidis
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

#include <util/log.h>

#include <QString>
#include <fmt/core.h>
#include <fmt/format.h>
#include <string>

/* Adapted from: https://fmt.dev/latest/api.html#udt */
template <>
struct fmt::formatter<QString> : fmt::formatter<std::string>
{
    auto format(const QString& qstr, fmt::format_context& ctx) const {
        return fmt::formatter<std::string>::format(qstr.toStdString(), ctx);
    }
};

#endif
