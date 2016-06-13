#pragma once

//! \cond INTERNAL

/*!
 * \file MidiQueue.h
 * \warning This file is not part of the library public interface!
 * Contains ASLA queue wrapper
 */

#include <alsa/asoundlib.h>
#include <string>

/*!
 * \brief The MidiQueue class
 * \class MidiQueue MidiQueue.h "MidiQueue.h"
 * \warning This class is not part of the library public interface!
 */

class MidiQueue
{
	constexpr static int kInvalidId = -1;
	constexpr static int kPPQN = 24;
	constexpr static double kMinimalBPM = 1.0;

public:
	MidiQueue();
	~MidiQueue();

	void init(snd_seq_t* sequencer);
	void init(snd_seq_t* sequencer, const std::string& name);

	void close();

	void start();
	void stop();
	void resume();

	void setTempo(double bpm);
	void changeTempo(double bpm);

	bool isValid() const;
	operator int() const;

	void enqueueMidiMessage(const snd_seq_event_type messageType, const int sourcePort, const snd_seq_tick_time_t& tick);
	void enqueueMidiSyncEvents(const int sourcePort, const bool includeMidiStart, const bool includeSongPositionReset, const unsigned int numberOfMidiClocks);
	unsigned int timeToSendInMicroseconds(const unsigned int numberOfMessages, const unsigned int deltaTimeInTicks, const double bpm) const;

private:
	unsigned int convertBPMToMicroseconds(double bpm) const;

private:
	snd_seq_t* _sequencer;
	int        _id;
};

//! \endcond
