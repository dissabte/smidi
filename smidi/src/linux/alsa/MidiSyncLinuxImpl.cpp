//! \cond INTERNAL

/*!
 * \file MidisyncLinuxImpl.h
 */

#include "MidiSyncLinuxImpl.h"
#include "MidiAlsaConstants.h"
#include <chrono>
#include <pthread.h>
#include <iostream>

const unsigned int MidiSyncLinux::Implementation::kPPQN = 24;

const std::chrono::microseconds MidiSyncLinux::Implementation::kMicrosecondsInAMinute = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::minutes(1));

MidiSyncLinux::Implementation::Implementation(MidiOutPortLinux::Implementation& midiOut)
    : _midiOutPort(midiOut)
    , _queue()
    , _sourcePort(midiOut.applicationPortId())
    , _bpm(120.0)
    , _threadIsCreated(false)
    , _syncIsStarted(false)
    , _pause(false)
    , _resume(false)
    , _changeBpm(false)
    , _restart(false)
    , _exit(false)
{
	_queue.init(midiOut.sequencer());
	startSyncThread();
}

MidiSyncLinux::Implementation::~Implementation()
{
	stopSyncThread();
}

void MidiSyncLinux::Implementation::close()
{
	_queue.close();
}

void MidiSyncLinux::Implementation::startSync(double bpm)
{
	_bpm = bpm;
	_restart = true;
	resumeSync();
}

void MidiSyncLinux::Implementation::stopSync()
{
	if (_syncIsStarted)
	{
		_pause = true;
		_syncIsStarted = false;
	}
}

void MidiSyncLinux::Implementation::resumeSync()
{
	if (!_syncIsStarted)
	{
		_resume = true;
		_resumeCondition.notify_one();
		_syncIsStarted = true;
	}
}

void MidiSyncLinux::Implementation::changeSyncBpm(double bpm)
{
	_bpm = bpm;
	_changeBpm = true;
}

bool MidiSyncLinux::Implementation::isSyncStarted() const
{
	return _threadIsCreated && _syncIsStarted;
}

std::chrono::microseconds MidiSyncLinux::Implementation::syncInitialLatencyForTempo(double bpm) const
{
	std::chrono::microseconds beatDuration(static_cast<unsigned long long>(kMicrosecondsInAMinute.count() / bpm));
	std::chrono::microseconds clockDuration(beatDuration / kPPQN);
	return 2 * clockDuration; // MIDI Start and MIDI Sond Position Pointer are sent before first MIDI Clock
}

void* syncThreadFunction(void* param)
{
	MidiSyncLinux::Implementation* sync = reinterpret_cast<MidiSyncLinux::Implementation*>(param);
	if (sync)
	{
		sync->syncThread();
	}
	return nullptr;
}

void MidiSyncLinux::Implementation::startSyncThread()
{
	if (!_threadIsCreated)
	{
		_pause.store(true); // to pause thread until sync is actually started

		pthread_attr_t attr = {};
		int err = pthread_attr_init(&attr);
		if (err == MidiAlsaConstants::kNoError)
		{
			sched_param param = {};
			param.sched_priority = 1;
			pthread_attr_setschedparam(&attr, &param);
			pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

			pthread_t threadId;
			err = pthread_create(&threadId, &attr, syncThreadFunction, reinterpret_cast<void*>(this));
			if (err != MidiAlsaConstants::kNoError)
			{
				perror("pthread_create");
			}
		}
		else
		{
			perror("pthread_attr_init");
		}
	}
}

void MidiSyncLinux::Implementation::stopSyncThread()
{
	if (_threadIsCreated)
	{
		_exit = true;
		pthread_join(_thread, nullptr);
	}
}

