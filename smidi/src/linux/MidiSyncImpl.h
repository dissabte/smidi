#pragma once

/*!
 * \file MidiSyncImpl.h
 * Contains platfrom-independent part of MIDI sync implementation.
 * \warning This file is not a part of library public interface!
 */

#include "../../include/smidi/MidiSync.h"


class MidiSyncImpl : public MidiSync
{
public:
	explicit MidiSyncImpl();
	virtual ~MidiSyncImpl();

	virtual void startSync(double bpm) override;
	virtual void stopSync() override;
	virtual void resumeSync() override;
	virtual void changeSyncBpm(double bpm) override;
	virtual bool isSyncStarted() const override;
	virtual double syncInitialLatencyForTempo(double bpm) const override;
};
