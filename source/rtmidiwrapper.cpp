#include "rtmidiwrapper.h"

RtMidiWrapper::RtMidiWrapper()
{
	midiout=new RtMidiOut();
}

bool RtMidiWrapper::sendMidiMessage(int a, int b, int c)
{
	std::vector<unsigned char> message;

	if (a>=0)
		message.push_back(a);

	if (b>=0)
		message.push_back(b);

	if (c>=0)
		message.push_back(c);

	if (message.empty())
		return false;

	try
	{
		midiout->sendMessage(&message);
	}
	catch (...)
	{
		 return false;
	}

	return true;
}

bool RtMidiWrapper::initialize()
{
	unsigned int num_ports = midiout->getPortCount();

	if (num_ports<=0)
		return false;

	try
	{
		midiout->openPort();
	}
	catch (...)
	{
		 return false;
	}

	return true;
}

int RtMidiWrapper::getPortCount()
{
	return midiout->getPortCount();
}

std::string RtMidiWrapper::getPortName(int port)
{
	return midiout->getPortName(port);
}

bool RtMidiWrapper::usePort(int port)
{
	try
	{
		midiout->closePort();
		midiout->openPort(port);
	}
	catch (...)
	{
		 return false;
	}

	return true;
}

bool RtMidiWrapper::setPatch (int channel, int patch)
{
	if (patch<0)
		patch=0;
	else if (patch>127)
		patch=127;

	// MIDI program change
	// first parameter is 0xC0-0xCF with C being the id and 0-F being the channel (0-15)
	// second parameter is the new patch (0-127)
	int command=192+channel;
	return sendMidiMessage(command, patch, -1);
}

bool RtMidiWrapper::setVolume (int channel, int volume)
{
	if (volume<0)
		volume=0;
	else if (volume>100)
		volume=100;

	// convert volume from range 0-100 into range 0-127 for MIDI
	volume = (int)((float)volume*1.27f);

	// MIDI control change
	// first parameter is 0xB0-0xBF with B being the id and 0-F being the channel (0-15)
	// second parameter is the control to change (0-127), 7 is channel volume
	// third parameter is the new volume (0-127)
	int command=176+channel;
	return sendMidiMessage(command, 7, volume);
}

bool RtMidiWrapper::setPan (int channel, int pan)
{
	if (pan<-100)
		pan=-100;
	else if (pan>100)
		pan=100;

	// convert pan from range -100-100 into range 0-127 for MIDI
	pan = (int)(((float)(pan+100)/200.0f)*127.0f);

	// MIDI control change
	// first parameter is 0xB0-0xBF with B being the id and 0-F being the channel (0-15)
	// second parameter is the control to change (0-127), 10 is channel pan
	// third parameter is the new pan (0-127)
	int command=176+channel;
	return sendMidiMessage(command, 10, pan);
}

bool RtMidiWrapper::setPitchBend (int channel, int bend)
{
	if (bend<-200)
		bend=-200;
	else if (bend>200)
		bend=200;

	// convert bend from range -200-200 into range 0-127 for MIDI
	bend = (int)(((float)(bend+200)/400.0f)*127.0f);

	// MIDI pitch bend
	// first parameter is 0xE0-0xEF with E being the id and 0-F being the channel (0-15)
	// second parameter is the lsb (0-127)
	// third parameter is the msb (0-127)
	int command=224+channel;
	return sendMidiMessage(command, 0, bend);
}

bool RtMidiWrapper::playNote(int channel, int pitch, int velocity)
{
	if (pitch<0)
		pitch=0;
	else if (pitch>127)
		pitch=127;

	if (velocity<1)
		velocity=1;
	else if (velocity>127)
		velocity=127;

	// MIDI note on
	// first parameter 0x90-9x9F with 9 being the id and 0-F being the channel (0-15)
	// second parameter is the pitch of the note (0-127), 60 would be a 'middle C'
	// third parameter is the velocity of the note (1-127), 0 is not allowed, 64 would be no velocity
	int command=144+channel;
	return sendMidiMessage(command, pitch, velocity);
}

bool RtMidiWrapper::stopNote(int channel, int pitch)
{
	if (pitch<0)
		pitch=0;
	else if (pitch>127)
		pitch=127;

	// MIDI note off
	// first parameter 0x80-9x8F with 8 being the id and 0-F being the channel (0-15)
	// second parameter is the pitch of the note (0-127), 60 would be a 'middle C'
	int command=128+channel;
	return sendMidiMessage(command, pitch, -1);
}
