#pragma once

/*!
 * \file MidiDeviceEnumerator.h
 */

#include <list>
#include <memory>

class MidiDevice;

/*!
 * \brief The device enumerator class
 * \class MidiDeviceEnumerator MidiDeviceEnumerator.h <smidi/MidiDeviceEnumerator.h>
 *
 * \note This class is not thread safe, so deviceNames() and createDevice() should not
 * be called in parrallel with updateDeviceList() without guard (e.g. mutex)
 */
class MidiDeviceEnumerator
{
public:

	//! Constructor
	MidiDeviceEnumerator();

	//! Destructor
	~MidiDeviceEnumerator();

	//! Returns the names of available MIDI devices
	std::list<std::string> deviceNames() const;

	/*!
	 * \brief Creates MidiDevice objects
	 * \param name the name of the device to create (one of returned by deviceName()).
	 * \return pointer to the MidiDevice object
	 * \sa deviceNames()
	 *
	 * Not only MidiDevice object is created but also MidiInPort and MidiOutPort object for
	 * each MIDI In and MIDI Out port respectively.
	 */
	std::shared_ptr<MidiDevice> createDevice(const std::string& name) const;

	/*!
	 * \brief  This method forces enumerator to re-enumerate the device list.
	 *
	 * With current implementatio all device object references that were created with createDevice() will be
	 * released internally. The holder of such references should also release them.
	 */
	void updateDeviceList();

private:
	class Implementation;
	std::unique_ptr<Implementation> _impl;
};
