#pragma once

/*!
 * \file MidiOutPortLinux.h
 * Contains platfrom-independent part of MIDI output port implementation.
 * \warning This file is not a part of library public interface!
 */

#include "../../include/smidi/MidiOutPort.h"
#include <memory>

/*!
 * \brief The MidiOutnPortLinux class
 * \class MidiOutnPortLinux MidiOutnPortLinux.h "MidiOutnPortLinux.h"
 * \warning This class is not a part of library public interface!
 */

class MidiOutPortLinux : public MidiOutPort
{
public:
	explicit MidiOutPortLinux(const std::string& name, int clientId, int portId);
	virtual ~MidiOutPortLinux();

	virtual const std::string& name() const override;

	virtual void start() override;
	virtual void stop() override;

	virtual void sendMessage(const MidiMessage& message) override;

	int applicationClientId() const;

private:
	class Implementation;
	std::unique_ptr<Implementation> _impl;
};

