#pragma once

//! \cond INTERNAL

/*!
 * \file MidiSyncImpl.h
 * \warning This file is not a part of library public interface!
 */

#include "../MidiSyncLinux.h"
#include "MidiOutPortLinuxImpl.h"
#include "MidiQueue.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

/*!
 * \brief The MidiSync::Implementation class
 * \warning This class is not a part of library public interface!
 */

class MidiSyncLinux::Implementation
{
	static const unsigned int kPPQN;
	static const std::chrono::microseconds kMicrosecondsInAMinute;

	friend void* syncThreadFunction(void*);

public:
	Implementation(MidiOutPortLinux::Implementation& midiOut);
	~Implementation();

	void startSync(double bpm);
	void stopSync();
	void resumeSync();
	void changeSyncBpm(double bpm);
	bool isSyncStarted() const;
	std::chrono::microseconds syncInitialLatencyForTempo(double bpm) const;

private:
	void startSyncThread();
	void stopSyncThread();
	void compensateLatency(std::chrono::time_point<std::chrono::high_resolution_clock>& now, std::chrono::time_point<std::chrono::high_resolution_clock>& plannedSendingTime, bool& compensationHappened, std::chrono::microseconds& compensation);
	void syncThread();

	static void preciseWaitUntil(const std::chrono::time_point<std::chrono::high_resolution_clock>& timePoint);

private:
	MidiOutPortLinux::Implementation& _midiOutPort;

	MidiQueue               _queue;
	std::atomic<int>        _sourcePort;
	double                  _bpm;

	pthread_t               _thread;
	bool                    _threadIsCreated;
	bool                    _syncIsStarted;

	std::atomic<bool>       _pause;
	std::atomic<bool>       _resume;
	std::atomic<bool>       _changeBpm;
	std::atomic<bool>       _restart;
	std::atomic<bool>       _exit;

	std::mutex              _pauseMutex;
	std::condition_variable _resumeCondition;
};

//! \endcond
