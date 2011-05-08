#ifndef REPEATCONTROLLER_H
#define REPEATCONTROLLER_H

#include <cstdint>
#include <map>
#include <audio/repeat.h>

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

    std::map<SystemLocation, Repeat> repeats; ///< Holds all repeats in the score
    Repeat& getPreviousRepeatGroup(const SystemLocation& location);
};

#endif // REPEATCONTROLLER_H
