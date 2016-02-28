#pragma once

//! \cond INTERNAL

/*!
 * \file MidiInPortLinuxImpl.h
 * \warning This file is not a part of library public interface!
 */

#include "../MidiInPortLinux.h"
#include "MidiQueue.h"
#include <thread>
#include <alsa/asoundlib.h>

class MidiMessage;

class MidiInPortLinux::Implementation
{
	static const int kDefaultPPQN = 240;
	static const int kDefaultTempo = 500000;

	static const int kReadCaps = SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ;
	static const int kWriteCaps = SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE;

public:
	using Callback = std::function<void(MidiMessage&)>;

public:
	Implementation(const std::string& name, int clientId, int portId);
	~Implementation();

	const std::string& name() const;

	void open();
	void close();
	bool isOpen() const;

	void setCallback(Callback callback);

	void start();
	void stop();

	int applicationClientId() const;

private:
	void midiInputThread();

private:
	std::string                _name;
	Callback                   _callback;
	snd_seq_t*                 _sequencer;
	snd_seq_addr_t             _deviceAddress;
	snd_seq_addr_t             _applicationAddress;
	snd_seq_port_subscribe_t*  _subscription;
	snd_midi_event_t*          _parser;
	std::thread                _thread;
	MidiQueue                  _queue;
	int                        _pipefd[2];
	bool                       _isOpen;
	bool                       _poll;
};

//! \endcond
