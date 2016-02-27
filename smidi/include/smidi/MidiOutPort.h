#pragma once

/*!
 * \file MidiOutPort.h
 * Contains platfrom-independent part of MIDI output port implementation.
 */

#include "MidiPort.h"
#include "MidiMessage.h"
#include <memory>

/*!
 * \brief The MidiOutPort class is the interface for MIDI output ports.
 * \class MidiOutPort MidiOutPort.h <smidi/MidiOutPort.h>
 * \sa MidiInPort
 */

class MidiOutPort : public MidiPort
{
public:
	explicit MidiOutPort() = default;
	virtual ~MidiOutPort() = default;

	/*!
	 * \brief Sends MIDI message to the device (device driver)
	 * \param [in] message MIDI message to send
	 */
	virtual void sendMessage(const MidiMessage& message) = 0;
};

