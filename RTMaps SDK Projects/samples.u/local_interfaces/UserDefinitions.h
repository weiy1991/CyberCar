////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _UserDefinitions_H
#define _UserDefinitions_H
#include "maps.hpp"
 /*	The user-defined structure shared header.
 *	This header may be included by all of the components or rrms that exchange
 *	data of type "MyNewStructure". This file define both :
 *		- the C structure containing the data (no pointer must be used)
 *		- the filter to use with inputs and RRMs
 */



// Define a user-specifi structure called MyNewStructure (no pointer are allowed in user-defined structures)

// First of all, let's disable structure fields alignment (force alignment to 1 byte)
// so that the sizeof(MyNewStructure) will be the same whatever the platform and compiler.
// This will allow to use a generic record/replay method for all specific structures which can be used to
// record in binary format on one platform and replay on another.
#pragma pack(push,1)

struct MyNewStructure 
{
	// One data
	int m_value;
	// Another data...
	char m_message[50];
};

//Now re-enable structure alignment
#pragma pack(pop)

// The RTMaps input filter for the structure MyNewStructure
const MAPSTypeFilterBase MAPSFilterMyNewStructure = MAPS_FILTER_USER_STRUCTURE(MyNewStructure);

#endif
