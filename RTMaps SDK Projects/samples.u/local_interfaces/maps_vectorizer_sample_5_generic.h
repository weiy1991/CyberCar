////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _Maps_vectorizer_sample_5_generic_H
#define _Maps_vectorizer_sample_5_generic_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSvectorizer_sample_5_generic: public MAPSComponent {
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSvectorizer_sample_5_generic);
	MAPS_COMPONENT_DYNAMIC_HEADER_CODE(MAPSvectorizer_sample_5_generic);

    MAPSArray<MAPSInput*>	m_inputs;
    MAPSArray<bool>			m_gotData;
    MAPSArray<double>		m_data;
    bool					m_dataReady;
	bool					m_ignoreTriggerInputContent;
	int						m_samplingMode;
	int						m_nbInputs;
    MAPSArray<MAPSIOElt*>	m_synchroIOElts;
    MAPSTimestamp			m_samplingPeriod;
	MAPSDelay				m_synchroTolerance;
    MAPSTimestamp			m_appointment;
};

#endif //_Maps_vectorizer_sample_5_generic_H