//! \cond INTERNAL

/*!
 * \file MidiDeviceEnumeratorImpl.cpp
 */

#include "MidiDeviceEnumeratorImpl.h"
#include "MidiAlsaConstants.h"
#include "MidiInPortLinuxImpl.h"
#include "MidiOutPortLinuxImpl.h"
#include <algorithm>
#include <iostream>

const int MidiDeviceEnumerator::Implementation::kWriteCapabilities = SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE;
const int MidiDeviceEnumerator::Implementation::kReadCapabilities = SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ;


MidiDeviceEnumerator::Implementation::Implementation()
    : _sequencer(nullptr)
    , _myClientId(MidiAlsaConstants::kInvalidId)
{
	int error = snd_seq_open(&_sequencer, "default", SND_SEQ_OPEN_OUTPUT, SND_SEQ_NONBLOCK);
	if (MidiAlsaConstants::kNoError == error)
	{
		// set the name for the sequencer client
		snd_seq_set_client_name(_sequencer, "MIDI Device Enumerator");

		_myClientId  = snd_seq_client_id(_sequencer);

		refreshDevices();
	}
	else
	{
		std::cerr << "Couldn't open ALSA sequencer client: " << snd_strerror(error) << std::endl;
	}
}

MidiDeviceEnumerator::Implementation::~Implementation()
{
	if (_sequencer)
	{
		snd_seq_close(_sequencer);
	}
}

std::list<std::string> MidiDeviceEnumerator::Implementation::deviceNames() const
{
	std::list<std::string> result;

	const auto getDeviceName = [](const DeviceMap::value_type& device) -> std::string { return device.first; };
	std::transform(std::begin(_deviceMap), std::end(_deviceMap), std::back_inserter(result), getDeviceName);

	return result;
}

std::shared_ptr<MidiDevice> MidiDeviceEnumerator::Implementation::createDevice(const std::string& deviceName)
{
	std::shared_ptr<MidiDevice> result;

	const auto i = _deviceMap.find(deviceName);
	if (i != std::end(_deviceMap))
	{
		const std::shared_ptr<MidiDevice>& device = std::get<DeviceObject>(i->second);
		if (device)
		{
			result = device;
		}
		else
		{
			int clientId = std::get<ClientId>(i->second);

			snd_seq_client_info_t* clientInfo = nullptr;
			snd_seq_client_info_alloca(&clientInfo);
			int error = snd_seq_get_any_client_info(_sequencer, clientId, clientInfo);
			if (MidiAlsaConstants::kNoError == error)
			{
				std::vector<std::shared_ptr<MidiInPort>> inputs;
				std::vector<std::shared_ptr<MidiOutPort>> outputs;
				const PortAction collectPorts = std::bind(&Implementation::collectMidiPortObjects, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref(inputs), std::ref(outputs));
				traverseClientPorts(_sequencer, clientInfo, 0, collectPorts);

				result = std::make_shared<MidiDevice>(deviceName, inputs, outputs);

				// cache the device object
				_deviceMap[deviceName] = std::make_tuple(clientId, result);
			}
			else
			{
				std::cerr << "Couldn't get client info for device named '" << deviceName << "'' (" << clientId << ") because:" << snd_strerror(error) << std::endl;
			}
		}
	}
	return result;
}

void MidiDeviceEnumerator::Implementation::updateDeviceList()
{
	refreshDevices();
}

void MidiDeviceEnumerator::Implementation::clearDevices()
{
	_deviceMap.clear();
	_ourClientIds.clear();
}

void MidiDeviceEnumerator::Implementation::refreshDevices()
{
	clearDevices();
	updateAllDeviceInformation(_deviceMap);
}

void MidiDeviceEnumerator::Implementation::updateAllDeviceInformation(DeviceMap& devices) const
{
	const ClientAction clientAction = std::bind(&Implementation::collectClientInformation, this, std::placeholders::_1, std::placeholders::_2, std::ref(devices));
	traverseAllClients(_sequencer, clientAction);
}

