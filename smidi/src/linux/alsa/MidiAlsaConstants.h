#pragma once

//! \cond INTERNAL

/*!
 * \file MidiAlsaConstants.h
 */

//! Some constants widely used with ALSA and other Linux functions

/*!
 * \brief Some constants widely used with ALSA and other Linux functions
 * \class MidiAlsaConstants MidiAlsaConstants.h "MidiAlsaConstants.h"
 */
struct MidiAlsaConstants
{
	//! Represents "no error" value which is used by many system calls on linux
	constexpr static int kNoError   = 0;

	//! Represents invalid id value
	constexpr static int kInvalidId = -1;
};

//! \endcond
