#include "repeat.h"

#include <powertabdocument/alternateending.h>

using std::shared_ptr;

Repeat::Repeat(const SystemLocation& startBarLocation) :
    startBarLocation(startBarLocation),
    activeRepeat(1)
{
}

Repeat::Repeat():
    activeRepeat(1)
{
}

/// Adds a new end bar to the repeat
void Repeat::addRepeatEnd(const SystemLocation& location, const RepeatEnd& endBar)
{
    endBars[location] = endBar;
}

/// Adds an alternate ending to the repeat group
void Repeat::addAlternateEnding(shared_ptr<const AlternateEnding> altEnding)
{
    const SystemLocation location(altEnding->GetSystem(), altEnding->GetPosition());

    std::vector<uint8_t> numbers = altEnding->GetListOfNumbers();

    // for each repeat that the ending is active, add it to the alternate endings map along with
    // the location of the ending
    for (auto i = numbers.begin(); i != numbers.end(); ++i)
    {
        alternateEndings[*i] = location;
    }
}

/// Performs a repeat event if possible
/// @return The playback position to shift to
SystemLocation Repeat::performRepeat(const SystemLocation& currentLocation)
{
    // deal with alternate endings - if we are at the start of the first alternate
    // ending, we can branch off to other alternate endings depending on the active repeat
    auto firstAltEnding = alternateEndings.find(1);
    if (firstAltEnding != alternateEndings.end() && firstAltEnding->second == currentLocation)
    {
        // branch off to the next alternate ending, if it exists
        auto nextAltEnding = alternateEndings.find(activeRepeat);
        if (nextAltEnding != alternateEndings.end())
        {
            return nextAltEnding->second;
        }
    }

    // now, we can look for repeat end bars
    auto repeatEnd = endBars.find(currentLocation);

    if (repeatEnd == endBars.end()) // no repeat end bar at the current location
    {
        return currentLocation;
    }
    else if (repeatEnd->second.performRepeat()) // repeat event performed
    {
        activeRepeat++;
        return startBarLocation;
    }
    else // repeat end bar exists, but no repeat event was performed
    {
        return currentLocation;
    }
}

uint8_t Repeat::getActiveRepeat() const
{
    return activeRepeat;
}

RepeatEnd::RepeatEnd() :
    repeatCount(0),
    remainingRepeats(0)
{
}

RepeatEnd::RepeatEnd(uint8_t repeatCount) :
    repeatCount(repeatCount),
    remainingRepeats(repeatCount - 1)
{
}

bool RepeatEnd::performRepeat()
{
    if (remainingRepeats != 0)
    {
        remainingRepeats--;
        return true;
    }
    else
    {
        remainingRepeats = repeatCount - 1;
        return false;
    }
}
