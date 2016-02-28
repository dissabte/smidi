//! \cond INTERNAL

/*!
 * \file MidiSyncLinux.cpp
 * \warning This file is not a part of library public interface!
 */

#include "MidiSyncLinux.h"

#ifdef SMIDI_USE_ALSA
#include "alsa/MidiSyncLinuxImpl.h"
#endif

MidiSyncLinux::MidiSyncLinux(std::unique_ptr<Implementation>&& implementation)
    : MidiSync()
    , _impl(std::move(implementation))
{
}

MidiSyncLinux::~MidiSyncLinux()
{
}

void MidiSyncLinux::startSync(double bpm)
{
	_impl->startSync(bpm);
}

void MidiSyncLinux::stopSync()
{
	_impl->stopSync();
}

void MidiSyncLinux::resumeSync()
{
	_impl->resumeSync();
}

void MidiSyncLinux::changeSyncBpm(double bpm)
{
	_impl->changeSyncBpm(bpm);
}

bool MidiSyncLinux::isSyncStarted() const
{
	return _impl->isSyncStarted();
}

double MidiSyncLinux::syncInitialLatencyForTempo(double bpm) const
{
	return _impl->syncInitialLatencyForTempo(bpm);
}

//! \endcond
