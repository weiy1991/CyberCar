////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_SetProperty_Overload_H
#define _Maps_SetProperty_Overload_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSSetProperty_Overload : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSSetProperty_Overload)

	//Overload of the Set method for integer properties.
	void Set(MAPSProperty& p, MAPSInt64 newVal);
	//Overload of the Set method for string properties.
	void Set(MAPSProperty& p, const MAPSString& newString);
private :
	// Place here your specific methods and attributes
	MAPSInt64 m_Value;
};

#endif
