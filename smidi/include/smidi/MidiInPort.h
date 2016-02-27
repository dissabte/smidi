#pragma once

/*!
 * \file MidiInPort.h
 */

#include "MidiPort.h"
#include <string>

/*!
 * \brief The MidiInPort class
 * \class MidiInPort MidiInPort.h <smidi/MidiInPort.h>
 * \sa MidiOutPort
 */

class MidiInPort : public MidiPort
{
public:
	explicit MidiInPort() = default;
	virtual ~MidiInPort() = default;
};
