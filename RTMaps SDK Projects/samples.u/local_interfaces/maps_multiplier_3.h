////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_multiplier_H
#define _Maps_multiplier_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSmultiplier3 : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSmultiplier3)
private :
	// Place here your specific methods and attributes

	// This member variable will be used for allocating once the output buffers at first time.
	bool m_firstTime;
};

#endif