void MidiDeviceEnumerator::Implementation::collectClientInformation(snd_seq_t* sequencer, snd_seq_client_info_t* clientInfo, DeviceMap& devices) const
{
	const int clientId = snd_seq_client_info_get_client(clientInfo);
	const std::string clientName = snd_seq_client_info_get_name(clientInfo);
	devices.emplace(clientName, std::make_tuple(clientId, nullptr));
}

void MidiDeviceEnumerator::Implementation::collectMidiPortObjects(snd_seq_t*, snd_seq_client_info_t*, snd_seq_port_info_t* portInfo, std::vector<std::shared_ptr<MidiInPort> >& inputPorts, std::vector<std::shared_ptr<MidiOutPort> >& outputPorts)
{
	const int clientId = snd_seq_port_info_get_client(portInfo);
	const int portId = snd_seq_port_info_get_port(portInfo);

	const std::string portName = snd_seq_port_info_get_name(portInfo);

	const unsigned int caps = snd_seq_port_info_get_capability(portInfo);
	if ((caps & kReadCapabilities) == kReadCapabilities)
	{
		std::unique_ptr<MidiInPortLinux::Implementation> impl(new MidiInPortLinux::Implementation(portName, clientId, portId));
		_ourClientIds.insert(impl->applicationClientId());

		inputPorts.emplace_back(std::make_shared<MidiInPortLinux>(std::move(impl)));
	}
	if ((caps & kWriteCapabilities) == kWriteCapabilities)
	{
		std::unique_ptr<MidiOutPortLinux::Implementation> impl(new MidiOutPortLinux::Implementation(portName, clientId, portId));
		_ourClientIds.insert(impl->applicationClientId());

		outputPorts.emplace_back(std::make_shared<MidiOutPortLinux>(std::move(impl)));
	}
}

void MidiDeviceEnumerator::Implementation::traverseAllClients(snd_seq_t* sequencer, MidiDeviceEnumerator::Implementation::ClientAction action) const
{
	snd_seq_client_info_t* clientInfo = nullptr;
	snd_seq_client_info_alloca(&clientInfo);
	snd_seq_client_info_set_client(clientInfo, MidiAlsaConstants::kInvalidId);

	while (MidiAlsaConstants::kNoError == snd_seq_query_next_client(sequencer, clientInfo))
	{
		const int clientId = snd_seq_client_info_get_client(clientInfo);
		const bool myClient = isOurClient(clientId);
		if (clientId == 0 || clientId == _myClientId || myClient) // skip "System" client and our own application clients
		{
			continue;
		}
		if (action)
		{
			action(sequencer, clientInfo);
		}
	}
}

void MidiDeviceEnumerator::Implementation::traverseClientPorts(snd_seq_t* sequencer, snd_seq_client_info_t* clientInfo, int capabilities, MidiDeviceEnumerator::Implementation::PortAction action) const
{
	snd_seq_port_info_t* portInfo = nullptr;
	snd_seq_port_info_alloca(&portInfo);

	const int clientId = snd_seq_client_info_get_client(clientInfo);
	snd_seq_port_info_set_client(portInfo, clientId);
	snd_seq_port_info_set_port(portInfo, -1);

	while (MidiAlsaConstants::kNoError == snd_seq_query_next_port(sequencer, portInfo))
	{
		const unsigned int portType = snd_seq_port_info_get_type(portInfo);
		const bool portUnderstandsMIDI = ((portType & SND_SEQ_PORT_TYPE_MIDI_GENERIC) == SND_SEQ_PORT_TYPE_MIDI_GENERIC);
		const bool portIsSynth = ((portType & SND_SEQ_PORT_TYPE_SYNTH) == SND_SEQ_PORT_TYPE_SYNTH);
		if (portUnderstandsMIDI || portIsSynth)
		{
			const unsigned int caps = snd_seq_port_info_get_capability(portInfo);
			if ((caps & capabilities) == capabilities)
			{
				if (action)
				{
					action(sequencer, clientInfo, portInfo);
				}
			}
		}
	}
}

bool MidiDeviceEnumerator::Implementation::isOurClient(const unsigned char clientId) const
{
	return _ourClientIds.count(clientId) != 0;
}

//! \endcond
