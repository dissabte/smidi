#pragma once

/*!
 * \file MidiDeviceEnumerator
 */

#include "MidiDevice.h"
#include <list>
#include <memory>

class MidiDeviceEnumerator
{
public:
	MidiDeviceEnumerator();
	~MidiDeviceEnumerator();

	std::list<std::string> deviceNames() const;
	std::shared_ptr<MidiDevice> createDevice(const std::string& name) const;

	void updateDeviceList();

private:
	class Implementation;
	std::unique_ptr<Implementation> _impl;
};
