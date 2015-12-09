////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_vectorizer_sample_2_reactive_H
#define _Maps_vectorizer_sample_2_reactive_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSvectorizer_sample_2_reactive : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSvectorizer_sample_2_reactive)
private :
	// Place here your specific methods and attributes
	MAPSInput*	m_inputs[2];

	int			m_val1;
	int			m_val2;
};

#endif