inline void MidiSyncLinux::Implementation::compensateLatency(std::chrono::time_point<std::chrono::high_resolution_clock>& now, std::chrono::time_point<std::chrono::high_resolution_clock>& plannedSendingTime, bool& compensationHappened, std::chrono::microseconds& compensation)
{
	// here comes jitter/drift compensation tricks (still not almighty)

	if (compensationHappened)
	{
		// we need to reset queue tempo back to normal if compensation has been applied on the previous frame
		_queue.stop();
		_queue.setTempo(_bpm);
		_queue.start();
		compensationHappened = false;
	}

	if (now > plannedSendingTime)
	{
		// "we are late", so we add this value to overall compensation
		compensation += std::chrono::duration_cast<std::chrono::microseconds>(now - plannedSendingTime);
	}
	else if (now < plannedSendingTime)
	{
		// "we are early" and we will send next buffer as planned
		preciseWaitUntil(plannedSendingTime);
		now = plannedSendingTime;
	}

	if (compensation > std::chrono::microseconds(5))
	{
		// calculate new bpm (a bit faster) to catch the phase during next send
		const std::chrono::microseconds compensatedTimeToSendPackets = std::chrono::microseconds(static_cast<int>(kMicrosecondsInAMinute.count() / _bpm)) - compensation;
		const double compensatedBpm = static_cast<double>(kMicrosecondsInAMinute.count()) / compensatedTimeToSendPackets.count();

		_queue.stop();
		_queue.setTempo(compensatedBpm);
		_queue.start();

		compensationHappened = true;
		compensation = std::chrono::microseconds(0);
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
		_resume = false;
	}

	// initial setup
	_queue.setTempo(_bpm);
	_queue.start();

	std::chrono::time_point<std::chrono::high_resolution_clock> now;
	std::chrono::time_point<std::chrono::high_resolution_clock> plannedSendingTime;
	std::chrono::microseconds compensation = std::chrono::microseconds(0);
	std::chrono::microseconds timeToSendPackets = std::chrono::microseconds(static_cast<int>(kMicrosecondsInAMinute.count() / _bpm));

	const auto syncStateChanged = [this]{ return (_exit || _pause || _changeBpm || _restart); };

	bool inludeMidiStart = true;
	bool compensationHappened = false;

	while (true)
	{
		if (!syncStateChanged())
		{
			now = std::chrono::high_resolution_clock::now();

			if (plannedSendingTime == std::chrono::time_point<std::chrono::high_resolution_clock>())
			{
				// either initial cycle or reset was made -> reset
				plannedSendingTime = now;
			}
			else
			{
				compensateLatency(now, plannedSendingTime, compensationHappened, compensation);
			}

			// next planned sending time
			plannedSendingTime += timeToSendPackets;

			// filling the queue with MIDI messages, since queue is started it will start sending immediately
			_queue.enqueueMidiSyncEvents(_sourcePort, inludeMidiStart, inludeMidiStart, kPPQN);
		}
		else
		{
			if (_pause)
			{
				_pause = false;
				_queue.stop();

				std::unique_lock<std::mutex> lock(_pauseMutex);
				_resumeCondition.wait(lock, [this]()->bool { return _resume; });
				_resume = false;

				inludeMidiStart = true;

				_queue.start();
				continue;
			}

			if (_exit)
			{
				_queue.stop();
				break;
			}

			if (_changeBpm)
			{
				_changeBpm = false;
				_queue.stop();
				_queue.setTempo(_bpm);

				timeToSendPackets = std::chrono::microseconds(static_cast<int>(kMicrosecondsInAMinute.count() / _bpm));
				plannedSendingTime = std::chrono::time_point<std::chrono::high_resolution_clock>();

				_queue.start();
				continue;
			}

			if (_restart)
			{
				_restart = false;
				_queue.stop();

				inludeMidiStart = true;

				plannedSendingTime = std::chrono::time_point<std::chrono::high_resolution_clock>();

				_queue.start();
				continue;
			}
		}

		if (inludeMidiStart)
		{
			inludeMidiStart = false;
		}

		preciseWaitUntil(plannedSendingTime);
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
