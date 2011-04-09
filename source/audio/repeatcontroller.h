#ifndef REPEATCONTROLLER_H
#define REPEATCONTROLLER_H

#include <cstdint>
#include <boost/unordered_map.hpp>

class Score;

/// Keeps track of repeat events during playback
class RepeatController
{
public:
    RepeatController(const Score* score);

    bool checkForRepeat(uint32_t currentSystem, uint32_t currentPos,
                    uint32_t& newSystem, uint32_t& newPos);

private:
    void indexRepeats();

    const Score* score;

    /// Holds information about a repeat - where the playback position should move to, and how many repeats are left
    struct Repeat
    {
        Repeat();
        Repeat(uint32_t startBarSystem, uint32_t startBarPos, uint8_t numRepeats);

        uint32_t startBarSystem;    ///< System index of the starting bar
        uint32_t startBarPos;       ///< Position index of the starting bar
        uint8_t repeatsRemaining;   ///< Number of repeats left to perform
    };

    /// Holds all repeat events in the score, indexed by the system and position index of the end bar
    boost::unordered_map<std::pair<uint32_t, uint32_t>, Repeat> repeats;
};

#endif // REPEATCONTROLLER_H
