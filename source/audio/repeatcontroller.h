#ifndef REPEATCONTROLLER_H
#define REPEATCONTROLLER_H

#include <cstdint>
#include <map>
#include <audio/repeat.h>
#include <audio/directionsymbol.h>
#include <memory>
#include <boost/unordered_map.hpp>

class Score;
class System;

/// Keeps track of repeat events during playback
class RepeatController
{
public:
    RepeatController(const Score* score);

    bool checkForRepeat(uint32_t currentSystem, uint32_t currentPos,
                    uint32_t& newSystem, uint32_t& newPos);

private:
    void indexRepeats();
    void indexDirections(uint32_t systemIndex, std::shared_ptr<const System> system);

    SystemLocation performMusicalDirection(uint8_t directionType);

    Repeat& getPreviousRepeatGroup(const SystemLocation& location);

    const Score* score;
    uint8_t activeSymbol; ///< active musical direction symbol during playback

    std::map<SystemLocation, Repeat> repeats; ///< Holds all repeats in the score

    boost::unordered_multimap<SystemLocation, DirectionSymbol> directions; ///< Stores each musical direction in the system
    boost::unordered_map<uint8_t, SystemLocation> symbolLocations; ///< Stores the location of each music symbol (coda, etc)
};

#endif // REPEATCONTROLLER_H
