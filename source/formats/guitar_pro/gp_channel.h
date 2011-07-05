#ifndef GP_CHANNEL_H
#define GP_CHANNEL_H

#include <cstdint>

namespace Gp {

class InputStream;

/// Holds information about a Midi Channel in a Guitar Pro document
/// - Used as a temporary structure while importing files, and handles the formatting conversions
class Channel
{
public:
    Channel();

    static uint8_t readChannelProperty(Gp::InputStream& stream);

    uint32_t instrument;
    uint8_t volume;
    uint8_t balance;
    uint8_t chorus;
    uint8_t reverb;
    uint8_t phaser;
    uint8_t tremolo;
};

}

#endif // GP_CHANNEL_H
