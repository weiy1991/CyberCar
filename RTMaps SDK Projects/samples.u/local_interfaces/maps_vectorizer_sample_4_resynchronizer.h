////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_vectorizer_sample_4_resynchronizer_H
#define _Maps_vectorizer_sample_4_resynchronizer_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSvectorizer_sample_4_resynchronizer : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSvectorizer_sample_4_resynchronizer)
private :
	// Place here your specific methods and attributes
	MAPSIOElt*	m_ioElts[2];
	MAPSInput*	m_inputs[2];
	MAPSDelay	m_synch_tolerance;
};

#endif
