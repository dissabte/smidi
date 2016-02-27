#pragma once

/*!
 * \file MidiOutPortLinuxImpl.h
 */

#include "../MidiOutPortLinux.h"
#include "MidiEventEncoder.h"
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

	void sendMessage(const MidiMessage& message);

	int applicationClientId() const;

private:
	std::string               _name;
	snd_seq_addr_t            _deviceAddress;
	snd_seq_addr_t            _applicationAddress;
	snd_seq_t*                _sequencer;
	snd_seq_port_subscribe_t* _subscription;
	MidiEventEncoder          _encoder;
	bool                      _isOpen;
};
