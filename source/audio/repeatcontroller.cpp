#include "repeatcontroller.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>

#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;

RepeatController::RepeatController(const Score* score) :
    score(score)
{
    indexRepeats();
}

/// Scans through the entire score and finds all of the pairs of repeat bars
void RepeatController::indexRepeats()
{
    const Barline* activeStartBar = score->GetSystem(0)->GetStartBarPtr();
    uint32_t activeStartBarSystem = 0; // system that the active start bar is in

    for (size_t currentSystem = 0; currentSystem < score->GetSystemCount(); currentSystem++)
    {
        shared_ptr<System> system = score->GetSystem(currentSystem);

        vector<Barline*> barlines;
        system->GetBarlines(barlines);

        for (size_t i = 0; i < barlines.size(); i++)
        {
            const Barline* currentBar = barlines.at(i);

            if (currentBar->IsRepeatStart())
            {
                activeStartBar = currentBar;
                activeStartBarSystem = currentSystem;
            }
            else if (currentBar->IsRepeatEnd())
            {
                repeatList.push_back(Repeat(activeStartBarSystem, activeStartBar->GetPosition(),
                                            currentSystem, currentBar->GetPosition(),
                                            currentBar->GetRepeatCount() - 1));
            }
        }
    }
}

/// Checks if a repeat needs to be performed at the given system and position.
/// @return true If the playback position needs to be changed, and
/// updates the newSystem and newPos parameters with the new playback position
bool RepeatController::checkForRepeat(uint32_t currentSystem, uint32_t currentPos,
                                  uint32_t& newSystem, uint32_t& newPos)
{
    if (repeatList.empty()) // no repeat events left in the score
    {
        return false;
    }

    Repeat& activeRepeat = repeatList.front();

    if (currentSystem != activeRepeat.endBarSystem || currentPos != activeRepeat.endBarPos)
    {
        return false;
    }

    newSystem = activeRepeat.startBarSystem;
    newPos = activeRepeat.startBarPos;

    activeRepeat.repeatsRemaining -= 1;

    if (activeRepeat.repeatsRemaining == 0)
    {
        repeatList.erase(repeatList.begin());
    }

    return true;
}

RepeatController::Repeat::Repeat(uint32_t startBarSystem, uint32_t startBarPos, uint32_t endBarSystem,
                                 uint32_t endBarPos, uint8_t numRepeats) :
    startBarSystem(startBarSystem),
    startBarPos(startBarPos),
    endBarSystem(endBarSystem),
    endBarPos(endBarPos),
    repeatsRemaining(numRepeats)
{

}
