#ifndef LAYOUT_H
#define LAYOUT_H

#include <memory>
#include <vector>

class Score;
class System;
class Staff;

/// Contains various functions for computing the layout of staves, systems, notes, etc
namespace Layout
{
void CalculateStdNotationHeight(Score* score, std::shared_ptr<System> system);

void CalculateTabStaffBelowSpacing(std::shared_ptr<const System> system, std::shared_ptr<Staff> staff);
void CalculateSymbolSpacing(const Score* score, std::shared_ptr<System> system,
                            std::shared_ptr<Staff> staff);

void FormatSystem(std::shared_ptr<System> system);

enum SymbolType
{
    NoSymbol,
    // symbols between staves
    SymbolLetRing,
    SymbolVolumeSwell,
    SymbolVibrato,
    SymbolWideVibrato,
    SymbolPalmMuting,
    SymbolTremoloPicking,
    SymbolTremoloBar,
    SymbolTrill,
    SymbolNaturalHarmonic,
    SymbolArtificialHarmonic,
    SymbolDynamic,
    // symbols below tab staff
    SymbolPickStrokeDown,
    SymbolPickStrokeUp,
    SymbolTap,
    SymbolHammerOnPullOff,
    SymbolSlide,
    SymbolTappedHarmonic
};

struct SymbolGroup
{
    SymbolGroup(int leftPosIndex, int leftX, int width, int height, SymbolType type);

    int leftPosIndex; /// Position index of the start of the symbol group
    int leftX; /// x-coordinate of the start of the symbol group
    int width; /// Width (in coordinates) of the group
    int height; /// offset from the staff
    SymbolType symbolType;
};

std::vector<Layout::SymbolGroup> CalculateSymbolLayout(const Score* score,
                                                       std::shared_ptr<const System> system,
                                                       std::shared_ptr<const Staff> staff);

std::vector<Layout::SymbolGroup> CalculateTabStaffBelowLayout(std::shared_ptr<const System> system,
                                                              std::shared_ptr<const Staff> staff);
}

#endif // LAYOUT_H
