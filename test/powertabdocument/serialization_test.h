#ifndef SERIALIZATION_TEST_H
#define SERIALIZATION_TEST_H

#include <boost/test/unit_test.hpp>
#include <sstream>
#include <powertabdocument/powertabinputstream.h>
#include <powertabdocument/powertaboutputstream.h>
#include <powertabdocument/powertabfileheader.h>

/// Serialize and then deserialize the given object, and then check for equality
template <typename T>
void testSerialization(T& originalObject)
{
    std::ostringstream outputStrStream(std::ostringstream::binary);
    PowerTabOutputStream outputStream(outputStrStream);
    originalObject.Serialize(outputStream);

    std::istringstream inputStrStream(outputStrStream.str(), std::istringstream::binary);
    PowerTabInputStream inputStream(inputStrStream);

    T newObject;
    newObject.Deserialize(inputStream, PowerTabFileHeader::FILEVERSION_CURRENT);

    BOOST_CHECK(originalObject == newObject);
}

#endif // SERIALIZATION_TEST_H
