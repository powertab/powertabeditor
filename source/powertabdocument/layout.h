#ifndef LAYOUT_H
#define LAYOUT_H

#include <memory>

class Score;
class System;
class Staff;

/// Contains various functions for computing the layout of staves, systems, notes, etc
namespace Layout
{
void CalculateStdNotationHeight(Score* score, std::shared_ptr<System> system);

void CalculateTabStaffBelowSpacing(std::shared_ptr<Staff> staff);
void CalculateSymbolSpacing(const Score* score, std::shared_ptr<System> system,
                            std::shared_ptr<Staff> staff);
}

#endif // LAYOUT_H
