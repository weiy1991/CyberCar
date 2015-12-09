////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter5
 * \file
 * \brief Counter
 * This is a very simple component : the Counter component.
 * It demonstrates how to periodically output data, by pausing
 * the component thread for a certain amount of time with the Wait function.
 * Reminder:
 *	- Rest(MAPSDelay t); pauses for the specified duration (in microseconds), based on the RTMaps virtual time.
 *	- MAPS::Sleep(MAPSDelay t); pauses for the specified duration (in microseconds, based on the "real" time (idenpendently of the replay time speed).
 * 	- Wait(MAPSTimestamp t); pauses until the given date in the RTMaps timebase (or doesn't pause at all if the date is overdue).
 */
#include "maps_counter.h"	

// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSCounter)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the output
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSCounter)
    MAPS_OUTPUT("output",MAPS::Integer32,NULL,NULL,1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSCounter)
    MAPS_PROPERTY("pStartTime",0,false,false)
    MAPS_PROPERTY("pPeriod",1000000,false,true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, no actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSCounter)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Counter) behaviour
MAPS_COMPONENT_DEFINITION(MAPSCounter,"Counter","1.0",255,
			  MAPS::Threaded,MAPS::Threaded,
			  0, // number of inputs
			  1, // number of outputs
			  2, // number of properties
			  0) // number of actions

//  The Birth() method
void MAPSCounter::Birth()
{    
    //  Initialise the appointment variable
    m_appointment=(int)GetIntegerProperty("pStartTime");
    //  Initialisation of the counter variable
    m_ticks=0;
}

void MAPSCounter::Core() 
{
	//  Wait for the next appointment
	Wait(m_appointment);

	//  Declaration of a new input/output data exchange structure
	MAPSIOElt		*ioElt;
	//  Ask for writing a new data to the output "output"
	ioElt= StartWriting(Output("output"));
	//	Check if ioElt is valid before doing anything with
	if (NULL==ioElt)
		return;
	//  We've got a new data where to write, so write to it the ticks value
	ioElt->Integer32()=m_ticks;
	//  And send the data to connected inputs (if there are)
	StopWriting(ioElt);

	//  According to the parity of ticks variable
	if (m_ticks%2==0)
		//  Tells "tic"
		ReportInfo("tic !");
	else
		//  or "toc"
		ReportInfo("toc !");

	//  Increments our counter
	m_ticks++; // ticks=ticks+1;

	//  Define the new appointment : we get user-defined "pPeriod" property
	m_appointment=m_appointment+GetIntegerProperty("pPeriod");
}

void MAPSCounter::Death()
{
}
