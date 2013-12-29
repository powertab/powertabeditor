/*
  * Copyright (C) 2011 Cameron White
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
  
#ifndef MACROS_H
#define MACROS_H

#include <cassert>
#include <cstdint>
#include <string>

// Little-endian based LONG/WORD/BYTE macros
#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((uint16_t)(((uint8_t)((uint32_t)(a) & 0xff)) | ((uint16_t)((uint8_t)((uint32_t)(b) & 0xff))) << 8))
#endif

#ifndef MAKELONG
#define MAKELONG(a, b)      ((uint32_t)(((uint16_t)((uint32_t)(a) & 0xffff)) | ((uint32_t)((uint16_t)((uint32_t)(b) & 0xffff))) << 16))
#endif

#ifndef LOWORD
#define LOWORD(l)           ((uint16_t)((uint32_t)(l) & 0xffff))
#endif

#ifndef HIWORD
#define HIWORD(l)           ((uint16_t)((uint32_t)(l) >> 16))
#endif

#ifndef LOBYTE
#define LOBYTE(w)           ((uint8_t)((uint32_t)(w) & 0xff))
#endif

#ifndef HIBYTE
#define HIBYTE(w)           ((uint8_t)((uint32_t)(w) >> 8))
#endif

void logToDebug(const std::string& msg, const std::string& file, int line);

#ifndef PTB_CHECK_THAT
#define PTB_CHECK_THAT(cond, rc) 	\
    if ( cond ) {}			\
    else {					\
        logToDebug("Error at: ", __FILE__, __LINE__); \
        assert(cond); \
        return rc;	\
    }

#endif

std::string ArabicToRoman(uint32_t number, bool upperCase);

#endif // MACROS_H
