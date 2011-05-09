#ifndef DIRECTIONSYMBOL_H
#define DIRECTIONSYMBOL_H

#include <cstdint>

/// Convenient class for storing information about a single direction
/// (vs. the Direction class which represents multiple directions)
class DirectionSymbol
{
public:
    DirectionSymbol(uint8_t symbolType, uint8_t activeSymbol, uint8_t repeatNumber);

    uint8_t getActiveSymbol() const;
    uint8_t getSymbolType() const;
    uint8_t getRepeatNumber() const;

    bool shouldPerformDirection(uint8_t activeSymbol, uint8_t activeRepeat) const;

private:
    uint8_t symbolType;
    uint8_t activeSymbol;
    uint8_t repeatNumber;
};

#endif // DIRECTIONSYMBOL_H
