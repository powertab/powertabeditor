#ifndef LAYOUT_H
#define LAYOUT_H

#include <memory>

class Score;
class System;

/// Contains various functions for computing the layout of staves, systems, notes, etc
namespace Layout
{
void calculateStdNotationHeight(Score* score, std::shared_ptr<System> system);
}

#endif // LAYOUT_H
