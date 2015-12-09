////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter8
 * \file
 * \brief SynchronizerComponent
 * A simple component demonstrating the use of the synchronizer object in RTMaps.
 * This component first retrieves the handle to the synchronizer object in the RTMaps kernel,
 * and then uses this object to adjust the RTMaps main clock when required.
 * Periodically, this component increments the current time with the value specified in the
 * "increment" property.
 * When changing the time specified in the "time" property, the component adjusts the current
 * time to new one.
 */

#include "maps_synchronizercomponent.h"

MAPS_BEGIN_INPUTS_DEFINITION(MAPSSynchronizerComponent)
MAPS_END_INPUTS_DEFINITION

MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSSynchronizerComponent)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSSynchronizerComponent)
    MAPS_PROPERTY("period",1000000,false,true)
    MAPS_PROPERTY("increment",500000,false,true)
    MAPS_PROPERTY("time",10000000,false,true)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSSynchronizerComponent)
MAPS_END_ACTIONS_DEFINITION

MAPS_COMPONENT_DEFINITION(MAPSSynchronizerComponent,"Synchronizer","1.0",128,MAPS::Threaded,
			  MAPS::Threaded,0,0,3,0)

//Implementation of the constructor (called when the component is placed on a diagram.
MAPSSynchronizerComponent::MAPSSynchronizerComponent(const char* componentName, MAPSComponentDefinition &cd) :
MAPSComponent(componentName,cd)
{
	m_Synchronizer = NULL;
}

void MAPSSynchronizerComponent::Set(MAPSProperty& p, MAPSInt64 value)
{
	if (&p==&Property("period")) {
		m_Period = value;
	} else if (&p == &Property("increment")) {
		m_Increment = value;
	} else if (&p == &Property("time")) {
		if (MAPS::IsRunning()) { // The component is running, so we can use the synchronizer object.
			if (m_Synchronizer) { //Always check we own the synchronizer before using it.
				m_Synchronizer->SignalSynchronizationCommand(value); //Set the RTMaps main clock to the time specified in value.
			}
		}
	}
	MAPSComponent::Set(p,value);
}

void MAPSSynchronizerComponent::Birth()
{
	m_Period = GetIntegerProperty("period");
	m_Increment = GetIntegerProperty("increment");

	//Try to retrieve the synchronizer handle from the RTMaps engine.
	//Since we cannot have 2 or more synchronizers within 1 diagram, this may return
	//NULL if the synchronizer object is already owned by someone else.
	MAPS::GetSynchronizer(this,&m_Synchronizer);
	if (m_Synchronizer == NULL)
		ReportWarning("Could not get synchronizer handle.");

}

void MAPSSynchronizerComponent::Core()
{
	m_TrigEvt.SetTrigger(m_Period);
	Wait4Event(m_TrigEvt);
	if (m_Synchronizer) { //Always check we own the synchronizer before using it.
		m_Synchronizer->SignalSynchronizationCommand(MAPS::CurrentTime()+m_Increment); //Set the RTMaps main clock to the time specified .
	}
}

void MAPSSynchronizerComponent::Death()
{
	//Once the synchronizer is not needed anymore, we MUST release it,
	//so that someone else can handle it.
	if (m_Synchronizer) {
		MAPS::ReleaseSynchronizer(this,&m_Synchronizer);
		m_Synchronizer=  NULL;
	}
}


