#include "directionsymbol.h"

#include <powertabdocument/direction.h>

DirectionSymbol::DirectionSymbol(uint8_t symbolType, uint8_t activeSymbol, uint8_t repeatNumber) :
    symbolType(symbolType),
    activeSymbol(activeSymbol),
    repeatNumber(repeatNumber)
{
}

uint8_t DirectionSymbol::getActiveSymbol() const
{
    return activeSymbol;
}

uint8_t DirectionSymbol::getRepeatNumber() const
{
    return repeatNumber;
}

uint8_t DirectionSymbol::getSymbolType() const
{
    return symbolType;
}

/// Determines whether the direction should be performed, based on the given information
bool DirectionSymbol::shouldPerformDirection(uint8_t activeSymbol, uint8_t activeRepeat) const
{
    return (this->activeSymbol == activeSymbol && symbolType >= Direction::fine &&
            (repeatNumber == 0 || repeatNumber == activeRepeat));
}
