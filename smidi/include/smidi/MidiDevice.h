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
	explicit MidiDevice(const std::string& name, const PortContainer& inputs, const PortContainer& outputs);

	virtual ~MidiDevice();

	const std::string& name() const;

	const PortContainer& inputPorts() const;
	const PortContainer& outputPorts() const;

private:
	std::string   _name;
	PortContainer _inputPorts;
	PortContainer _outputPorts;
};
