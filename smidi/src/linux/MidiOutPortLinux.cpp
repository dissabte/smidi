/*!
 * \file MidiOutPortLinux.cpp
 * Contains platform independent MidiOutPortLinux implementation
 */

#include "MidiOutPortLinux.h"

#ifdef SMIDI_USE_ALSA
#include "alsa/MidiOutPortLinuxImpl.h"
#endif

MidiOutPortLinux::MidiOutPortLinux(std::unique_ptr<Implementation>&& implementation)
    : MidiOutPort()
    , _impl(std::move(implementation))
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
	_impl->start();
}

void MidiOutPortLinux::stop()
{
	_impl->stop();
}

void MidiOutPortLinux::sendMessage(const MidiMessage& message)
{
	if (_impl->isOpen())
	{
		_impl->sendMessage(message);
	}
}
