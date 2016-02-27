#pragma once

/*!
 * \file MidiMessage.h
 * Contains implementation of MidiMessage class.
 */

#include <vector>

/*!
 * \class MidiMessage MidiMessage.h <smidi/MidiMessage>
 * \brief The MidiMessage class
 */

class MidiMessage
{
public:
	/*!
	 * \enum DeviceNotificationType
	 * MIDI message types with their status byte as value.
	 */
	enum Type : unsigned char
	{
		NoteOff         = 0x80, //!< Note Off has MIDI channel number in lower nibble. Followed by 2 data bytes: *note number* and *note velocity*.
		NoteOn          = 0x90, //!< Note On has MIDI channel number in lower nibble. Followed by 2 data bytes: *note number* and *note velocity*.
		AfterTouch      = 0xA0, //!< After Touch has MIDI channel number in lower nibble. Followed by 2 data bytes: *note number* and *pressure*.
		ControlChange   = 0xB0, //!< Control Change has MIDI channel number in lower nibble. Followed by 2 data bytes: *controller number* and *controller value*.
		ProgramChange   = 0xC0, //!< Program Change has MIDI channel number in lower nibble. Followed by single data byte - *program number*.
		ChannelPressure = 0xD0, //!< Channel Pressure has MIDI channel number in lower nibble. Followed by single data byte - *pressure amount*.
		PitchWheel      = 0xE0, //!< Pitch Whell has MIDI channel number in lower nibble. Followed by 2 data bytes that represent single 14-bit *value*.

		System          = 0xF0, //!< This one is used to indicate that the message is the System i.e. one of the following.
		SysEx           = 0xF0, //!< System Exclusive (SysEx) message is a variable lenth message followed by SysExEnd (0xF7) status byte.
		MTCQuarter      = 0xF1, //!< Time Code Quarter frame is followed by single data byte with *time code value*.
		SongPosition    = 0xF2, //!< Song Position Pointer is followed by 2 data bytes that represent single 14-bit value of *song position* measured in MIDI Beats.
		SongSelect      = 0xF3, //!< Song select is followed by single data byte - *song number*.
		TuneRequest     = 0xF6, //!< Tune Request has no data bytes.
		SysExEnd        = 0xF7, //!< System Exclusive End status bytes makrs the end of System Exclusive MIDI message.

		MidiClock       = 0xF8, //!< MIDI Clock pulse is not followed by data bytes.
		MidiStart       = 0xFA, //!< MIDI Clock Start is not followed by data bytes.
		MidiContinue    = 0xFB, //!< MIDI Clock Continue is not followed by data bytes.
		MidiStop        = 0xFC, //!< MIDI Clock Stop is not followed by data bytes.
		ActiveSense     = 0xFE, //!< Active Sense is sent by devices approximately every 300ms if there was no any other MIDI activity. No data bytes.
		Reset           = 0xFF  //!< Reset is not followed by data bytes as well.
	};

	using data_type = std::vector<unsigned char>; //!< underlying data type for message buffer/container
	using size_type = data_type::size_type;       //!< size type for message buffer/container

public:
	MidiMessage();

	//! Copy ctor is defaulted since members also have trivial copy ctors
	MidiMessage(const MidiMessage&) = default;

	//! Move ctor is defaulted since members also have trivial move ctors
	MidiMessage(MidiMessage&&) = default;

	/*!
	 * \brief This constructor that accepts `std::vector` with message bytes
	 * \param [in] newData vector with message data. Contents will be copied into internal buffer.
	 * \param [in] newTimestamp optional timestamp parameter. Default value is 0.
	 */
	MidiMessage(const std::vector<unsigned char>& newData, unsigned long long newTimestamp = 0);

	/*!
	 * \brief This constructor that accepts pointer to external message buffer
	 * \param [in] newData pointer to existing message buffer. Contents will be copied into internal buffer.
	 * \param [in] newSize the size of external message buffer.
	 * \param [in] newTimestamp optional timestamp parameter. Default value is 0.
	 */
	MidiMessage(const unsigned char* newData, size_type newSize, unsigned long long newTimestamp = 0);

	/*!
	 * \brief Initializer-list constructor
	 * \param [in] newData initializer list with message bytes, e.g. `{0xB3, 0x12, 0x34}`. Contents will be copied into internal buffer.
	 * \param [in] newSize the size of external message buffer.
	 * \param [in] newTimestamp optional timestamp parameter. Default value is 0.
	 */
	MidiMessage(std::initializer_list<unsigned char> newData, unsigned long long newTimestamp = 0);

	//! Destructor is non-virtual since there shouldn't be any MidiMessage derivatives.
	~MidiMessage();

public:

	//! Returns `true` if this message is empty.
	bool isEmpty() const;

	/*!
	* \brief Returns `true` if the message is of spceified type.
	* \param [in] message type to check this message against.
	* \sa MidiMessage::Type
	*/
	bool isActually(MidiMessage::Type type) const;

	//! Returns `true` if this message contains full SysEx, i.e. if first message byte is 0xF0 and the last one is 0xF7.
	bool isCompleteSysEx() const;

	//! Returns channel number for non-system messages. For System messages will return 0.
	unsigned char channel() const;

	//! Returns MIDI message timestamp (OS dependent)
	unsigned long long timestamp() const;

	/*!
	* \brief Sets new message timestamp
	* \param [in] newTimestamp timestamp of the message. OS dependent value.
	*/
	void setTimestamp(unsigned long long newTimestamp);

	//! Returns size of the message (i.e. number of bytes).
	size_type size() const;

	//! Returns constant reference to the internal message container.
	const data_type& data() const;

	/*!
	* \brief Operator used to implicitly cast MidiMessage object to pointer to the message data buffer.
	*
	* Some OS dependent functions that send MIDI messages to the driver accept pointers to message data buffer.
	*/
	operator const unsigned char*() const;

	/*!
	* \brief Operator used to implicitly cast MidiMessage object to pointer to the message data buffer.
	*
	* Some OS dependent functions that send MIDI messages to the driver accept pointers to message data buffer.
	*/
	operator const char*() const;

	/*!
	* \brief Combines two message in a single one.
	*
	* Newly created MidiMessage will have concatenated data. This is usefull when, for example, you receive SysEx which
	* can be split for several message by the driver (due to internal driver buffer limits).
	*/
	MidiMessage operator+(const MidiMessage& other);

	/*!
	* \brief Appends MidiMessage data to this message.
	*
	* This is usefull when, for example, you receive SysEx which can be split for several message by the driver
	* (due to internal driver buffer limits).
	*/
	void operator +=(const MidiMessage& other);

	/*!
	* \brief This method resizes internal buffer to the specified size.
	* \param [in] newSize the size of the buffer to resize to.
	*
	* Some OS-dependent functions accept pre-allocated buffers. This function can be usefull in such cases.
	* If the size is bigger than the current one - the contents will be filled with zeros.
	*/
	void resizeBuffer(size_type newSize);

private:
	data_type           _data;
	unsigned long long  _timestamp;
};
