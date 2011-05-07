#ifndef REPEAT_H
#define REPEAT_H

#include <cstdint>
#include <powertabdocument/systemlocation.h>
#include <map>

/// Represents a repeat end bar
class RepeatEnd
{
public:
    RepeatEnd();
    RepeatEnd(uint8_t repeatCount);

    bool performRepeat();

private:
    uint8_t repeatCount;
    uint8_t remainingRepeats;
};

/// Contains all information about a repeat - start bar, end bar(s), alternate endings, repeat count, etc
class Repeat
{
public:
    Repeat();
    Repeat(const SystemLocation& startBarLocation);

    void addRepeatEnd(const SystemLocation& location, const RepeatEnd& endBar);
    SystemLocation performRepeat(const SystemLocation& location);

private:
    std::map<SystemLocation, RepeatEnd> endBars;
    SystemLocation startBarLocation;
    uint8_t activeRepeat;
};

#endif // REPEAT_H
