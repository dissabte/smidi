#include <iostream>
#include <smidi/MidiDeviceEnumerator.h>
#include <smidi/MidiInPort.h>
#include <smidi/MidiOutPort.h>

int main()
{
	MidiDeviceEnumerator enumerator;
	std::cout << "Found MIDI devices:\n";
	for (const std::string& deviceName : enumerator.deviceNames())
	{
		std::cout << "\t- " << deviceName << std::endl;

		std::shared_ptr<MidiDevice> device = enumerator.createDevice(deviceName);
		for (const std::shared_ptr<MidiPort>& port : device->inputPorts())
		{
			std::cout << "\t  IN:  " << port->name() << std::endl;
		}
		for (const std::shared_ptr<MidiPort>& port : device->outputPorts())
		{
			std::cout << "\t  OUT: " << port->name() << std::endl;
		}
	}
	std::cout << "Done\n";
	return 0;
}
