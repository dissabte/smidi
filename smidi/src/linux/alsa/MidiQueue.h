#pragma once

#include <alsa/asoundlib.h>
#include <string>

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

	void start();
	void stop();
	void resume();

	void setTempo(double bpm);
	void changeTempo(double bpm);

	bool isValid() const;
	operator int() const;

private:
	unsigned int convertBPMToMicroseconds(double bpm) const;

private:
	snd_seq_t* _sequencer;
	int        _id;
};
