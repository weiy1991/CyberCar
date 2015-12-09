////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter4 
 * \file
 * \brief CANWriter
 * This component demonstrates the emission of CAN frames
 * To test this component, display the CAN frames into a
 * CANFrameViewer component (available in the "Viewers" section).
*/

#include "maps_CANWriter.h"	// Includes the header of this component

// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSCANWriter)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs (here, no outputs are present)
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSCANWriter)
    MAPS_OUTPUT("oCAN1",MAPS::CANFrame,NULL,NULL,1)
    MAPS_OUTPUT("oCAN2",MAPS::CANFrame,NULL,NULL,2)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties (here, no properties are present)
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSCANWriter)
    MAPS_PROPERTY("pId1",123,false,true)	// Identifier of the frames sent on oCAN1
    MAPS_PROPERTY("pExt1",false,false,true)	// Type of frame (STD/EXT) on oCAN1 : STD by default
    MAPS_PROPERTY("pId2",456,false,true)	// Identifier of the frames sent on oCAN1
    MAPS_PROPERTY("pExt2",true,false,true)	// Type of frame (STD/EXT) on oCAN1 : EXT by default
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, no actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSCANWriter)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (CANWriter) behaviour
MAPS_COMPONENT_DEFINITION(MAPSCANWriter,"CANWriter","1.0",128,MAPS::Threaded,MAPS::Threaded,0,2,4,0)

void MAPSCANWriter::Birth()
{
}

void MAPSCANWriter::Core() 
{
	// Wait for 100 "real-milliseconds"
	MAPS::Sleep(100000);

	// First sample of CAN frame writing : write a single CAN frame to output oCAN1
	// Declare a new MAPSIOElt
	MAPSIOElt *ioEltOutput1;
	// Start writing a new data on oCAN1 output, get the corresponding MAPSIOElt
	ioEltOutput1=StartWriting(Output("oCAN1"));

	// We get the frame where to write the CAN data
	MAPSCANFrame &frame1=ioEltOutput1->CANFrame();
	// Set the frame identifier (adress) on the CAN bus
	frame1.arbitrationId=(long)(GetIntegerProperty("pId1")|(GetBoolProperty("pExt1")?MAPSCANFrame::ExtendedId:MAPSCANFrame::StandardId));

	// This is not a remote frame
	frame1.isRemote=false;
	// Set the frame data length
	frame1.dataLength=1;
	// Set the data content
	frame1.data[0]=0;
	// Set the vector size to 1 (number of CAN frames contained in this MAPSIOElt). This is optionnal here
	// because the output has been defined with a maximum vector size of 1
	ioEltOutput1->VectorSize()=1;
	// Send the corresponding CAN frame now !
	StopWriting(ioEltOutput1);

	// Second sample of CAN frame writing : write a vector of CAN frames to output oCAN2
	// Declare a new MAPSIOElt
	MAPSIOElt *ioEltOutput2;
	// Start writing a new data on oCAN2 output, get the corresponding MAPSIOElt
	ioEltOutput2=StartWriting(Output("oCAN2"));

	// We get the frame where to write the CAN data : note that we get here a pointer to an array of 
	// CAN frames
	MAPSCANFrame *frames2=&(ioEltOutput2->CANFrame());
	// Set the frame identifier (adress) on the CAN bus
	frames2[0].arbitrationId=(long)(GetIntegerProperty("pId2")|(GetBoolProperty("pExt2")?MAPSCANFrame::ExtendedId:MAPSCANFrame::StandardId));
	// This is not a remote frame
	frames2[0].isRemote=false;
	// Set the frame data length
	frames2[0].dataLength=1;
	// Set the data content
	frames2[0].data[0]=1;
	// Write another CAN frame with a slightly different Id
	frames2[1].arbitrationId=(long)(GetIntegerProperty("pId2")+1|(GetBoolProperty("pExt2")?MAPSCANFrame::ExtendedId:MAPSCANFrame::StandardId));
	frames2[1].isRemote=false;
	frames2[1].dataLength=1;
	frames2[1].data[0]=2;

	// Set the vector size to 2 (number of CAN frames contained in this MAPSIOElt). 
	ioEltOutput2->VectorSize()=2;
	// Send the corresponding CAN frame now !
	StopWriting(ioEltOutput2);
}

void MAPSCANWriter::Death()
{
}
