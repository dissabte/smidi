#pragma once

//! \cond INTERNAL

/*!
 * \file MidiSyncImpl.h
 * Contains platfrom-independent part of MIDI sync implementation.
 * \warning This file is not a part of library public interface!
 */

#include "../../include/smidi/MidiSync.h"
#include <memory>

class MidiSyncLinux : public MidiSync
{
public:
	class Implementation;
	
	explicit MidiSyncLinux(std::unique_ptr<Implementation>&& implementation);
	virtual ~MidiSyncLinux();

	virtual void startSync(double bpm) override;
	virtual void stopSync() override;
	virtual void resumeSync() override;
	virtual void changeSyncBpm(double bpm) override;
	virtual bool isSyncStarted() const override;
	virtual double syncInitialLatencyForTempo(double bpm) const override;

private:
	std::unique_ptr<Implementation> _impl;
};

//! \endcond
