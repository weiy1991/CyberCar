////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_vectorizer_sample_3_resampling_H
#define _Maps_vectorizer_sample_3_resampling_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSvectorizer_sample_3_resampling : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSvectorizer_sample_3_resampling)
private :
	// Place here your specific methods and attributes
	MAPSTimestamp	m_appointment;
	int				m_val1;
	int				m_val2;
};

#endif
