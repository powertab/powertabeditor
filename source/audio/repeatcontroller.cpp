#include "repeatcontroller.h"

#include <powertabdocument/score.h>
#include <powertabdocument/system.h>

#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;
using std::make_pair;

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

        vector<const Barline*> barlines;
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
                // add to the repeat list, indexed by the location of the end bar
                repeats[make_pair(currentSystem, currentBar->GetPosition())] = Repeat(activeStartBarSystem,
                                                                                      activeStartBar->GetPosition(),
                                                                                      currentBar->GetRepeatCount() - 1);
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
    if (repeats.empty()) // no repeat events left in the score
    {
        return false;
    }

    // Find if there is a repeat event at the given location
    auto repeatIterator = repeats.find(make_pair(currentSystem, currentPos));

    if (repeatIterator == repeats.end())
    {
        return false;
    }

    Repeat& activeRepeat = repeatIterator->second;

    newSystem = activeRepeat.startBarSystem;
    newPos = activeRepeat.startBarPos;

    activeRepeat.repeatsRemaining -= 1;

    if (activeRepeat.repeatsRemaining == 0)
    {
        repeats.erase(repeatIterator);
    }

    return true;
}

RepeatController::Repeat::Repeat() :
    startBarSystem(0), startBarPos(0), repeatsRemaining(0)
{
}

RepeatController::Repeat::Repeat(uint32_t startBarSystem, uint32_t startBarPos, uint8_t numRepeats) :
    startBarSystem(startBarSystem),
    startBarPos(startBarPos),
    repeatsRemaining(numRepeats)
{

}
