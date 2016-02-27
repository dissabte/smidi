/*!
 * \file MidiDeviceEnumerator.cpp
 */

#include "../include/smidi/MidiDeviceEnumerator.h"

#ifdef __linux__

#ifdef SMIDI_USE_ALSA
#include "linux/alsa/MidiDeviceEnumeratorImpl.h"
#endif

#endif


MidiDeviceEnumerator::MidiDeviceEnumerator()
    : _impl(new Implementation())
{
}

MidiDeviceEnumerator::~MidiDeviceEnumerator()
{
}

std::list<std::string> MidiDeviceEnumerator::deviceNames()
{
	return _impl->deviceNames();
}

std::shared_ptr<MidiDevice> MidiDeviceEnumerator::createDevice(const std::string& name)
{
	return _impl->createDevice(name);
}
