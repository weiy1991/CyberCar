////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter5
 * \file
 * \brief UserStructureOutput
 * This sample demonstrates how to generate and write user defined structures. 
 * Components can exchange data of any type, as soon as these do not contain 
 * any code or pointer. User may experience a lot of problems if they try to exchange 
 * objects instead of structured data.
*/

#include "maps_UserStructureOutput.h"	// Includes the header of this component
#include "UserDefinitions.h"
#include <cstdlib>
#include <cstdio>

// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSUserStructureOutput)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSUserStructureOutput)
    // We define the component output, and type it as a MAPS::Structure named 
    // "MyNewStructure". RTMaps will allocate the output buffer (I/O elements)
    // using the size specified by sizeof(MyNewStructure)
    MAPS_OUTPUT_USER_STRUCTURE("oMyStructure",MyNewStructure)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties (here, no properties are present)
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSUserStructureOutput)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, no actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSUserStructureOutput)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (UserStructureOutput) behaviour
MAPS_COMPONENT_DEFINITION(MAPSUserStructureOutput,"UserStructureOutput","1.0",128,MAPS::Threaded,MAPS::Threaded,0,1,0,0)

void MAPSUserStructureOutput::Birth()
{
}

void MAPSUserStructureOutput::Core() 
{
	// Get the I/O element data for writing
	MAPSIOElt *ioElt=StartWriting(Output("oMyStructure"));
	// Is it a valid I/O element ?
	if (ioElt==NULL)
		return;
	// Get the structure contained in the data. We force the cast.
	// This is OK if the size allocated for the data is correct (sizeof(MyNewStructure))
	// Users may be very carefull on this point.
	MyNewStructure &myStruct=*static_cast<MyNewStructure *>(ioElt->Data());
	// Write some random data in this user defined structure
	myStruct.m_value=std::rand();
	sprintf(myStruct.m_message,"From %s",Name());
	// Send the IO element
	StopWriting(ioElt);
	// And sleep during 1 second
	MAPS::Sleep(1000000);
}

void MAPSUserStructureOutput::Death()
{
}
