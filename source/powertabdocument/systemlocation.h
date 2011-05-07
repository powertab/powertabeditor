#ifndef SYSTEMLOCATION_H
#define SYSTEMLOCATION_H

#include <cstdint>
#include <boost/operators.hpp> // used to generate additional operators, e.g. <=, !=, etc

/// Convenient class to represent a location in the score - contains system index and position index
class SystemLocation :
        boost::equality_comparable<SystemLocation>, // generate != from operator==
        boost::less_than_comparable<SystemLocation> // generate <=, >, >= from operator<
{
public:
    SystemLocation();
    SystemLocation(uint32_t system, uint32_t position);

    bool operator<(const SystemLocation& location) const;
    bool operator==(const SystemLocation& location) const;

    uint32_t getSystemIndex() const;
    uint32_t getPositionIndex() const;

private:
    uint32_t system;
    uint32_t position;
};

#endif // SYSTEMLOCATION_H
