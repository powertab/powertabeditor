#ifndef GP_STREAM_H
#define GP_STREAM_H

#include <istream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <type_traits>
#include <bitset>

namespace Gp
{

typedef std::bitset<8> Flags;

class InputStream
{
public:
    InputStream(std::istream& stream);

    template <class T>
    T read();

    std::string readString();
    std::string readIntString();
    std::string readFixedLengthString(uint32_t maxLength);

    std::string readVersionString();

    void skip(int numBytes);

private:
    template <class LengthPrefixType>
    std::string readCharacterString();

    std::istream& stream_;
};

/// Reads simple data (e.g. uint32_t, int16_t) from the input stream
template <class T>
inline T InputStream::read()
{
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    T data;
    stream_.read((char*)&data, sizeof(data));
    return data;
}

/// Reads a character string.
/// The string consists of some number of bytes (encoding the length of the string, n)
/// followed by n characters
/// This is templated on the length prefix type, to allow for strings prefixed with a 2-byte length value,
/// 4-byte length value, etc
template <typename LengthPrefixType>
inline std::string InputStream::readCharacterString()
{
    static_assert(std::is_integral<LengthPrefixType>::value, "LengthPrefix must be an integral type");

    const LengthPrefixType length = read<LengthPrefixType>();

    std::string str;
    str.resize(length);
    stream_.read(&str[0], length);
    return str;
}

}

#endif // GP_STREAM_H
