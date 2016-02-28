#pragma once

//! \cond INTERNAL

/*!
 * \file MidiInPortLinux.h
 * \warning This file is not a part of library public interface!
 *
 * Contains platfrom-independent part of MIDI input port implementation.
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
	//! Forward declaration of internal implementation class
	class Implementation;

	/*!
	 * \brief Constructor
	 * \param implementation actual MidiInPort implementation (e.g. ALSA implementation on Linux)
	 */
	explicit MidiInPortLinux(std::unique_ptr<Implementation>&& implementation);

	//! Trivial destructor
	virtual ~MidiInPortLinux() = default;

	virtual const std::string& name() const override;

	virtual void start() override;
	virtual void stop() override;

private:
	std::unique_ptr<Implementation> _impl;
};

//! \endcond
