////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/* Chapter 8 - Custom clocks and synchronization
 * This section demonstrates how to write components that can
 * act on the RTMaps clock, either by correcting the current date
 * from time to time (based on an external tick such as a PPS signal from
 * a GPS for example), or by implementing a real clock inside a component.
*/


/*! \ingroup Chapter8
 * \file
 * \brief CustomClock
 * A simple component demonstrating how to implement a custom clock within an RTMaps component.
 * If this component is chosen in the RTMaps settings as the one whose clock will be used,
 * then the CurrentTime() function will be called each time someone in RTMaps requests the 
 * current time (VCR, Recorder, any other component...)
 * Once this component is on the diagram, choose menu Diagram |Properties | General |Current clock | thisComponentName
 * in order to use it as custom clock.
 */

#include "maps_customclock.h"

MAPS_BEGIN_INPUTS_DEFINITION(MAPSCustomClock)
MAPS_END_INPUTS_DEFINITION

MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSCustomClock)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSCustomClock)
    MAPS_PROPERTY("timespeed",1000,false,true)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSCustomClock)
MAPS_END_ACTIONS_DEFINITION

MAPS_COMPONENT_DEFINITION(MAPSCustomClock,"CustomClock","1.0",128,MAPS::Threaded,
			  MAPS::Threaded,0,0,1,0)

/* This method is called when RTMaps needs to start its current clock. */
void MAPSCustomClock::RunClock()
{
	/*
	Use this function to declare the maximum "real" timespeed that your clock will impose.
	This value can be higher than the "real" timespeed but never lower.
	If this value is much higher than the "real" timespeed, this may result in more CPU consumption,
	whereas lower values result in unaccuracies for pausing functions such as Rest or Wait (so this may
	cause delays during replay for example.
	*/
	SetAbsoluteTimeSpeed((int)GetIntegerProperty("timespeed"));

	m_MyTimeMonitor.Lock();
	m_Timebase = 0;
	//Retrieve absolute PC clock value.
	m_TimebaseX = MAPS::GetSystemAccurateTiming();
	m_Timespeed = (int)GetIntegerProperty(0);
	m_MyTimeMonitor.Release();
}


/* This method is called everytime someone in RTMaps asks for the current time*/
MAPSTimestamp MAPSCustomClock::CurrentTime() 
{
	m_MyTimeMonitor.Lock();
	MAPSTimestamp t = m_Timebase + (MAPS::GetSystemAccurateTiming() - m_TimebaseX)*m_Timespeed/1000;
	m_MyTimeMonitor.Release();
	return t;
}

void MAPSCustomClock::Set(MAPSProperty& p, MAPSInt64 value)
{
	if (&p == &Property("timespeed")) {
		if (MAPS::IsRunning()) {
			//The required timespeed has just changed.
			m_MyTimeMonitor.Lock();
			MAPSTimestamp t = MAPS::GetSystemAccurateTiming();
			MAPSTimestamp x = t-m_TimebaseX;
			MAPSTimestamp offset = x*m_Timespeed;
			m_Timebase = m_Timebase + offset/1000;
			m_TimebaseX = t;
			m_Timespeed = (int)value;
			// Use this function to declare the maximum "real" timespeed that your clock will impose.
			SetAbsoluteTimeSpeed((int)value);
			m_MyTimeMonitor.Release();
		}
	}
	MAPSComponent::Set(p,value);
}

//There is really nothing to do in there...
void MAPSCustomClock::Birth()
{
}

void MAPSCustomClock::Death()
{
}

void MAPSCustomClock::Core()
{
	Wait4Event(isDyingEvent);
}

