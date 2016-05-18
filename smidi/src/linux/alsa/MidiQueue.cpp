//! \cond INTERNAL

/*!
 * \file MidiQueue.cpp
 * \warning This file is not a part of library public interface!
 */

#include "MidiQueue.h"
#include <iostream>

MidiQueue::MidiQueue()
	: _sequencer(nullptr)
	, _id(kInvalidId)
{
}

MidiQueue::~MidiQueue()
{
	if (_id != kInvalidId)
	{
		snd_seq_free_queue(_sequencer, _id);
	}
}

void MidiQueue::init(snd_seq_t* sequencer)
{
	if (_id == kInvalidId)
	{
		_sequencer = sequencer;
		_id = snd_seq_alloc_queue(sequencer);
	}
	else
	{
		std::cerr << "Queue is already initialized\n";
	}
}

void MidiQueue::init(snd_seq_t* sequencer, const std::string& name)
{
	if (_id == kInvalidId)
	{
		_sequencer = sequencer;
		_id = snd_seq_alloc_named_queue(sequencer, name.c_str());
	}
	else
	{
		std::cerr << "Queue is already initialized\n";
	}
}

void MidiQueue::start()
{
	snd_seq_start_queue(_sequencer, _id, nullptr);
	snd_seq_drain_output(_sequencer);
}

void MidiQueue::stop()
{
	snd_seq_stop_queue(_sequencer, _id, nullptr);
	snd_seq_drain_output(_sequencer);
}

void MidiQueue::resume()
{
	snd_seq_continue_queue(_sequencer, _id, nullptr);
	snd_seq_drain_output(_sequencer);
}

void MidiQueue::setTempo(double bpm)
{
	const unsigned int tempo = convertBPMToMicroseconds(bpm);
	snd_seq_queue_tempo_t* queueTempo = nullptr;
	snd_seq_queue_tempo_alloca(&queueTempo);
	snd_seq_queue_tempo_set_tempo(queueTempo, tempo);
	snd_seq_queue_tempo_set_ppq(queueTempo, kPPQN);
	snd_seq_set_queue_tempo(_sequencer, _id, queueTempo);
	snd_seq_drain_output(_sequencer);
}

void MidiQueue::changeTempo(double bpm)
{
	const unsigned int tempo = convertBPMToMicroseconds(bpm);
	snd_seq_change_queue_tempo(_sequencer, _id, tempo, NULL);
	snd_seq_drain_output(_sequencer);
}

MidiQueue::operator int() const
{
	return _id;
}

void MidiQueue::enqueueMidiMessage(const snd_seq_event_type messageType, const int sourcePort, const snd_seq_tick_time_t& tick)
{
	snd_seq_event_t event = {};
	event.type = messageType;
	snd_seq_ev_schedule_tick(&event, _id, 1, tick);
	snd_seq_ev_set_source(&event, sourcePort);
	snd_seq_ev_set_subs(&event);
	snd_seq_event_output_direct(_sequencer, &event);
}

void MidiQueue::enqueueMidiSyncEvents(const int sourcePort, bool includeMidiStart, bool includeSongPositionReset, unsigned int numberOfMidiClocks)
{
	snd_seq_tick_time_t tick = 0;
	if (includeMidiStart)
	{
		enqueueMidiMessage(SND_SEQ_EVENT_START, sourcePort, tick);
		++tick;
	}

	if (includeSongPositionReset)
	{
		enqueueMidiMessage(SND_SEQ_EVENT_SONGPOS, sourcePort, tick);
		++tick;
	}

	for (unsigned int eventIndex = 0; eventIndex < numberOfMidiClocks; ++eventIndex)
	{
		enqueueMidiMessage(SND_SEQ_EVENT_CLOCK, sourcePort, tick);
		++tick;
	}
	snd_seq_drain_output(_sequencer);
}

unsigned int MidiQueue::timeToSendInMicroseconds(const unsigned int numberOfMessages, const unsigned int deltaTimeInTicks, const double bpm) const
{
	const unsigned int microsecondsPerQuarterNote = convertBPMToMicroseconds(bpm);
	const unsigned int microsecondsPerTick = microsecondsPerQuarterNote / kPPQN;
	const unsigned int ticks = numberOfMessages * deltaTimeInTicks;
	const unsigned int timeToSend = ticks * microsecondsPerTick;
	return timeToSend;
}

unsigned int MidiQueue::convertBPMToMicroseconds(double bpm) const
{
	if (bpm < 1.0)
	{
		bpm = 1.0;
	}
	const double microsecondsPerQuarterNote = 6e7 / bpm;
	return static_cast<unsigned int>(microsecondsPerQuarterNote);
}

//! \endcond
