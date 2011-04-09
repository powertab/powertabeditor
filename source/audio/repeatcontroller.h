#ifndef REPEATCONTROLLER_H
#define REPEATCONTROLLER_H

#include <cstdint>
#include <vector>

class Score;

/// Keeps track of repeat events during playback
class RepeatController
{
    struct Repeat;

public:
    RepeatController(const Score* score);

    bool checkForRepeat(uint32_t currentSystem, uint32_t currentPos,
                    uint32_t& newSystem, uint32_t& newPos);

private:
    void indexRepeats();

    const Score* score;
    std::vector<Repeat> repeatList; ///< Contains all repeat events in the score, in order of occurrence

    struct Repeat
    {
        Repeat(uint32_t startBarSystem, uint32_t startBarPos, uint32_t endBarSystem,
               uint32_t endBarPos, uint8_t numRepeats);

        uint32_t startBarSystem;    ///< System index of the starting bar
        uint32_t startBarPos;       ///< Position index of the starting bar
        uint32_t endBarSystem;      ///< System index of the ending bar
        uint32_t endBarPos;         ///< Position index of the ending bar
        uint8_t repeatsRemaining;   ///< Number of repeats left to perform
    };
};

#endif // REPEATCONTROLLER_H
