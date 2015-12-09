////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_FusionSampleWithMultipleStartReading_H
#define _Maps_FusionSampleWithMultipleStartReading_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSFusionSampleWithMultipleStartReading : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSFusionSampleWithMultipleStartReading)
private :
	// Place here your specific methods and attributes
	MAPSIOElt* m_lastImageIOElt;
	MAPSIOElt* m_lastPointIOElt;
};

#endif
