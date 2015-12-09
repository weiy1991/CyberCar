////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_ComponentWithActions_H
#define _Maps_ComponentWithActions_H

// Includes maps sdk library header
#include "maps.hpp"


// Declares a new MAPSComponent child class
class MAPSComponentWithActions : public MAPSComponent 
{
	// Standard header code declaration
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSComponentWithActions)
		
	int m_period;

	static void SpeedUp(MAPSModule *module,int actionNb);
	static void SpeedDown(MAPSModule *module,int actionNb);

	int GetPeriod() {return(m_period);};
	void SetPeriod(int newPeriod);

};

#endif

