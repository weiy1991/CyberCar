////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/* Chapter 6 - Multi threaded components.
 * This section demonstrates how to write components that run
 * multiple threads. This is quite rare but can happen in case
 * of interfacing parallel prots in full duplex for example
 * (one thread has to be waiting on a blocking StartReading call to
 * get data on the component input, and a second thread has to be
 * waiting on a blocking call to read incoming data on the bus).
 */

/*! \ingroup Chapter6
 * \file
 * \brief MultiThread
 * This component demonstrates the creation of a new thread in a component. Multi-threading
 * must be used with care, because the user must take great care of data protection against
 * "simultaneous" access to data by multiple threads. When you need a multi-thread behaviour
 * in a component, you should first be sure that this behaviour cannot be decomposed into 
 * more simple mono-thread components, connected to each other by RTMaps inputs/outputs.
 * The following component has already one thread (it is defined as a threaded component with 
 * the MAPS::Threaded constant in MAPS_COMPONENT_DEFINITION macro), and launch another thread
 * to execute some job in a multi-task way. For example, in order to write a component 
 * interfacing a bus (a CAN bus for example), one thread may be used to read from the bus
 * and write to a RTMaps output, another thread may be use to read from an RTMaps input and
 * write to the bus.
*/

#include "maps_MultiThread.h"


// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSMultiThread)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the output (here, no outputs are present)
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSMultiThread)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties (here, no properties are present)
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSMultiThread)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, no actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSMultiThread)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Multithread) behaviour
MAPS_COMPONENT_DEFINITION(MAPSMultiThread,"MultiThread","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  0, // number of inputs
			  0, // number of outputs
			  0, // number of properties
			  0) // number of actions


//////////////////////////////////////
// Methods executed by the main component thread

// The main thread call at its birth the Birth() method (when you run RTMaps application)
void MAPSMultiThread::Birth()
{
	ReportInfo("Main thread birth");
	// The main thread creates the new thread
	// Prefer using this MAPSComponent::CreateThread function
	// instead of the OS specific one, so that the RTMaps engine
	// can know that this component owns 2 threads, and won't call
	// the Death method at shutdown until all the component threads
	// have exited. (and the code is portable !)
	CreateThread((MAPSThreadFunction)&MAPSMultiThread::NewThreadMethod);	
}

// Then it loops into the Core() method
void MAPSMultiThread::Core() 
{
	Rest(500000);
	ReportInfo("Main thread : hello !");
}

// The Death() method is called into the main thread once the second thread has died.
void MAPSMultiThread::Death()
{
	ReportInfo("Main thread death");
}

//////////////////////////////////////
// Method executed by the second thread
void MAPSMultiThread::NewThreadMethod()
{
	while (!IsDying()) 
	{
		Rest(1000000);
		ReportInfo("Second thread : hello !");
	};
	ReportInfo("Secondary thread is dying...");
}				
