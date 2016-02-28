//! \cond INTERNAL

/*!
 * \file MidiEventEncoder.cpp
 */

#include "MidiEventEncoder.h"
#include "MidiAlsaConstants.h"
#include <sys/time.h>
#include <iostream>

MidiEventEncoder::MidiEventEncoder(int initialBufferSize)
	: _parser(nullptr)
	, _bufferSize(initialBufferSize)
{
	int error = snd_midi_event_new(_bufferSize, &_parser);
	if (MidiAlsaConstants::kNoError == error)
	{
		snd_midi_event_init(_parser);
	}
	else
	{
		std::cerr << "Couldn't create MIDI event parser with size " << initialBufferSize << " because: " << snd_strerror(error) << std::endl;
	}
}

MidiEventEncoder::~MidiEventEncoder()
{
	if (_parser)
	{
		snd_midi_event_free(_parser);
	}
}

int MidiEventEncoder::bufferSize() const
{
	return _bufferSize;
}

void MidiEventEncoder::resizeBuffer(int newBufferSize)
{
	if (newBufferSize != _bufferSize)
	{
		int error = snd_midi_event_resize_buffer(_parser, newBufferSize);
		if (MidiAlsaConstants::kNoError != error)
		{
			std::cerr << "Couldn't resize MIDI event parser size from " << _bufferSize << " to " << newBufferSize << " because: " << snd_strerror(error) << std::endl;
		}
	}
}

void MidiEventEncoder::setRunningStatusEnabled(bool enabled)
{
	snd_midi_event_no_status(_parser, enabled ? 0 : 1);
}

MidiEventEncoder::operator snd_midi_event_t*()
{
	return _parser;
}

bool MidiEventEncoder::decode(snd_seq_event_t* event, MidiMessage& message)
{
	bool result = false;
	if (event->type == SND_SEQ_EVENT_PORT_SUBSCRIBED)
	{
		const int sourceClient = event->data.connect.sender.client;
		const int sourcePort = event->data.connect.sender.port;
		const int destClient = event->data.connect.dest.client;
		const int destPort = event->data.connect.dest.port;
		std::cout << "Port subscribed: (" << sourceClient << ":" << sourcePort << ") -> (" << destClient << ":" << destPort << ")\n";
	}
	else if (event->type == SND_SEQ_EVENT_PORT_UNSUBSCRIBED)
	{
		const int sourceClient = event->data.connect.sender.client;
		const int sourcePort = event->data.connect.sender.port;
		const int destClient = event->data.connect.dest.client;
		const int destPort = event->data.connect.dest.port;
		std::cout << "Port unsubscribed: (" << sourceClient << ":" << sourcePort << ") -> (" << destClient << ":" << destPort << ")\n";
	}
	else
	{
		// check buffer space requirements
		int bufferSizeRequired = 3; // this is the max size of non-SysEx MIDI message
		if (event->type == SND_SEQ_EVENT_SYSEX)
		{
			bufferSizeRequired = event->data.ext.len;
		}
		if (message.size() < bufferSizeRequired)
		{
			message.resizeBuffer(bufferSizeRequired);
		}

		// decode
		long numberOfBytes = snd_midi_event_decode(_parser, message, message.size(), event);
		if (numberOfBytes > 0)
		{
			// timestamp
			timeval currentSystemTime = {};
			gettimeofday(&currentSystemTime, nullptr);
			const unsigned long long timeStampInMs = (currentSystemTime.tv_sec * 1000) + (currentSystemTime.tv_usec / 1000);
			message.setTimestamp(timeStampInMs);
			result = true;
		}
		else
		{
			perror("Event decoding failed");
		}
	}
	return result;
}

bool MidiEventEncoder::encode(snd_seq_event_t* event, const MidiMessage& message)
{
	bool result = false;

	if (message.size() > bufferSize())
	{
		resizeBuffer(message.size());
	}

	snd_seq_ev_clear(event);

	int bytesEncoded = snd_midi_event_encode(_parser, message, message.size(), event);
	if (bytesEncoded == message.size())
	{
		result = true;
	}
	else
	{
		std::cerr << "Couldn't parse outgoing MIDI message: " << message.toString() << " (number of encoded bytes is " << bytesEncoded << ")\n";
	}
	return result;
}

//! \endcond
