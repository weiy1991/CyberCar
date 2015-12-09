////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_MultiThread_H
#define _Maps_MultiThread_H

// Includes maps sdk library header
#include "maps.hpp"


// Declares a new MAPSComponent child class
class MAPSMultiThread : public MAPSComponent 
{
	//! Standard header code declaration
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSMultiThread)

	void NewThreadMethod();
};

#endif
