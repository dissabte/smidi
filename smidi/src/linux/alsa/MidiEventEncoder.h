#pragma once

#include "../../../include/smidi/MidiMessage.h"
#include <alsa/asoundlib.h>

class MidiEventEncoder
{
public:
	MidiEventEncoder() = delete;
	MidiEventEncoder(int initialBufferSize);
	~MidiEventEncoder();

	int bufferSize() const;

	operator snd_midi_event_t*();

	void resizeBuffer(int newBufferSize);
	void setRunningStatusEnabled(bool enabled);

	bool decode(snd_seq_event_t* event, MidiMessage& message);
	bool encode(snd_seq_event_t* event, const MidiMessage& message);

private:
	snd_midi_event_t* _parser;
	int               _bufferSize;
};
