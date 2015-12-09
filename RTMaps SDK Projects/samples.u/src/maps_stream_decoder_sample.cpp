////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \defgroup Chapter7 Chapter 7 - Stream.
 * This section demonstrates how to write components that
 * deal with stream data (packets of bytes).
 * Usually, such stream data contain messages that have to be parsed
 * but the packets may not contain entire messages (a packet can start in the
 * middle of a message, and end in the middel of the next one), hence the
 * need for a way to push the packets into an internal ring buffer, and
 * pop data whan a complete message has been exploited.
*/

/*! \ingroup Chapter7
 * \file
 * \brief Stream8IOSample
 * This component is a template for RTMaps 8 bits serial I/O streams generation and processing 
 * (RS232/RS422 sensors or sound processing for example). This component inherits from the class 
 * MAPSStream8IOComponent. It means that MAPSStream8IOComponent provides to this component its 
 * basic behaviour and a set of standard inputs, outputs, properties and actions.
 * In this component, we will also learn how to use the MAPSRBRegion API in order to parse 
 * a stream correctly (detecting start of frames, end of frames, and using the internal ring buffer
 * of the MAPSStream8IOComponent template).
*/

#include "maps_stream_decoder_sample.h"

MAPS_BEGIN_INPUTS_DEFINITION(MAPSStream8IOSample)
MAPS_END_INPUTS_DEFINITION

MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSStream8IOSample)
	//Let's imagine we will parse a stream containing the vehicle speed information.
	MAPS_OUTPUT("veh_speed",MAPS::Float64,NULL,"km/h",1)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSStream8IOSample)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSStream8IOSample)
MAPS_END_ACTIONS_DEFINITION

MAPS_COMPONENT_DEFINITION(MAPSStream8IOSample,"stream_decoder_sample","1.0",128,MAPS::Sequential|MAPS::Threaded,
			  MAPS::Threaded,0,1,0,0) //Let's declare the output mentionned above.

void MAPSStream8IOSample::Birth()
{
    // Always call the MAPSStream8IOComponent::Birth() method first (the parent of this component)
    MAPSStream8IOComponent::Birth();
    // Make your own initialisations here
    // For example sends an initialisation stream to the component output oStream8
    const char* initStream ="SENSOR START";
	SendData(initStream,MAPS::Strlen(initStream));
}

// The Core() method
void MAPSStream8IOSample::Core()
{
    // Always call the MAPSStream8IOComponent::Core() method first (the parent of this component)
    MAPSStream8IOComponent::Core();
    // Most of the time, you don't have to put code in the Core() method
}

// The Death() method
void MAPSStream8IOSample::Death()
{
    // Make your own closure here
    
    // Always call the MAPSStream8IOComponent::Death() method at the end (the parent of this component)
    MAPSStream8IOComponent::Death();
}

// The main function for a RTMaps 8 bits serial I/O streams generation and processing component. This method
// is called each time a stream data  has been read and stacked by the parent class MAPSStream8IOComponent into data parameter. 
// All information concerning data timings are kept into 2 other stacks given in the parameters timestamp and timeOfIssue
// The region parameter contains the region of available data into the ring-buffers data, timestamp and timeOfIssue. A set of method 
// of the class MAPSRBRegion (for MAPS Ring Buffer Region) can be used to access the data. You have to refer to the MAPSRBRegion
// reference documentation in order to get more information on these methods.
/* Let's imagine the following protocol emitted by the sensor (via RS232, IP sockets, or other)
 * 
 *	Bytes	:	Data type (little endian)	:	Content		
 *--------------------------------------------------------------------------------------------------
 *	1 to 2	:	unsigned short				:	Header (or message ID) - constant - 0x3048			
 *	3 to 4	:	unsigned short				:	Data length	(header and DLC excluded) - constant - 0x0004 (MSG_DATA_LENGTH)				
 *	4 to 8	:	signed int					:	Vehicle speed in km/h	
 */

void MAPSStream8IOSample::NewDataCallback(MAPSRBRegion &region,const unsigned char *data, const MAPSTimestamp *timestamp, const MAPSTimestamp *timeOfIssue)
{
	// Declares an index to the ring-buffer
	int			offset=0, lastOffset=0;
	// Get the size of the ring-buffer
	const int	regionSize=region.Size();
	// Just for info...
	ReportInfo("New packet of data received.");

	//Let's write a simple state machine.
	int state = 0;
	unsigned char c;

	unsigned short dlc;
	// Browse the bytes in the internal ring buffer (e.g. bytes that have not been removed yet + bytes that
	// have just arrived and that have just been stacked in the internalt ring buffer.
	while(offset<regionSize)
	{
		c = data[region[offset]];
		offset++;
		switch(state) 
		{
			case 0: //Looking for message ID (0x3048)- first byte.
				if (c==0x48) {//Remember we are in little-endian
					//Let's store the timestamp of this byte.
					//If it is indeed a message start byte, we can consider it is the most
					//accurate timestamp we can measure for this message.
					_msg_start_timestamp = timestamp[region[offset-1]];
					//Let's jump to state 2 in order to look for second header byte.
					state++; 
				} else {
					lastOffset = offset;
				}
				break;
			case 1: //Looking for second byte of the message id.
				if (c==0x30) { 
					state++; //At this point, we've found the 2 header bytes.
				} else {
					lastOffset = offset; 
					state = 0;
				}
				break;
			case 2 : 
				dlc = c;
				state++;
				break;
			case 3 :
				dlc += c<<8;
				if (dlc == MSG_DATA_LENGTH) { //Check that datalength equals 4.
					state++;
				} else {
					lastOffset = offset;
					state = 0;
				}
				break;
			case 4:  //Here we are with the message data.
				{
					if (regionSize - offset + 1 >= MSG_DATA_LENGTH ) //Let's check that there is enough data
																 //stacked in the buffer for a complete message 
					{
						//Let's copy the complete message to a contiguous memory buffer:
						MAPSRBRegion potentialMessageRegion;
						region.SubRegion(potentialMessageRegion,MAPSConstRBRegionSubRegionFromBeginning,offset-1,MSG_DATA_LENGTH);
						potentialMessageRegion.CopyFromRB(data,_msg_data,MSG_DATA_LENGTH);
						//We now have a complete single message in a contiguous buffer. Let's deal with it.
						DecodeMessageData();
						//Increment the offset and lastOffset to jump to the end of the message and start looking
						//for a new one.
						offset += MSG_DATA_LENGTH - 1;
						lastOffset = offset;
						state = 0;
					} else 
					{ //Message is incomplete. Do not increment lastOffset so that the first part of the message
						//will remain in the internal ring buffer.
						break;
					}
				}
				break;
		}
	}
    // Unpop all data from the ring-buffer (FIFO)
    region.Pop(lastOffset);
}

void MAPSStream8IOSample::DecodeMessageData()
{
	//The data part of a single complete message is stored in _msg_data.
	//Let's output the value...
	MAPSInt32 value = *(MAPSInt32*)_msg_data;

	MAPSIOElt* ioEltOut = StartWriting(Output(0));
	ioEltOut->Integer32() = value;
	ioEltOut->Timestamp() = _msg_start_timestamp;
	StopWriting(ioEltOut);

	//Just for info
	MAPSStreamedString str;
	str << "We've found a message. Vehicle speed is " << value << " km/h.";
	ReportInfo(str);

}
