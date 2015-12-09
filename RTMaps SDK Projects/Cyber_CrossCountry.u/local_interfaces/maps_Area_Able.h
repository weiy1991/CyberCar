////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Area_Able_H
#define _Maps_Area_Able_H

// Includes maps sdk library header
#include "maps.hpp"
#include <sys/timeb.h>
using namespace cv;
using namespace std;
// Declares a new MAPSComponent child class
class MAPSArea_Able : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSArea_Able)
private :
	/*MAPSIOElt* ioEltIn_DMap;
	MAPSIOElt* ioEltIn_RImage;

	MAPSIOElt* Out_info;
	MAPSIOElt* ioEltOut;*/

	/*IplImage imageIn_DMap;
	IplImage imageIn_RImage;
	IplImage imageOut;*/
	
	MAPSTimestamp m_nextTimestamp;//time control for the image
	int sleepTime;
	bool m_firstTime;

	//time estimation
	struct timeb startTime_V,endTime_V;
	float time_V;
	
	int HoughLine(Mat mat,int pos,int threshold);
};

#endif
