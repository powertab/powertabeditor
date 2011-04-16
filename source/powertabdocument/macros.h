#ifndef __MACROS_H__
#define __MACROS_H__

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

#ifndef CHECK_THAT
#define CHECK_THAT(cond, rc) 	\
    if ( cond ) {}			\
    else {					\
        logToDebug("Error at: ", __FILE__, __LINE__); \
        assert(cond); \
        return rc;	\
    }

#endif

// Useful macro for hiding "unused parameter" warnings
#define UNUSED(x) (void)x;


std::string ArabicToRoman(uint32_t number, bool upperCase);

#endif
