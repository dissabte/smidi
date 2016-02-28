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
	constexpr static unsigned int kPPQN = 24;

public:
	Implementation(MidiOutPortLinux::Implementation& midiOut);
	~Implementation();

	void startSync(double bpm);
	void stopSync();
	void resumeSync();
	void changeSyncBpm(double bpm);
	bool isSyncStarted() const;
	double syncInitialLatencyForTempo(double bpm) const;

private:
	void startSyncThread();
	void syncThread();

	static void preciseWaitUntil(const std::chrono::time_point<std::chrono::high_resolution_clock>& timePoint);

private:
	MidiOutPortLinux::Implementation& _midiOutPort;

	MidiQueue               _queue;
	std::atomic<int>        _sourcePort;
	double                  _bpm;

	std::thread             _thread;
	bool                    _threadIsCreated;

	std::atomic<bool>       _pause;
	std::atomic<bool>       _resume;
	std::atomic<bool>       _updateBpm;
	std::atomic<bool>       _restart;
	std::atomic<bool>       _exit;

	std::mutex              _pauseMutex;
	std::condition_variable _resumeCondition;
};

//! \endcond
