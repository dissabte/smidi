//! \cond INTERNAL

/*!
 * \file MidiOutPortLinuxImpl.cpp
 */

#include "MidiOutPortLinuxImpl.h"
#include "MidiAlsaConstants.h"
#include "../../../include/smidi/MidiMessage.h"
#include <iostream>

MidiOutPortLinux::Implementation::Implementation(const std::string& name, int clientId, int portId)
    : _name(name)
    , _deviceAddress{static_cast<unsigned char>(clientId), static_cast<unsigned char>(portId)}
    , _applicationAddress{static_cast<unsigned char>(MidiAlsaConstants::kInvalidId), static_cast<unsigned char>(MidiAlsaConstants::kInvalidId)}
    , _sequencer(nullptr)
    , _subscription(nullptr)
    , _encoder(kInitialBufferSize)
    , _sync(std::unique_ptr<MidiSyncLinux::Implementation>(new MidiSyncLinux::Implementation(*this)))
    , _isOpen(false)
{
	// open ALSA sequencer client
	if (MidiAlsaConstants::kNoError == snd_seq_open(&_sequencer, "default", SND_SEQ_OPEN_OUTPUT, SND_SEQ_NONBLOCK))
	{
		// set the name for the sequencer client
		snd_seq_set_client_name(_sequencer, name.c_str());

		_applicationAddress.client = snd_seq_client_id(_sequencer);
	}
	else
	{
		std::cerr << "Couldn't open ALSA sequencer for " << _name.c_str() << std::endl;
	}
}

MidiOutPortLinux::Implementation::~Implementation()
{
	close();
	if (_sequencer)
	{
		snd_seq_close(_sequencer);
	}
}

const std::string& MidiOutPortLinux::Implementation::name() const
{
	return _name;
}

void MidiOutPortLinux::Implementation::open()
{
	if (!_isOpen)
	{
		const unsigned int capabilities = SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ;
		const unsigned int type = SND_SEQ_PORT_TYPE_MIDI_GENERIC|SND_SEQ_PORT_TYPE_APPLICATION;
		_applicationAddress.port = snd_seq_create_simple_port(_sequencer, _name.c_str(), capabilities, type);
		if (_applicationAddress.port >= 0)
		{
			snd_seq_port_info_t *portInfo = nullptr;
			snd_seq_port_info_alloca(&portInfo);
			snd_seq_get_any_port_info(_sequencer, _deviceAddress.client, _deviceAddress.port, portInfo);

			if (MidiAlsaConstants::kNoError == snd_seq_port_subscribe_malloc(&_subscription))
			{
				snd_seq_port_subscribe_set_sender(_subscription, &_applicationAddress);
				snd_seq_port_subscribe_set_dest(_subscription, &_deviceAddress);
				snd_seq_port_subscribe_set_time_real(_subscription, 1);
				snd_seq_port_subscribe_set_time_update(_subscription, 1);
				if (MidiAlsaConstants::kNoError == snd_seq_subscribe_port(_sequencer, _subscription))
				{
					_isOpen = true;
				}
				else
				{
					std::cerr << "Couldn't allocate space for port subscription for " << _name.c_str() << std::endl;
					snd_seq_port_subscribe_free(_subscription);
				}
			}
			else
			{
				std::cerr << "Couldn't allocate space for port subscription for " << _name.c_str() << std::endl;
				snd_seq_port_subscribe_free(_subscription);
			}
		}
		else
		{
			std::cerr << "Couldn't create MIDI input port for " << _name.c_str() << std::endl;
		}
	}
	else
	{
		std::cerr << "The port is already opened for " << _name.c_str() << std::endl;
	}
}

void MidiOutPortLinux::Implementation::close()
{
	if (_isOpen)
	{
		snd_seq_unsubscribe_port(_sequencer, _subscription);
		snd_seq_port_subscribe_free(_subscription);

		_isOpen = false;
	}
	else
	{
		std::cerr << "The port isn't' opened for " << _name.c_str() << std::endl;
	}
}

bool MidiOutPortLinux::Implementation::isOpen() const
{
	return _isOpen;
}

void MidiOutPortLinux::Implementation::start()
{
}

void MidiOutPortLinux::Implementation::stop()
{
}

void MidiOutPortLinux::Implementation::sendMessage(const MidiMessage& message)
{
	snd_seq_event_t event = {};
	if (_encoder.encode(&event, message))
	{
		snd_seq_ev_set_source(&event, _applicationAddress.port);
		snd_seq_ev_set_subs(&event);
		snd_seq_ev_set_direct(&event);

		const int numberOfUnprocessedEventsOrError = snd_seq_event_output(_sequencer, &event);
		if (numberOfUnprocessedEventsOrError >= 0)
		{
			snd_seq_drain_output(_sequencer);
		}
		else
		{
			const int error = numberOfUnprocessedEventsOrError;
			std::cerr << "Couldn't send MIDI message: " << message.toString() << " for " << _name.c_str() << " because: " << snd_strerror(error) << std::endl;
		}
	}
}

MidiSync& MidiOutPortLinux::Implementation::sync()
{
	return _sync;
}

int MidiOutPortLinux::Implementation::applicationClientId() const
{
	return _applicationAddress.client;
}

snd_seq_t*MidiOutPortLinux::Implementation::sequencer() const
{
	return _sequencer;
}

//! \endcond
