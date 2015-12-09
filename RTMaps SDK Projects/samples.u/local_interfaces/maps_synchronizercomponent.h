////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _MAPS_SYNCHRONIZER_COMP_H
#define _MAPS_SYNCHRONIZER_COMP_H

#include "maps.hpp"
#include "maps_synchronizer.h"


class MAPSSynchronizerComponent : public MAPSComponent
{

    MAPS_COMPONENT_HEADER_CODE_WITHOUT_CONSTRUCTOR(MAPSSynchronizerComponent);
    MAPSSynchronizerComponent(const char* componentName, MAPSComponentDefinition& cd);
    virtual ~MAPSSynchronizerComponent() {}
    void Set(MAPSProperty& p, MAPSInt64 value);
private :
    //This is the object that will be use to synchronize the RTMaps main clock.
    MAPSSynchronizer* m_Synchronizer;

    MAPSEvent m_TrigEvt;

    MAPSTimestamp m_Period;
    MAPSTimestamp m_Increment;

};
#endif //_MAPS_SYNCHRONIZER_COMP_H
