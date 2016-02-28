#pragma once

/*!
 * \file MidiDeviceEnumeratorImpl.h
 * Contains Linux implementation of MidiDeviceEnumerator
 */

#include "../../../include/smidi/MidiDeviceEnumerator.h"
#include "../../../include/smidi/MidiDevice.h"
#include "../../../include/smidi/MidiPort.h"
#include <map>
#include <set>
#include <tuple>
#include <string>
#include <memory>
#include <functional>
#include <alsa/asoundlib.h>

class MidiDeviceEnumerator::Implementation
{
	static const int kWriteCapabilities;
	static const int kReadCapabilities;

	enum DeviceInfoFields
	{
		ClientId,
		DeviceObject
	};

	using PortAction   = std::function<void(snd_seq_t*, snd_seq_client_info_t*, snd_seq_port_info_t*)>;
	using ClientAction = std::function<void(snd_seq_t*, snd_seq_client_info_t*)>;
	using DeviceInfo = std::tuple<int, std::shared_ptr<MidiDevice>>;
	using DeviceMap  = std::map<std::string, DeviceInfo>;

public:
	Implementation();
	~Implementation();

	std::list<std::string> deviceNames() const;
	std::shared_ptr<MidiDevice> createDevice(const std::string& deviceName);

	void updateDeviceList();

private:
	void clearDevices();
	void refreshDevices();

	void updateAllDeviceInformation(DeviceMap& devices) const;

	void collectClientInformation(snd_seq_t* sequencer, snd_seq_client_info_t* clientInfo, DeviceMap& devices) const;
	void collectMidiPortObjects(snd_seq_t* sequencer, snd_seq_client_info_t* clientInfo, snd_seq_port_info_t*portInfo, std::vector<std::shared_ptr<MidiPort>>& inputPorts, std::vector<std::shared_ptr<MidiPort> >& outputPorts);

	void traverseAllClients(snd_seq_t* sequencer, ClientAction action) const;
	void traverseClientPorts(snd_seq_t* sequencer, snd_seq_client_info_t* clientInfo, int capabilities, MidiDeviceEnumerator::Implementation::PortAction action) const;

	bool isOurClient(const unsigned char clientId) const;

private:
	DeviceMap     _deviceMap;
	std::set<int> _ourClientIds;
	snd_seq_t*    _sequencer;
	unsigned char _myClientId;
};
