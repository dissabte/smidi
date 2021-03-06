#include <iostream>
#include <smidi/MidiDeviceEnumerator.h>
#include <smidi/MidiDevice.h>
#include <smidi/MidiInPort.h>
#include <smidi/MidiOutPort.h>
#include <hotplug/HotPlugWatcher.h>
#include <iostream>


void printDevices(const MidiDeviceEnumerator& enumerator)
{
	std::cout << "Found MIDI devices:\n";
	for (const std::string& deviceName : enumerator.deviceNames())
	{
		std::cout << "\t- " << deviceName << std::endl;

		std::shared_ptr<MidiDevice> device = enumerator.createDevice(deviceName);
		for (const std::shared_ptr<MidiInPort>& port : device->inputPorts())
		{
			std::cout << "\t  IN:  " << port->name() << std::endl;
		}
		for (const std::shared_ptr<MidiOutPort>& port : device->outputPorts())
		{
			std::cout << "\t  OUT: " << port->name() << std::endl;
			port->sync().startSync(120.0);
		}
	}
	std::cout << "Done. You can plug in/out some USB/MIDI device now OR press Enter to quit...\n";
}

int main()
{
	MidiDeviceEnumerator enumerator;

	printDevices(enumerator);

	HotPlugWatcher watcher(HotPlugWatcher::USB);
	watcher.registerObserver([&enumerator](const HotPlugNotificationType& notification, const HotPlugNotificationData&)
	{
		if (notification == HotPlugNotificationType::DEVICE_ADDED ||
		    notification == HotPlugNotificationType::DEVICE_REMOVED)
		{
			enumerator.updateDeviceList();
			printDevices(enumerator);
		}
	});

	std::cin.ignore();
	return 0;
}
