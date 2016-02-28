//! \cond INTERNAL

/*!
 * \file MidiSyncImpl.cpp
 * \warning This file is not a part of library public interface!
 */

#include "MidiSyncImpl.h"

MidiSyncImpl::MidiSyncImpl()
{
}

MidiSyncImpl::~MidiSyncImpl()
{
}

void MidiSyncImpl::startSync(double bpm)
{
	(void)bpm;
}

void MidiSyncImpl::stopSync()
{
}

void MidiSyncImpl::resumeSync()
{
}

void MidiSyncImpl::changeSyncBpm(double bpm)
{
	(void)bpm;
}

bool MidiSyncImpl::isSyncStarted() const
{
	return false;
}

double MidiSyncImpl::syncInitialLatencyForTempo(double bpm) const
{
	(void)bpm;
	return 0.0;
}

//! \endcond
