////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/* Chapter 4 - CAN frames handling.
 * This section demonstrates how to handle CAN frames
 * (using the MAPSCANFrame structure).
 */

/*! \ingroup Chapter4 
 * \file
 * \brief CANReader
 * This component demonstrates the reception of CAN frames
 * To test this component, you can use the RandomCANFrames component
 * which is able to generate CAN frames, or open one of the RTMaps databases
 * containing CAN bus data into a Player component.
*/

#include "maps_CANReader.h"	// Includes the header of this component

// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSCANReader)
    MAPS_INPUT("iCAN",MAPS::FilterCANFrame,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs (here, no outputs are present)
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSCANReader)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties (here, no properties are present)
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSCANReader)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, no actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSCANReader)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (CANReader) behaviour
MAPS_COMPONENT_DEFINITION(MAPSCANReader,"CANReader","1.0",128,MAPS::Threaded|MAPS::Sequential,MAPS::Threaded,1,0,0,0)

void MAPSCANReader::Birth()
{
}

void MAPSCANReader::Core() 
{
	// Declare a new MAPSIOElt pointer, read the next input CAN Frame, and get the corresponding MAPSIOElt
	MAPSIOElt *iIOEltCAN=StartReading(Input("iCAN"));
	// Always check a pointer returned by RTMaps
	if (NULL == iIOEltCAN)
		return;
	// It's Ok, the MAPSIOElt returned is valid

	// Get the array of frames encapsulated in the MAPSIOElt
	MAPSCANFrame *receivedFrame=&(iIOEltCAN->CANFrame());
	// Declare a MAPSStreamedString data to display the frames content in the Studio Console
	MAPSStreamedString str;
	// Display the number of frames included in the MAPSIOElt and the common timestamp of these frames
	str<<iIOEltCAN->VectorSize()<<" frame(s) received at timestamp : "<<(int)(iIOEltCAN->Timestamp()/1000)<<" ms";
	// For all frames
	for(int i=0;i<iIOEltCAN->VectorSize();i++)
	{
		str<<'\n';
		str<<"Id : ";
		// Display the identifier (Extended or Standard)
		if (receivedFrame[i].arbitrationId&MAPSCANFrame::ExtendedId)
			str<<"Ext";
		else
			str<<"Std";
		str<<".0x"<<MAPSSManip::hex<<(receivedFrame[i].arbitrationId&MAPSCANFrame::AddressMask);

		// Display the length of the frame
		str<<" - Length : "<<MAPSSManip::dec<<(int)receivedFrame[i].dataLength;
		// If the frame is a remote frame, display this info
		if (receivedFrame[i].isRemote)
			str<<" - Remote frame ";

		// Display all data bytes of the frame
		str<<MAPSSManip::hex;
		for(int j=0;j<receivedFrame[i].dataLength;j++)
			str<<" - "<<(int)receivedFrame[i].data[j];
	}
	// Send the string to the console as an information
	ReportInfo(str);
}

void MAPSCANReader::Death()
{
}
