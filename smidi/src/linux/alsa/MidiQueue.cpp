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
