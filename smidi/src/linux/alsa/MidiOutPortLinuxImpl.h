#pragma once

//! \cond INTERNAL

/*!
 * \file MidiOutPortLinuxImpl.h
 * \warning This file is not a part of library public interface!
 */

#include "../MidiOutPortLinux.h"
#include "MidiEventEncoder.h"
#include "MidiSyncLinuxImpl.h"
#include <alsa/asoundlib.h>

class MidiOutPortLinux::Implementation
{
	const static std::size_t kInitialBufferSize = 256;

public:
	Implementation(const std::string& name, int clientId, int portId);
	~Implementation();

	const std::string& name() const;

	void open();
	void close();
	bool isOpen() const;

	void start();
	void stop();

	void sendMessage(const MidiMessage& message);

	MidiSync& sync();

	int applicationClientId() const;

public:
	snd_seq_t* sequencer() const;

private:
	std::string               _name;
	snd_seq_addr_t            _deviceAddress;
	snd_seq_addr_t            _applicationAddress;
	snd_seq_t*                _sequencer;
	snd_seq_port_subscribe_t* _subscription;
	MidiEventEncoder          _encoder;
	MidiSyncLinux             _sync;
	bool                      _isOpen;
};

//! \endcond
