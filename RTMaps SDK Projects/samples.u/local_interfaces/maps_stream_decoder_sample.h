////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _MAPSStream8IOSample_H
#define _MAPSStream8IOSample_H

#ifndef _MAPS_H
#include "maps.hpp"
#endif

#include "MAPSStream8IOComponent.h"


#define MSG_DATA_LENGTH 4 //Messages are made of 8 bytes with 2 header bytes, 2 data length bytes, and 4 data bytes.


class MAPSStream8IOSample: public MAPSStream8IOComponent
{
	MAPS_CHILD_COMPONENT_HEADER_CODE(MAPSStream8IOSample,MAPSStream8IOComponent);

	// This is the callback where to place input stream processing
	void	NewDataCallback(MAPSRBRegion &region,const unsigned char *data, const MAPSTimestamp *timestamp, const MAPSTimestamp *timeOfIssue);

	//An buffer we will use to store single messages in contiguous memory.
	MAPSTimestamp _msg_start_timestamp;
	unsigned char _msg_data[MSG_DATA_LENGTH];

	void DecodeMessageData();

};

#endif
