////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_multiplier_H
#define _Maps_multiplier_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSmultiplier4 : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSmultiplier4)
	//Overload the Set functions (virtual) enable to hook the change of property values.
	//Here we hook the changes in properties of integer type.
	void Set(MAPSProperty& p, MAPSInt64 value);
private :
	// Place here your specific methods and attributes

	// This member variable will be used for allocating once the output buffers at first time.
	bool m_firstTime;

	// This member variable will remain synchronized with the "factor" property value.
	int m_factor;
};

#endif
