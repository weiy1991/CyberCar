////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_MultiThreadAndSynchronisation_H
#define _Maps_MultiThreadAndSynchronisation_H

// Includes maps sdk library header
#include "maps.hpp"
#include "MAPSRBRegion.h"

// Declares a new MAPSComponent child class
class MAPSMultiThreadAndSynchronisation : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSMultiThreadAndSynchronisation)
private :
	
	void PopThread();

	MAPSEvent		m_popThreadReadyEvent;

	MAPSMutex		m_fifoMutex;
	MAPSRBRegion	m_fifo;
	int* 			m_fifoData;

};

#endif
