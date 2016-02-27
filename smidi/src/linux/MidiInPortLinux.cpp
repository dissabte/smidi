/*!
 * \file MidiInPort
 */

#include "MidiInPortLinux.h"

#ifdef SMIDI_USE_ALSA
#include "alsa/MidiInPortLinuxImpl.h"
#endif

MidiInPortLinux::MidiInPortLinux(const std::string& name, int clientId, int portId)
    : MidiInPort()
    , _impl(new Implementation(name, clientId, portId))
{
	_impl->open();
}

MidiInPortLinux::~MidiInPortLinux()
{
}

const std::string& MidiInPortLinux::name() const
{
	return _impl->name();
}

void MidiInPortLinux::start()
{
}

void MidiInPortLinux::stop()
{
}

int MidiInPortLinux::applicationClientId() const
{
	return _impl->applicationClientId();
}
