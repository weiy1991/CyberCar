////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_EnumProperties_H
#define _Maps_EnumProperties_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSEnumProperties : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSEnumProperties)
	void Set(MAPSProperty& p, const MAPSString& value);
	void Set(MAPSProperty& p, MAPSInt64 value);
	void Set(MAPSProperty& p, MAPSEnumStruct enumStruct);
private :
	// Place here your specific methods and attributes
    int			m_CurrentPropIndex;
    MAPSString	m_CurrentPropString;
    void UpdateCurrentPropValue();
};

#endif
