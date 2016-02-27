/*!
 * \file MidiOutPortLinux.cpp
 * Contains platform independent MidiOutPortLinux implementation
 */

#include "MidiOutPortLinux.h"

#ifdef SMIDI_USE_ALSA
#include "alsa/MidiOutPortLinuxImpl.h"
#endif


MidiOutPortLinux::MidiOutPortLinux(const std::string& name, int clientId, int portId)
    : MidiOutPort()
    , _impl(new Implementation(name, clientId, portId))
{
	_impl->open();
}

MidiOutPortLinux::~MidiOutPortLinux()
{
}

const std::string& MidiOutPortLinux::name() const
{
	return _impl->name();
}

void MidiOutPortLinux::start()
{
}

void MidiOutPortLinux::stop()
{
}

void MidiOutPortLinux::sendMessage(const MidiMessage& message)
{
	if (_impl->isOpen())
	{
		_impl->sendMessage(message);
	}
}

int MidiOutPortLinux::applicationClientId() const
{
	return _impl->applicationClientId();
}
