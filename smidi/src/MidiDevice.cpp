/*!
 * \file MidiDevice.cpp
 */

#include "../include/smidi/MidiDevice.h"
#include "../include/smidi/MidiInPort.h"
#include "../include/smidi/MidiOutPort.h"

MidiDevice::MidiDevice(const std::string& name, const MidiDevice::InputPortContainer& inputs, const MidiDevice::OutputPortContainer& outputs)
    : _name(name)
    , _inputPorts(inputs)
    , _outputPorts(outputs)
{
}

const std::string& MidiDevice::name() const
{
	return _name;
}

const MidiDevice::InputPortContainer& MidiDevice::inputPorts() const
{
	return _inputPorts;
}

const MidiDevice::OutputPortContainer& MidiDevice::outputPorts() const
{
	return _outputPorts;
}
