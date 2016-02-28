/*!
 * \file MidiInPortLinuxImpl.cpp
 * \warning This file is not a part of library public interface!
 */

#include "MidiInPortLinuxImpl.h"
#include "MidiEventEncoder.h"
#include "MidiAlsaConstants.h"
#include <functional>
#include <iostream>
#include <unistd.h>
#include <alsa/asoundlib.h>

MidiInPortLinux::Implementation::Implementation(const std::string& name, int clientId, int portId)
	: _name(name)
	, _sequencer(nullptr)
	, _deviceAddress{static_cast<unsigned char>(clientId), static_cast<unsigned char>(portId)}
	, _applicationAddress{static_cast<unsigned char>(MidiAlsaConstants::kInvalidId), static_cast<unsigned char>(MidiAlsaConstants::kInvalidId)}
	, _subscription(nullptr)
	, _parser(nullptr)
	, _pipefd{MidiAlsaConstants::kInvalidId, MidiAlsaConstants::kInvalidId}
	, _isOpen(false)
{
	// open ALSA sequencer client
	if (MidiAlsaConstants::kNoError == snd_seq_open(&_sequencer, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK))
	{
		// set the name for the sequencer client
		snd_seq_set_client_name(_sequencer, name.c_str());

		_applicationAddress.client = snd_seq_client_id(_sequencer);

		// create pipe
		if (MidiAlsaConstants::kNoError == pipe(_pipefd))
		{
			// create input queue
			_queue.init(_sequencer, name + " Input Queue");
			_queue.setTempo(1200.0); // some random high tempo
		}
		else
		{
			std::cerr << "Couldn't create pipe for " << _name.c_str() << std::endl;
		}
	}
	else
	{
		std::cerr << "Couldn't open ALSA sequencer for " << _name.c_str() << std::endl;
	}
}

MidiInPortLinux::Implementation::~Implementation()
{
	close();

	// close pipe
	if (_pipefd[0] != MidiAlsaConstants::kInvalidId)
	{
		::close(_pipefd[0]);
	}
	if (_pipefd[1] != MidiAlsaConstants::kInvalidId)
	{
		::close(_pipefd[1]);
	}

	if (_sequencer)
	{
		snd_seq_close(_sequencer);
	}
}

const std::string& MidiInPortLinux::Implementation::name() const
{
	return _name;
}

void MidiInPortLinux::Implementation::open()
{
	if(!_isOpen)
	{
		// get port info
		snd_seq_port_info_t* sourcePortInfo = nullptr;
		snd_seq_port_info_alloca(&sourcePortInfo);
		snd_seq_get_any_port_info(_sequencer, _deviceAddress.client, _deviceAddress.port, sourcePortInfo);

		// check if we can read from this port
		int caps = snd_seq_port_info_get_capability(sourcePortInfo);
		if ((caps & kReadCaps) == kReadCaps)
		{
			// create input port
			snd_seq_port_info_t* destinationPortInfo;
			snd_seq_port_info_alloca(&destinationPortInfo);

			snd_seq_port_info_set_client(destinationPortInfo, 0);
			snd_seq_port_info_set_port(destinationPortInfo, 0);
			snd_seq_port_info_set_capability(destinationPortInfo, kWriteCaps);
			snd_seq_port_info_set_type(destinationPortInfo, SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);
			snd_seq_port_info_set_midi_channels(destinationPortInfo, 16);

			snd_seq_port_info_set_timestamping(destinationPortInfo, 1);
			snd_seq_port_info_set_timestamp_real(destinationPortInfo, 1);
			snd_seq_port_info_set_timestamp_queue(destinationPortInfo, _queue);

			snd_seq_port_info_set_name(destinationPortInfo, _name.c_str());
			int result = snd_seq_create_port(_sequencer, destinationPortInfo);
			if (MidiAlsaConstants::kNoError == result)
			{
				_applicationAddress.port = snd_seq_port_info_get_port(destinationPortInfo);

				// create subscription
				result = snd_seq_port_subscribe_malloc(&_subscription);
				if (MidiAlsaConstants::kNoError == result)
				{
					snd_seq_port_subscribe_set_sender(_subscription, &_deviceAddress);
					snd_seq_port_subscribe_set_dest(_subscription, &_applicationAddress);
					result = snd_seq_subscribe_port(_sequencer, _subscription);
					if (MidiAlsaConstants::kNoError == result)
					{
						// start the input queue
						_queue.start();

						// set the flag to poll input event
						_poll = true;

						// start MIDI input thread.
						_thread = std::thread(&Implementation::midiInputThread, this);
						if (_thread.joinable())
						{
							_isOpen = true;
						}
						else
						{
							std::cerr << "Couldn't start midi input thread: " << _name.c_str() << std::endl;
							snd_seq_unsubscribe_port(_sequencer, _subscription);
							snd_seq_port_subscribe_free(_subscription);
						}
					}
					else
					{
						std::cerr << "Couldn't subscribe port: " << _name.c_str() << " because: " << snd_strerror(result) << std::endl;
						snd_seq_port_subscribe_free(_subscription);
						_subscription = nullptr;
					}
				}
				else
				{
					std::cerr << "Couldn't allocate port subscription: " << _name.c_str() << " because: " << snd_strerror(result) << std::endl;
				}
			}
			else
			{
				std::cerr << "Couldn't create input port: " << _name.c_str() << " because: " << snd_strerror(result) << std::endl;
			}
		}
		else
		{
			std::cerr << "Invalid client id/port id specified: " << _name.c_str() << ", client id: " << _deviceAddress.client << ", port id: " << _deviceAddress.port << std::endl;
		}
	}
	else
	{
		std::cerr << "Already open: " << _name.c_str() << std::endl;
	}
}


