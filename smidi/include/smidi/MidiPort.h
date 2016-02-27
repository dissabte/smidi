#pragma once

/*!
 * \file MidiPort.h
 * Contains interface for MIDI ports classes
 */

#include <string>


/*!
 * \brief The MidiPort class represents interface for the single MIDI port either input or output.
 * \class MidiPort MidiPort.h <smidi/MidiPort.h>
 * \sa MidiDevice
 */

class MidiPort
{
public:
	/*!
	 * \brief Сonstructor
	 * \param name the device dependent name of this MIDI port.
	 */
	explicit MidiPort() {}
	virtual ~MidiPort() {}

	//! Returns the reference to the name of this port (device driver dependent)
	virtual const std::string& name() const = 0;

	/*! \brief Starts the device.
	 *
	 *  In case of input device - starts listening for incoming MIDI events.
	 *  In case of output device - possible makes preparation related to MIDI messages ouput.
	 *  Started device is able to send (in case of output) or receive (in case of input) MIDI messages.
	 */
	virtual void start() = 0;

	/*! \brief Stops the device.
	 *
	 * In case of input device - stops listening for incoming MIDI events.
	 * In case of output device - possibly releases some resources related to MIDI messages output.
	 * Stopped device can't send or receive MIDI.
	 */
	virtual void stop() = 0;
};
