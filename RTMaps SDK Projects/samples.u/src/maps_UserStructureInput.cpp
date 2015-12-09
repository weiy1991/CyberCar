////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter5
 * \file
 * \brief UserStructureInput
 * This sample demonstrates how to read user defined structures. 
 * Components can exchange data of any type, as soon as these do not contain 
 * any code or pointer. User may experience a lot of problems if they try to exchange 
 * objects instead of structured data.
*/

#include "maps_UserStructureInput.h"	// Includes the header of this component
#include "UserDefinitions.h"

// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSUserStructureInput)
    MAPS_INPUT("iMyStructure",MAPSFilterMyNewStructure,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs (here, no outputs are present)
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSUserStructureInput)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties (here, no properties are present)
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSUserStructureInput)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, no actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSUserStructureInput)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (UserStructureInput) behaviour
MAPS_COMPONENT_DEFINITION(MAPSUserStructureInput,"UserStructureInput","1.0",128,MAPS::Threaded,MAPS::Threaded,1,0,0,0)

void MAPSUserStructureInput::Birth()
{
	// Nothing to do in the Birth() method
}

void MAPSUserStructureInput::Core() 
{
	// Get the I/O element data for reading
	MAPSIOElt *ioElt=StartReading(Input("iMyStructure"));
	// Is it a valid I/O element ?
	if (ioElt==NULL)
		return;
	// Get the structure contained in the data. We force the cast.
	// RTMaps input/output typing is the way user can prevent bad data exchanges
	MyNewStructure &myStruct=*static_cast<MyNewStructure *>(ioElt->Data());
	// Reports the structure content in the console window.
	MAPSStreamedString str;
	str<<"Received : Value="<<myStruct.m_value<<" - Message = "<<myStruct.m_message;
	ReportInfo(str);
}

void MAPSUserStructureInput::Death()
{
	// Reports this information to the rtmaps console
	ReportInfo("UserStructureInput : Passing through Death() method");
}
