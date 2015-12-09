////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter5
 * \file
 * \brief EnumProperties
 * A simple component demonstrating the use of the new "Enumeration" property type : a property that can deal with a finite number of string values.
 */

#include "maps_enum_props.h"

MAPS_BEGIN_INPUTS_DEFINITION(MAPSEnumProperties)
MAPS_END_INPUTS_DEFINITION

MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSEnumProperties)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSEnumProperties)
    /*Macro declaring an enum property :
    * Param 1 : property name
    * Param 2 : initial available values
    * Param 3 : default selected value index
    * Param 4 : has to be specified before run
    * Param 5 : can be changed after run
    */
    MAPS_PROPERTY_ENUM("enumpropName","stringValue1|stringValue2|stringValue3",0,false,true)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSEnumProperties)
MAPS_END_ACTIONS_DEFINITION

MAPS_COMPONENT_DEFINITION(MAPSEnumProperties,"EnumProperties","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  0, 
			  0, 
			  1, 
			  0)

//When changing the selection in an enum property, this change can be performed either by ...
//.. selecting an index in the list...
void MAPSEnumProperties::Set(MAPSProperty &p, MAPSInt64 value)
{
	MAPSComponent::Set(p,value);
	if (&p == &Property("enumpropName")) {
		UpdateCurrentPropValue();	
	}
}

//... selecting the desired string ...
void MAPSEnumProperties::Set(MAPSProperty &p, const MAPSString& value)
{
	MAPSComponent::Set(p,value);
	if (&p == &Property("enumpropName")) {
		UpdateCurrentPropValue();
	}
}

//... or specifying the entire enumerations structures.
void MAPSEnumProperties::Set(MAPSProperty &p, MAPSEnumStruct enumStruct)
{
	MAPSComponent::Set(p,enumStruct);
	if (&p == &Property("enumpropName")) {
		UpdateCurrentPropValue();
	}
}

void MAPSEnumProperties::UpdateCurrentPropValue()
{
	MAPSEnumStruct& enumStruct = Property(0).EnumValues();
	m_CurrentPropIndex = enumStruct.selectedEnum;
	m_CurrentPropString = (*enumStruct.enumValues)(m_CurrentPropIndex);
	MAPSStreamedString sx;
	sx << "New value has been selected : " << m_CurrentPropIndex << ", " << m_CurrentPropString;
	ReportInfo(sx);
}

void MAPSEnumProperties::Birth()
{
	//Programmatically set the content and current selection of such enum property
	MAPSEnumStruct newPropValues;
	newPropValues.FromString("4|1|newVal1|newVal2|newVal3|newVal4"); //nb entries = 4|current selection=1 which corresponds to "newVal2"here (zero based)|entry 0|entry 1|...|entry n
	Set(Property(0),newPropValues);
	
	//Or just set the current index
	Set(Property(0),1); //sets the currently selected index (zero-based)
	
	//Or just set the current string among the existing one (should match one of the possibilities.
	Set(Property(0),"newVal2");
	
	//Note that these Set(...) methods can be replaced by DirectSet(...) corresponding ones in case
	//you would like to bypass the Set overrides here-above and directly affect the new value in the 
	//RTMaps symbol table.
	//DirectSet would also skip the call to Dynamic() when called at design time (Dynamic() is never called at runtime anyway...)
}

void MAPSEnumProperties::Core() 
{
	//Querying enum properties:
	
	//Get the selected index.
	int current_index = (int)GetIntegerProperty("enumpropname");
	//Or get the selected string
	MAPSString str = GetStringProperty("enumpropname");
	//Or get the full content and selection information
	MAPSEnumStruct enum_values = GetEnumProperty("enumpropname");

	//Do something now...
	Wait4Event(isDyingEvent);
}

void MAPSEnumProperties::Death()
{
}
