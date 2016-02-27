#pragma once

/*!
 * \file MidiInPortLinux.h
 * Contains platfrom-independent part of MIDI input port implementation.
 * \warning This file is not a part of library public interface!
 */

#include "../include/smidi/MidiInPort.h"
#include <memory>

/*!
 * \brief The MidiInPortLinux class
 * \class MidiInPortLinux MidiInPortLinux.h "MidiInPortLinux.h"
 * \warning This class is not a part of library public interface!
 */

class MidiInPortLinux : public MidiInPort
{
public:
	explicit MidiInPortLinux(const std::string& name, int clientId, int portId);
	virtual ~MidiInPortLinux();

	virtual const std::string& name() const override;

	virtual void start() override;
	virtual void stop() override;

	int applicationClientId() const;

private:
	class Implementation;
	std::unique_ptr<Implementation> _impl;
};