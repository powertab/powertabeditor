#include "systemlocation.h"

SystemLocation::SystemLocation(uint32_t system, uint32_t position) :
    system(system),
    position(position)
{
}

SystemLocation::SystemLocation() :
    system(0),
    position(0)
{
}

bool SystemLocation::operator<(const SystemLocation& location) const
{
    if (system == location.system)
    {
        return position < location.position;
    }
    else
    {
        return system < location.system;
    }
}

bool SystemLocation::operator==(const SystemLocation& location) const
{
    return system == location.system && position == location.position;
}

uint32_t SystemLocation::getPositionIndex() const
{
    return position;
}

uint32_t SystemLocation::getSystemIndex() const
{
    return system;
}
