//! \cond INTERNAL

/*!
 * \file MidisyncLinuxImpl.h
 */

#include "MidiSyncLinuxImpl.h"
#include "MidiAlsaConstants.h"
#include <chrono>
#include <pthread.h>
#include <iostream>

MidiSyncLinux::Implementation::Implementation(MidiOutPortLinux::Implementation& midiOut)
    : _midiOutPort(midiOut)
    , _queue()
    , _sourcePort(MidiAlsaConstants::kInvalidId)
    , _bpm(120.0)
    , _threadIsCreated(false)
    , _pause(false)
    , _resume(false)
    , _updateBpm(false)
    , _restart(false)
    , _exit(false)
{
	_queue.init(midiOut.sequencer());
}

MidiSyncLinux::Implementation::~Implementation()
{
}

void MidiSyncLinux::Implementation::startSync(double bpm)
{

}

void MidiSyncLinux::Implementation::stopSync()
{

}

void MidiSyncLinux::Implementation::resumeSync()
{

}

void MidiSyncLinux::Implementation::changeSyncBpm(double bpm)
{

}

bool MidiSyncLinux::Implementation::isSyncStarted() const
{

}

double MidiSyncLinux::Implementation::syncInitialLatencyForTempo(double bpm) const
{

}

void MidiSyncLinux::Implementation::startSyncThread()
{
	if (!_threadIsCreated)
	{
		_pause.store(true); // to pause thread until sync is actually started

		_thread = std::thread(&MidiSyncLinux::Implementation::syncThread, this);
		if (_thread.joinable())
		{
			sched_param param  = {};
			param.sched_priority = sched_get_priority_max(SCHED_FIFO);
			int error = pthread_setschedparam(_thread.native_handle(), SCHED_FIFO, &param);
			if (MidiAlsaConstants::kNoError == error)
			{

			}
			else
			{
				std::cerr << "Couldn't set sync thread priority\n";
			}

			_threadIsCreated = true;
		}
		else
		{
			std::cerr << "Couldn't start sync thread\n";
		}
	}
}

void MidiSyncLinux::Implementation::syncThread()
{
	// initial pause check
	if (_pause)
	{
		_pause = false;
		std::unique_lock<std::mutex> lock(_pauseMutex);
		_resumeCondition.wait(lock, [this]{ return _resume.load(); });
	}

	// initial setup
	_queue.enqueueMidiSyncEvents(_sourcePort, true, true, kPPQN);

	std::chrono::time_point<std::chrono::high_resolution_clock> now;
	std::chrono::time_point<std::chrono::high_resolution_clock> plannedSendingTime;

	auto syncStateChanged = [this]{ return (_exit || _pause || _updateBpm || _restart); };

	while (true)
	{
		if (!syncStateChanged())
		{
			// jitter/drift compensation tricks (still not almighty)

			now = std::chrono::high_resolution_clock::now();

			// calculate planned sending time
			if (plannedSendingTime == std::chrono::time_point<std::chrono::high_resolution_clock>())
			{
				plannedSendingTime = now;
			}
			else
			{
				// TODO: calculate "now"  - new sending time
				if (now > plannedSendingTime)
				{
					// "we are late", so we add this value to overall compensation
					//compensation += delta;
				}
				else
				{
					// "we are early" and we will send next buffer as planned
					//now = plannedSendingTime;
				}
			}

			// TODO: calculate new plannedSendingTime = now + time-to-send-clocks

			// TODO: send
			_queue.start();
		}
		else
		{
			if (_pause)
			{
				// TODO: stop sending
				_queue.stop();

				_pause = false;
				std::unique_lock<std::mutex> lock(_pauseMutex);
				_resumeCondition.wait(lock, [this]()->bool { return _resume; });

				// TODO: could be some new setup of midi messages or whatever
				_queue.enqueueMidiSyncEvents(_sourcePort, true, true, kPPQN);
				continue;
			}

			if (_exit)
			{
				// TODO: stop sending
				_queue.stop();
				break;
			}

			if (_updateBpm)
			{
				// TODO: stop sending
				_queue.stop();

				// TODO: change queue tempo (can this be done w/o stop?)
				_queue.changeTempo(_bpm);

				// TODO: don't forget about possible time drift
				plannedSendingTime = std::chrono::time_point<std::chrono::high_resolution_clock>();
				continue;
			}

			if (_restart)
			{
				// TODO: stop sending
				_queue.stop();

				// TODO: initialize midi clocks
				_queue.enqueueMidiSyncEvents(_sourcePort, true, true, kPPQN);

				plannedSendingTime = std::chrono::time_point<std::chrono::high_resolution_clock>();
				continue;
			}
		}

		// TODO: calc time to wait for packets
		std::chrono::time_point<std::chrono::high_resolution_clock> endTime = now + std::chrono::time_point<std::chrono::high_resolution_clock>::duration::zero();

		// TODO: possibly update midi clocks to not send midi start/spp again
		//_queue.enqueueMidiSyncEvents(_sourcePort, false, false, kPPQN);

		// TODO: precise wait for packets
		preciseWaitUntil(endTime);
	}
}

void MidiSyncLinux::Implementation::preciseWaitUntil(const std::chrono::time_point<std::chrono::high_resolution_clock>& timePoint)
{
	const std::chrono::milliseconds spinTime(4);
	const std::chrono::time_point<std::chrono::high_resolution_clock> timePointWithoutSpinTime = timePoint - spinTime;

	std::this_thread::sleep_until(timePointWithoutSpinTime);

	while (std::chrono::high_resolution_clock::now() < timePoint); //spin
}

//! \endcond
