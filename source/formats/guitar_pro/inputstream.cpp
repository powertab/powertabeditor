#include "inputstream.h"

#include <cassert>

Gp::InputStream::InputStream(std::istream& stream) :
    stream_(stream)
{
    stream_.exceptions(std::istream::failbit | std::istream::badbit | std::istream::eofbit);
}

/// Reads the file version
std::string Gp::InputStream::readVersionString()
{
    stream_.seekg(std::ios_base::beg);

    // the version consists of a 30 character string, although not all 30
    // characters may be used
    std::string version = readCharacterString<uint8_t>();

    // skip past any unread characters to land at position 0x1f
    stream_.seekg(31, std::ios_base::beg);

    return version;
}

/// Reads a string in the most common format for Guitar Pro - an integer representing the
/// size of the stored information + 1, followed by the length-prefixed string of characters
/// representing the data
std::string Gp::InputStream::readString()
{
    const uint32_t size = read<uint32_t>();

    std::string str = readCharacterString<uint8_t>();
    assert(size - 1 == str.length());

    return str;
}

/// Reads a string prefixed with 4 bytes indicating the length
std::string Gp::InputStream::readIntString()
{
    return readCharacterString<uint32_t>();
}

/// Reads a fixed length string (any unused characters trailing the string are skipped)
std::string Gp::InputStream::readFixedLengthString(uint32_t maxLength)
{
    const uint8_t actualLength = read<uint8_t>();

    std::string str;
    str.resize(actualLength);
    stream_.read(&str[0], actualLength);

    // skip any characters that didn't need to be read
    skip(maxLength - actualLength);

    return str;
}

void Gp::InputStream::skip(int numBytes)
{
    stream_.seekg(numBytes, std::ios_base::cur);
}
