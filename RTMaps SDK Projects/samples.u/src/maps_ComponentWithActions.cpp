////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/* Chapter 5 - Miscellaneous functions.
 * This section demonstrates several additional functions
 * that can be helpful such as:
 *	- how to write Actions
 *	- how to dynamically create inputs, outputs and properties
 *  - how to use properties of type Enumeration
 *  - how to use personalized data structures in MAPSIOElts
 */


/*! \ingroup Chapter5
 * \file
 * \brief ComponentWithActions
 * This component demonstrates the use of actions in a RTMaps component.
 * Actions are procedures (without parameters) the user can call to activate one component
 * behaviour. Actions can be available either in run mode only, or in both run and standby
 * mode (i.e. when the diagram is executing or not). 
 * You can call actions in RTMaps Studio by a right clicking on your component.
*/

#include "maps_ComponentWithActions.h"


// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSComponentWithActions)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the output (here, no outputs are present)
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSComponentWithActions)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties (here, no properties are present)
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSComponentWithActions)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, 2 actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSComponentWithActions)
	MAPS_ACTION("aSpeedUp",MAPSComponentWithActions::SpeedUp)
	MAPS_ACTION("aSpeedDown",MAPSComponentWithActions::SpeedDown)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Action) behaviour
MAPS_COMPONENT_DEFINITION(MAPSComponentWithActions,"ComponentWithActions","1.0",255,
			  MAPS::Threaded,MAPS::Threaded,
			  0, // number of inputs
			  0, // number of outputs
			  0, // number of properties
			  2) // number of actions


//This static function is called when the corresponding 
//action is requested from the RTMaps control interface.
void MAPSComponentWithActions::SpeedUp(MAPSModule *module,int actionNb)
{
	// The function is static. The 'module' pointer provided as an argument
	// enables to call non-static functions or access non-static member variables
	// of the component the action was triggered from.

	if (MAPS::IsRunning())
	{
		// This action is only supported in run mode.
		MAPSComponentWithActions &comp=*(MAPSComponentWithActions *)module;
		// Decrease the period
		comp.SetPeriod(comp.GetPeriod()/2);
	}
}

//This static function is called when the corresponding 
//action is requested from the RTMaps control interface.
void MAPSComponentWithActions::SpeedDown(MAPSModule *module,int actionNb)
{
	// The function is static. The 'module' pointer provided as an argument
	// enables to call non-static functions or access non-static member variables
	// of the component the action was triggered from.

	if (MAPS::IsRunning())
	{
		// This action is only supported in run mode.
		MAPSComponentWithActions &comp=*(MAPSComponentWithActions *)module;
		// Increase the period
		comp.SetPeriod(comp.GetPeriod()*2);
	}
}

void MAPSComponentWithActions::SetPeriod(int newPeriod)
{
	if (newPeriod>0)
	{
		m_period=newPeriod;
		MAPSStreamedString sx;
		sx << "Period is now " << m_period << " microseconds.";
		ReportInfo(sx);
	}
	else
		ReportWarning("Period change failed");
}

void MAPSComponentWithActions::Birth()
{
    // Initialize the sleeping period to 1 second
    SetPeriod(1000000);
}													

void MAPSComponentWithActions::Core() 
{
    Rest(m_period);
    ReportInfo("tic");
    Rest(m_period);
    ReportInfo("toc");
}


void MAPSComponentWithActions::Death()
{
}
