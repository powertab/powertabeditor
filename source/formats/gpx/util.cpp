#include "util.h"

#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_binary.hpp>

/// Converts 4 bytes starting at the given index into an integer
uint32_t Gpx::Util::readUInt(const std::vector<char>& bytes, size_t index)
{
    uint32_t value = 0;

    auto begin = bytes.begin() + index;

    using namespace boost::spirit;
    qi::parse(begin, begin + sizeof(uint32_t), qi::dword, value);

    return value;
}
