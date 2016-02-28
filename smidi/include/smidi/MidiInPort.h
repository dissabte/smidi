#pragma once

/*!
 * \file MidiInPort.h
 */

#include "MidiPort.h"

/*!
 * \brief The MidiInPort class is the interface for the MIDI input ports
 * \class MidiInPort MidiInPort.h <smidi/MidiInPort.h>
 * \sa MidiOutPort
 */

class MidiInPort : public MidiPort
{
public:
	//! Trivial constructor
	explicit MidiInPort() = default;

	//! Trivial destructor
	virtual ~MidiInPort() = default;
};
