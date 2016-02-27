#pragma once

/*!
 * \file MidiDevice.h
 * Contains MidiDevice implementation
 */

#include "MidiPort.h"
#include <vector>
#include <memory>

class MidiDevice
{
public:
	using PortContainer = std::vector<std::shared_ptr<MidiPort>>;

public:
	explicit MidiDevice(const std::string& name, const PortContainer& inputs, const PortContainer& outputs)
	    : _name(name)
	    , _inputPorts(inputs)
	    , _outputPorts(outputs)
	{
	}

	virtual ~MidiDevice() {}

	const std::string& name() const { return _name; }

	const PortContainer& inputPorts() const { return _inputPorts; }
	const PortContainer& outputPorts() const { return _outputPorts; }

private:
	std::string _name;
	PortContainer _inputPorts;
	PortContainer _outputPorts;
};
