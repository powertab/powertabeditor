/*
 * Copyright (C) 2022 Cameron White
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

#include "document.h"

using Ottavia = Gp7::Beat::Ottavia;

UTIL_DEFINE_ENUMTOSTRING(Ottavia, { { Ottavia::O8va, "8va" },
                                    { Ottavia::O8vb, "8vb" },
                                    { Ottavia::O15ma, "15ma" },
                                    { Ottavia::O15mb, "15mb" } })
