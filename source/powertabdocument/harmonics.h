#ifndef HARMONICS_H
#define HARMONICS_H

#include <cstdint>
#include <vector>

/// Contains useful utility functions for working with
/// natural harmonics and tapped harmonics that fall outside
/// of the scope of the Note class
namespace Harmonics
{
    uint8_t getPitchOffset(uint8_t fretOffset);
    std::vector<uint8_t> getFretOffsets();
}

#endif // HARMONICS_H
