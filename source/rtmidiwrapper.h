#ifndef RTMIDIWRAPPER_H
#define RTMIDIWRAPPER_H

#include <string>
#include <memory>

class RtMidiOut;

class RtMidiWrapper
{
private:
    std::unique_ptr<RtMidiOut> midiout;
    bool sendMidiMessage(int a, int b, int c);

public:
    RtMidiWrapper();
    ~RtMidiWrapper();

    bool initialize(unsigned int preferredPort = 0);
    int getPortCount();
    std::string getPortName(int port);
    bool usePort(int port);
    bool setPatch(int channel, int patch);
    bool setVolume(int channel, int volume); // volume in the range 0-100
    bool setPan(int channel, int pan); // pan in the range -100(left)-100(right)
    bool setPitchBend(int channel, int bend); // bend in the range -200(2 semitones down)-200(2 semitones up)
    bool playNote(int channel, int pitch, int velocity);
    bool stopNote(int channel, int pitch);
};

#endif // RTMIDIWRAPPER_H
