/*!
 * \file MidiInPort
 */

#include "MidiInPortLinux.h"

#ifdef SMIDI_USE_ALSA
#include "alsa/MidiInPortLinuxImpl.h"
#endif

MidiInPortLinux::MidiInPortLinux(std::unique_ptr<Implementation>&& implementation)
    : MidiInPort()
    , _impl(std::move(implementation))
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
	_impl->start();
}

void MidiInPortLinux::stop()
{
	_impl->stop();
}
