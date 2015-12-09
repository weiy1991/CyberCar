////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_pathable_3dv_H
#define _Maps_pathable_3dv_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSpathable_3dv : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSpathable_3dv)
private :
	/*MAPSIOElt* ioEltIn;
	MAPSIOElt* ioEltOut;
	IplImage& imageIn;
	IplImage& imageOut;*/

	bool m_firstTime;

	void ThresholdImage(const IplImage& imageIn, IplImage& imageOut, unsigned char threshold);
};

#endif
