////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _MAPS_CUSTOMCLOCK_H
#define _MAPS_CUSTOMCLOCK_H

#include "maps.hpp"
#include "maps_baseclock.h"


class MAPSCustomClock : public MAPSComponent,
			public MAPSBaseClock //Inherit this class to be able to implement a custom clock in your component.
{

    //We have to use this specific macro for components implementing a custom clock.
    MAPS_CLOCK_COMPONENT_HEADER_CODE(MAPSCustomClock);
    void Set(MAPSProperty& p, MAPSInt64 value);
private :
    MAPSMutex		m_MyTimeMonitor;

    MAPSTimestamp	m_Timebase;
    MAPSTimestamp	m_TimebaseX;
    int				m_Timespeed;

    //Implement those 4 methods, inherited from the MAPSBaseClock class.
    void InitClock() {}		    //Nothing special here. Initialization of the clock when RTMaps is launched.
    void RunClock();		    //Implemented in .cpp. Called when RTMaps needs to start the clock.
    void ShutdownClock() {}	    //Nothing special here. Called when RTMaps stops the current clock.
    MAPSTimestamp CurrentTime();    //Implemented in .cpp. Called every time someone in RTMaps needs the current time.

};
#endif //_MAPS_CUSTOMCLOCK_H