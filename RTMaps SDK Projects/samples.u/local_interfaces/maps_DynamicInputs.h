////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_MultipleInputs_H
#define _Maps_MultipleInputs_H

#include "maps.hpp"


class MAPSDynamicInputs : public MAPSComponent 
{
	// Use standard header definition macro without constructor nor destructor.
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSDynamicInputs)
private :
	int m_nbInputs;

	void Dynamic();
	
};

#endif
