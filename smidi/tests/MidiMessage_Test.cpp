#include <UnitTest++/UnitTest++.h>
#include <smidi/MidiMessage.h>
#include <vector>

SUITE(MidiMessageTests)
{
	TEST(MidiMessageConstruction)
	{
		// Default ctor
		MidiMessage message;
		const bool a = message.isEmpty();
		CHECK(message.isEmpty());
		CHECK(!message.isCompleteSysEx());
		CHECK(!message.isActually(MidiMessage::NoteOff));
		CHECK_EQUAL(0, message.timestamp());
		CHECK_EQUAL(0, message.channel());
		CHECK_EQUAL(0, message.size());

		// Initializer-list ctor
		MidiMessage ccMessage{0xB1, 0x23, 0x45};
		CHECK(!ccMessage.isEmpty());
		CHECK(ccMessage.isActually(MidiMessage::ControlChange));
		unsigned char ch = ccMessage.channel();
		CHECK_EQUAL(1, static_cast<int>(ch));
		CHECK_EQUAL(3, ccMessage.size());

		// Vector ctor
		const unsigned long long timestamp = 12;
		std::vector<unsigned char> sysExData{0xF0, 0x11, 0x11, 0x11}; // incomplete sysex
		MidiMessage sysExMessage(sysExData, timestamp);
		CHECK(!sysExMessage.isEmpty());
		CHECK(sysExMessage.isActually(MidiMessage::System));
		CHECK(sysExMessage.isActually(MidiMessage::SysEx));
		CHECK(!sysExMessage.isCompleteSysEx());
		CHECK_EQUAL(sysExData.size(), sysExMessage.size());
		CHECK_EQUAL(timestamp, sysExMessage.timestamp());

		// Buffer pointer ctor
		MidiMessage anotherSysExMessage(sysExData.data(), sysExData.size(), timestamp);
		CHECK(!sysExMessage.isEmpty());
		CHECK(sysExMessage.isActually(MidiMessage::System));
		CHECK(sysExMessage.isActually(MidiMessage::SysEx));
		CHECK(!sysExMessage.isCompleteSysEx());
		CHECK_EQUAL(sysExData.size(), sysExMessage.size());
		CHECK_EQUAL(timestamp, sysExMessage.timestamp());

		// Copy ctor
		MidiMessage copiedCCMessage(ccMessage);
		CHECK(!copiedCCMessage.isEmpty());
		CHECK(copiedCCMessage.isActually(MidiMessage::ControlChange));
		CHECK_EQUAL(1, copiedCCMessage.channel());
		CHECK_EQUAL(3, copiedCCMessage.size());

		// Move ctor
		MidiMessage movedSysExMessage(MidiMessage(sysExData, timestamp));
		CHECK(!movedSysExMessage.isEmpty());
		CHECK(movedSysExMessage.isActually(MidiMessage::System));
		CHECK(movedSysExMessage.isActually(MidiMessage::SysEx));
		CHECK(!movedSysExMessage.isCompleteSysEx());
		CHECK_EQUAL(sysExData.size(), movedSysExMessage.size());
		CHECK_EQUAL(timestamp, movedSysExMessage.timestamp());

	}

	TEST(MidiMessageMembers)
	{
		const unsigned long long timestamp = 25;
		std::vector<unsigned char> sysExData{0xF0, 0x22, 0x22, 0x22, 0x33}; // incomplete sysex
		MidiMessage sysExMessage(sysExData, timestamp);
		CHECK(!sysExMessage.isEmpty());
		CHECK(sysExMessage.isActually(MidiMessage::System));
		CHECK(sysExMessage.isActually(MidiMessage::SysEx));
		CHECK(!sysExMessage.isCompleteSysEx());
		CHECK_EQUAL(sysExData.size(), sysExMessage.size());
		CHECK_EQUAL(timestamp, sysExMessage.timestamp());

		// +=
		sysExMessage += MidiMessage{0xF7};
		CHECK(sysExMessage.isCompleteSysEx());
		CHECK_EQUAL(sysExData.size() + 1, sysExMessage.size());

		// +
		MidiMessage concatenated = MidiMessage{0x85} + MidiMessage{0x67, 0x89};
		CHECK(concatenated.isActually(MidiMessage::NoteOff));
		CHECK_EQUAL(3, concatenated.size());
		CHECK_EQUAL(5, concatenated.channel());
		CHECK_EQUAL(0x85, concatenated.data()[0]);
		CHECK_EQUAL(0x67, concatenated.data()[1]);
		CHECK_EQUAL(0x89, concatenated.data()[2]);

		// set/get timestamp
		concatenated.setTimestamp(123);
		CHECK_EQUAL(123, concatenated.timestamp());

		// resize buffer
		concatenated.resizeBuffer(4);
		CHECK_EQUAL(4, concatenated.size());
		CHECK_EQUAL(0, concatenated.data()[3]);

		// casts
		const unsigned char* messagePointer = nullptr;
		messagePointer = concatenated;
		CHECK_EQUAL(messagePointer, concatenated.data().data());

		unsigned char* messagePointer2 = nullptr;
		messagePointer2 = concatenated;
		CHECK_EQUAL(messagePointer2, messagePointer);
	}
}
