#pragma once

#include <vector>

class MidiMessage
{
public:
	static const unsigned char kNoteOff         = 0x80;
	static const unsigned char kNoteOn          = 0x90;
	static const unsigned char kAfterTouch      = 0xA0;
	static const unsigned char kControlChange   = 0xB0;
	static const unsigned char kProgramChange   = 0xC0;
	static const unsigned char kChannelPressure = 0xD0;
	static const unsigned char kPitchWheel      = 0xE0;

	static const unsigned char kSystem          = 0xF0; // i.e. one of the following
	static const unsigned char kSysEx           = 0xF0;
	static const unsigned char kMTCQuarter      = 0xF1;
	static const unsigned char kSongPosition    = 0xF2;
	static const unsigned char kSongSelect      = 0xF3;
	static const unsigned char kTuneRequest     = 0xF6;

	static const unsigned char kMidiClock       = 0xF8;
	static const unsigned char kMidiStart       = 0xFA;
	static const unsigned char kMidiContinue    = 0xFB;
	static const unsigned char kMidiStop        = 0xFC;
	static const unsigned char kActiveSense     = 0xFE;
	static const unsigned char kReset           = 0xFF;

	using data_type = std::vector<unsigned char>;
	using size_type = data_type::size_type;

public:
	MidiMessage();
	MidiMessage(const MidiMessage&) = default;
	MidiMessage(MidiMessage&&) = default;

	MidiMessage(const std::vector<unsigned char> newData, unsigned long long newTimestamp = 0);
	MidiMessage(const unsigned char* newData, size_type newSize, unsigned long long newTimestamp = 0);

	~MidiMessage(); // non-virtual!

public:
	bool isEmpty() const;
	bool isActually(unsigned char noteType) const;
	bool isCompleteSysEx() const;

	unsigned char channel() const;

	unsigned long long timestamp() const;
	void setTimestamp(unsigned long long newTimestamp);

	size_type size() const;
	const data_type& data() const;

	operator const unsigned char*() const;
	operator const char*() const;

	MidiMessage operator+(const MidiMessage& other);
	void operator +=(const MidiMessage& other);

	void resizeBuffer(size_type newSize);

private:
	data_type           _data;
	unsigned long long  _timestamp;
};
