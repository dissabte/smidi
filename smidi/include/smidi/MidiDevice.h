#pragma once

/*!
 * \file MidiDevice.h
 * Contains MidiDevice implementation
 */

#include <vector>
#include <memory>

class MidiInPort;
class MidiOutPort;

/*!
 * \brief The MidiDevice class represents MIDI device with input and ouput MIDI ports.
 * \class MidiDevice MidiDevice.h <smidi/MidiDevice.h>
 */

class MidiDevice
{
public:
	//! type alias for MidiInPort container
	using InputPortContainer = std::vector<std::shared_ptr<MidiInPort>>;

	//! type alias for MidiOutPort container
	using OutputPortContainer = std::vector<std::shared_ptr<MidiOutPort>>;

public:
	/*!
	 * \brief Contructor
	 * \param name the name of the MIDI device.
	 * \param inputs the container with MidiInPorts of the device.
	 * \param outputs the container with MidiOutPorts of the device.
	 *
	 * This constructor is mainly used by MidiDeviceEnumerator.
	 */
	explicit MidiDevice(const std::string& name, const InputPortContainer& inputs, const OutputPortContainer& outputs);

	//! Trivial destructor
	virtual ~MidiDevice() = default;

	//! Returns device name
	const std::string& name() const;

	//! Returns immutable reference to the input MidiPort container
	const InputPortContainer& inputPorts() const;

	//! Returns immutable reference to the input MidiPort container
	const OutputPortContainer& outputPorts() const;

private:
	std::string         _name;
	InputPortContainer  _inputPorts;
	OutputPortContainer _outputPorts;
};
