/*!
 * \file MidiMessage.cpp
 * Contains implementation of MidiMessage class.
 */

#include <../include/smidi/MidiMessage.h>
#include <algorithm>

MidiMessage::MidiMessage()
    : _data{}
    , _timestamp(0)
{
}

MidiMessage::MidiMessage(const std::vector<unsigned char>& newData, unsigned long long newTimestamp)
    : _data(newData)
    , _timestamp(newTimestamp)
{
}

MidiMessage::MidiMessage(const unsigned char* newData, MidiMessage::size_type newSize, unsigned long long newTimestamp)
    : _timestamp(newTimestamp)
{
	_data.resize(newSize);
	std::copy(newData, newData + newSize, _data.begin());
}

MidiMessage::MidiMessage(std::initializer_list<unsigned char> newData, unsigned long long newTimestamp)
    : _data(newData)
    , _timestamp(newTimestamp)
{
}

MidiMessage::~MidiMessage()
{
}

bool MidiMessage::isEmpty() const
{
	return _data.empty();
}

bool MidiMessage::isActually(MidiMessage::Type type) const
{
	return isEmpty() ? false : ((_data.front() & type) == type);
}

bool MidiMessage::isCompleteSysEx() const
{
	return isEmpty() ? false : (_data.front() == 0xF0 && _data.back() == 0xF7);
}

unsigned char MidiMessage::channel() const
{
	bool emt = isEmpty();
	bool syst = isActually(System);
	return (isEmpty() || isActually(System)) ? 0 : (_data.front() & 0x0F);
}

unsigned long long MidiMessage::timestamp() const
{
	return _timestamp;
}

void MidiMessage::setTimestamp(unsigned long long newTimestamp)
{
	_timestamp = newTimestamp;
}

MidiMessage::size_type MidiMessage::size() const
{
	return _data.size();
}

const MidiMessage::data_type&MidiMessage::data() const
{
	return _data;
}

MidiMessage MidiMessage::operator+(const MidiMessage& other)
{
	MidiMessage result(_data, _timestamp);
	result += other;
	return result;
}

void MidiMessage::operator +=(const MidiMessage& other)
{
	_data.insert(_data.end(), other.data().begin(), other.data().end());
}

void MidiMessage::resizeBuffer(MidiMessage::size_type newSize)
{
	_data.resize(newSize);
}

MidiMessage::operator const char*() const
{
	return reinterpret_cast<const char*>(_data.data());
}

MidiMessage::operator const unsigned char*() const
{
	return _data.data();
}
