////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_Counter_H
#define _Maps_Counter_H

#include "maps.hpp"

//The MAPSCounter SDK programmers guide sample
class MAPSCounter : public MAPSComponent 
{
	// Standard header code declaration
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSCounter)

private :
	//  Declaration of a new timestamp variable that will keep the next appointment timestamp
	MAPSTimestamp	m_appointment;
	//  Declaration of the counter variable
	int				m_ticks;
};

#endif