bool MidiInPortLinux::Implementation::isOpen() const
{
	return _isOpen;
}

void MidiInPortLinux::Implementation::close()
{
	if (_isOpen)
	{
		// remove subscription
		snd_seq_unsubscribe_port(_sequencer, _subscription);
		snd_seq_port_subscribe_free(_subscription);
		_subscription = nullptr;

		// stop queue
		_queue.stop();

		// stop the thread
		if (_poll)
		{
			_poll = false;
			unsigned char stop = 1;
			::write(_pipefd[1], &stop, sizeof(stop));
			_thread.join();
		}

		// destroy port
		snd_seq_delete_port(_sequencer, _applicationAddress.port);
		_applicationAddress.port = MidiAlsaConstants::kInvalidId;

		_isOpen = false;
	}
}

void MidiInPortLinux::Implementation::setCallback(MidiInPortLinux::Implementation::Callback callback)
{
	_callback = callback;
}

void MidiInPortLinux::Implementation::start()
{
}

void MidiInPortLinux::Implementation::stop()
{
}

int MidiInPortLinux::Implementation::applicationClientId() const
{
	return _applicationAddress.client;
}

void MidiInPortLinux::Implementation::midiInputThread()
{
	Implementation* impl = this;

	MidiEventEncoder encoder(0);
	encoder.setRunningStatusEnabled(false);

	// setup poll descriptors
	const int pollDescriptorsCount = snd_seq_poll_descriptors_count(impl->_sequencer, POLLIN);
	// note: we add 1 custom descriptor to force the poll() call to return
	pollfd* pollDescriptors = reinterpret_cast<pollfd*>(alloca((pollDescriptorsCount + 1) * sizeof(pollfd)));
	snd_seq_poll_descriptors(impl->_sequencer, pollDescriptors, pollDescriptorsCount, POLLIN);
	pollDescriptors[pollDescriptorsCount].fd = impl->_pipefd[0];
	pollDescriptors[pollDescriptorsCount].events = POLLIN;

	MidiMessage message;

	while (impl->_poll)
	{
		const int checkSequencerFIFO = 1;
		const int numberOfEventPending = snd_seq_event_input_pending(impl->_sequencer, checkSequencerFIFO);
		if (numberOfEventPending == 0)
		{
			if (poll(pollDescriptors, pollDescriptorsCount + 1, -1) >= 0)
			{
				// check if the polled one is our custom descriptur
				const bool shouldCheckPollingStatus = (pollDescriptors[pollDescriptorsCount].revents & POLLIN) == POLLIN;
				if (shouldCheckPollingStatus)
				{
					unsigned char data;
					::read(pollDescriptors[pollDescriptorsCount].fd, &data, sizeof(data));
				}
			}
			continue;
		}

		snd_seq_event_t* event = nullptr;
		const int resultOrError = snd_seq_event_input(impl->_sequencer, &event);
		if (resultOrError >= MidiAlsaConstants::kNoError)
		{
			MidiMessage decodedMessage;
			encoder.decode(event, decodedMessage);
			snd_seq_free_event(event);

			message += decodedMessage;
		}
		else
		{
			const int error = resultOrError;
			std::cerr << "Couldn't read midi event with: " << impl->_name.c_str() << " because: " << snd_strerror(error) << std::endl;
		}

		const bool partialSysEx = message.isActually(MidiMessage::SysEx) && !message.isCompleteSysEx();
		if (!partialSysEx && !message.isEmpty())
		{
			if (impl->_callback)
			{
				impl->_callback(message);
			}
		}
	}
}
