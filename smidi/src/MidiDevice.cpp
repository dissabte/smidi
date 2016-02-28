#include "../include/smidi/MidiDevice.h"

MidiDevice::MidiDevice(const std::string& name, const MidiDevice::PortContainer& inputs, const MidiDevice::PortContainer& outputs)
    : _name(name)
    , _inputPorts(inputs)
    , _outputPorts(outputs)
{
}

MidiDevice::~MidiDevice()
{
}

const std::string& MidiDevice::name() const
{
	return _name;
}

const MidiDevice::PortContainer&MidiDevice::inputPorts() const
{
	return _inputPorts;
}

const MidiDevice::PortContainer&MidiDevice::outputPorts() const
{
	return _outputPorts;
}
