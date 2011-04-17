#ifndef RTMIDIWRAPPER_H
#define RTMIDIWRAPPER_H

/**
// MIDI control change:
// first parameter is 0xB0-0xBF with B being the id and 0-F being the channel (0-15)
// second parameter is the control to change (0-127), (e.g. 7 is channel volume)
// third parameter is the new value (0-127)
**/

#include <string>
#include <memory>

class RtMidiOut;

class RtMidiWrapper
{
private:
    std::unique_ptr<RtMidiOut> midiout;
    bool sendMidiMessage(uint8_t a, uint8_t b, uint8_t c);

public:
    RtMidiWrapper();
    ~RtMidiWrapper();

    bool initialize(uint32_t preferredPort = 0);
    uint32_t getPortCount();
    std::string getPortName(uint32_t port);
    bool usePort(uint32_t port);
    bool setPatch(uint8_t channel, uint8_t patch);
    bool setVolume(uint8_t channel, uint8_t volume);
    bool setPan(uint8_t channel, uint8_t pan); // pan in the range -100(left)-100(right)
    bool setPitchBend(uint8_t channel, uint8_t bend); // bend in the range -200(2 semitones down)-200(2 semitones up)
    bool playNote(uint8_t channel, uint8_t pitch, uint8_t velocity);
    bool stopNote(uint8_t channel, uint8_t pitch);
    bool setVibrato(uint8_t channel, uint8_t modulation);
    bool setSustain(uint8_t channel, bool sustainOn);

    enum MidiMessage
    {
        NOTE_OFF = 128,
        NOTE_ON = 144,
        CONTROL_CHANGE = 176,
        PROGRAM_CHANGE = 192,
        PITCH_WHEEL = 224
    };

    enum ControlChanges
    {
        MOD_WHEEL = 1,
        CHANNEL_VOLUME = 7,
        PAN_CHANGE = 10,
        HOLD_PEDAL = 64
    };
};

#endif // RTMIDIWRAPPER_H
