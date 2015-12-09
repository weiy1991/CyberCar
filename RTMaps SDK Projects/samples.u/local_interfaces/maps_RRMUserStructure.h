////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _MAPSRRMUserStructure_H
#define _MAPSRRMUserStructure_H

// Includes maps sdk library header
#include "maps.hpp"
#include "maps_rrm.h"

// The MAPSRRMUserStructure SDK programmers guide sample
class MAPSRRMUserStructure: public MAPSRecordReplayMethod 
{
	// Use standard header definition macro
	MAPS_RECORD_REPLAY_METHOD_STANDARD_HEADER_CODE(MAPSRRMUserStructure);
private :
	// A common private attribute for RRMs
	//	to use with the Hint() method in order to return the string to be recorded in the 
	//	.rec file
	MAPSString m_hint;
};

#endif
