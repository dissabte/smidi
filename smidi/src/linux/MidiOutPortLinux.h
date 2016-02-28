#pragma once

//! \cond INTERNAL

/*!
 * \file MidiOutPortLinux.h
 * \warning This file is not a part of library public interface!
 *
 * Contains platfrom-independent part of MIDI output port implementation.
 */

#include "../../include/smidi/MidiOutPort.h"
#include "MidiSyncLinux.h"
#include <memory>

/*!
 * \brief The MidiOutnPortLinux class
 * \class MidiOutnPortLinux MidiOutnPortLinux.h "MidiOutnPortLinux.h"
 * \warning This class is not a part of library public interface!
 */

class MidiOutPortLinux : public MidiOutPort
{
public:
	class Implementation;

	explicit MidiOutPortLinux(std::unique_ptr<Implementation>&& implementation);
	virtual ~MidiOutPortLinux();

	virtual const std::string& name() const override;

	virtual void start() override;
	virtual void stop() override;

	virtual void sendMessage(const MidiMessage& message) override;

	virtual MidiSync& sync() override;

private:
	std::unique_ptr<Implementation> _impl;
};

//! \endcond
