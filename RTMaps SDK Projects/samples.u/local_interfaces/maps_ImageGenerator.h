////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_ImageGenerator_H
#define _Maps_ImageGenerator_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSImageGenerator : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSImageGenerator)
private :

	MAPSTimestamp m_nextTimestamp;
};

#endif
