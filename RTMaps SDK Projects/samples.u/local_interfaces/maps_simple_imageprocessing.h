////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_simple_imageprocessing_H
#define _Maps_simple_imageprocessing_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSsimple_imageprocessing : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSsimple_imageprocessing)
private :
	// Place here your specific methods and attributes

	bool m_firstTime;

	void ThresholdImage(const IplImage& imageIn, IplImage& imageOut, unsigned char threshold);
};

#endif
