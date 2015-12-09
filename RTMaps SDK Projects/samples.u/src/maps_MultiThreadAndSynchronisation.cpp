////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter6
 * \file
 * \brief MultiThreadAndSynchronisation
 * This component completes the MAPSMultiThread sample with thread synchronisation. Multi-threading
 * must be used with care, because the user must take great care of data protection against
 * "simultaneous" access to data by multiple threads. When you need a multi-thread behaviour
 * in a component, you should first be sure that this behaviour cannot be decomposed into 
 * more simple mono-thread components, connected to each other by RTMaps inputs/outputs.
 * Once your are sure that your component need a multi-thread architecture, you can use multiple 
 * RTMaps synchronisation structures to help you protect and ensure your component to be multi-thread
 * safe. These structures are :
 * 		- MAPSEvent : events are objects you can use to signal from one thread to another that some
 * 					  task has been completed, or more generally an event has occured.
 * 		- MAPSMutex : mutex are objects you can use to protect some data from a simultaneous multi-thread
 * 					  access.
 * In this sample, the main thread reads some data from its input, and write it in local FIFO. The second 
 * thread periodically collect all data in the FIFO and send it to its output. We protect the FIFO with a
 * MAPSMutex object.
*/

#include "maps_MultiThreadAndSynchronisation.h"	// Includes the header of this component

// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSMultiThreadAndSynchronisation)
    MAPS_INPUT("input",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs (here, no outputs are present)
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSMultiThreadAndSynchronisation)
    MAPS_OUTPUT("output",MAPS::Integer32,NULL,NULL,10)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties (here, no properties are present)
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSMultiThreadAndSynchronisation)
    MAPS_PROPERTY("pLocalFIFOSize",50,false,false)
    MAPS_PROPERTY("pPopPeriod",1000000,false,true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, no actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSMultiThreadAndSynchronisation)
MAPS_END_ACTIONS_DEFINITION

// 1.1: -removed test on the result of new.
// Use the macros to declare this component (MultiThreadAndSynchronisation) behaviour
MAPS_COMPONENT_DEFINITION(MAPSMultiThreadAndSynchronisation,"MultiThreadAndSynchronisation","1.1",128,
			  MAPS::Threaded,MAPS::Threaded,
			  1, // number of inputs
			  1, // number of outputs
			  2, // number of properties
			  0) // number of actions


// The main thread call at its birth the Birth() method (when you run RTMaps application)
void MAPSMultiThreadAndSynchronisation::Birth()
{
	ReportInfo("Main thread birth");
	// Reset the ready event
	m_popThreadReadyEvent.Reset();
	// Initialize the FIFO
	m_fifo.Init((int)GetIntegerProperty("pLocalFIFOSize"),0,0);
	m_fifoData=new int[m_fifo.FifoSize()];
	// The main thread creates the new thread
	CreateThread((MAPSThreadFunction)&MAPSMultiThreadAndSynchronisation::PopThread);	
	ReportInfo("Main thread : waiting until second thread is ready");
	// Wait for the ready event to be set by the second thread
	m_popThreadReadyEvent.Wait();
	ReportInfo("Main thread : 2nd thread is ready...");
}

// Then it loops into the Core() method
void MAPSMultiThreadAndSynchronisation::Core() 
{
	// Get the I/O buffer element containing the new data
	MAPSIOElt* iIOElt=StartReading(Input("input"));
	// Check the returned pointer
	if (iIOElt==NULL)
		return;
	// Lock the mutex
	m_fifoMutex.Lock();
	// We can safely use the FIFO
	// Declares a temporary region that will localise where to add data into the FIFO (ring-buffer)
	// To get more info about MAPSRBRegion class, see the RTMaps SDK reference
	MAPSRBRegion		newDataRegion;
	MAPSConstRBRegionState	state;
	// Localise where data has to be pushed into the FIFO
	state=m_fifo.Push(iIOElt->VectorSize(),newDataRegion);
	if (!MAPSRBREGION_NO_ERRORS(state))
		ReportInfo(MAPSRBRegion::StateToString(state)); // There were an error
	else
	{
		if (!MAPSRBREGION_NO_WARNINGS(state))
			ReportInfo(MAPSRBRegion::StateToString(state)); // There were a warning, this is not
		// fatal for the current operation
		// Apply the push by a copy of input data into the FIFO...
		newDataRegion.CopyToRB(m_fifoData,&iIOElt->Integer32(),iIOElt->VectorSize());
	}
	// Release the mutex
	m_fifoMutex.Release();
}

// The Death() method is called into the main thread once the second thread has died.
void MAPSMultiThreadAndSynchronisation::Death()
{
	ReportInfo("Main thread death");
	if (NULL != m_fifoData)
	{
		delete [] m_fifoData;
		m_fifoData=NULL;
	}
}

//////////////////////////////////////
// Method executed by the second thread
void MAPSMultiThreadAndSynchronisation::PopThread()
{
	// Wait for a while, to illustrate events use
	MAPS::Sleep(2000000);
	ReportInfo("Second thread : about to set the ready event");
	m_popThreadReadyEvent.Set();
	while (!IsDying()) 
	{
		bool done=false;

		// Sleep for some user defined period
		Rest(GetIntegerProperty("pPopPeriod"));
		//  Ask for writing a new data to the output
		MAPSIOElt		*oIOElt= StartWriting(Output("output"));
		//	Check if oIOElt is valid before doing anything with
		if (oIOElt==NULL)
			return;

		// Lock the mutex
		m_fifoMutex.Lock();
		// We can safely use the FIFO
		MAPSRBRegion			popRegion;
		MAPSConstRBRegionState	state;
		// Want to get as many data as possible from the fifo (oIOElt->BufferSize() contains the
		// max nb of data we can get)/
		state=m_fifo.Pop(oIOElt->BufferSize(),&popRegion);
		if (!MAPSRBREGION_NO_ERRORS(state))
			ReportInfo(MAPSRBRegion::StateToString(state)); // There were an error
		else
		{
			if (!MAPSRBREGION_NO_WARNINGS(state))
				ReportInfo(MAPSRBRegion::StateToString(state)); // There were a warning, this is not
			// fatal for the current operation
			if (popRegion.Size()>0)
			{
				popRegion.CopyFromRB(m_fifoData,&oIOElt->Integer32(),oIOElt->BufferSize());
				done=true;
			}
		}
		// Release the mutex
		m_fifoMutex.Release();
		//  And send the data if the work has been done...
		StopWriting(oIOElt,!done);
	}
	ReportInfo("Secondary thread is dying...");
}				
