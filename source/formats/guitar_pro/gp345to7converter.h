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

#ifndef FORMATS_GP_GP345TO7CONVERTER_H
#define FORMATS_GP_GP345TO7CONVERTER_H

#include "document.h"
#include <formats/gp7/document.h>

namespace Gp
{
/// Converts the Guitar Pro 3/4/5 document into a GP 7 document.
Gp7::Document convertToGp7(const Gp::Document &doc);
}

#endif

