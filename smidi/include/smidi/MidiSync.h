#pragma once

/*!
 * \file MidiSync.h
 * Contains MidiSync interface
 */

#include <memory>

/*!
 * \brief Interface class for MidiSync platform specific implementations
 * \class MidiSync MidiSync.h <smidi/MidiSync.h>
 */

class MidiSync
{
public:
	explicit MidiSync();
	virtual ~MidiSync();

	//! Starts sending MIDI Clocks
	virtual void startSync(double bpm) = 0;

	//! Stops sending MIDI Clocks
	virtual void stopSync() = 0;

	//! Resumes MIDI Clocks sending with at previouos tempo
	virtual void resumeSync() = 0;

	//! Changes current sync tempo
	virtual void changeSyncBpm(double bpm) = 0;

	//! Returns true if MIDI Clocks are being sent
	virtual bool isSyncStarted() const = 0;

	//! Returns the delay between syncStart() call and first the actual MIDI Clock event
	virtual double syncInitialLatencyForTempo(double bpm) const = 0;
};
