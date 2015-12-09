////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter5
 * \file
 * \brief SetProperty_Overload
 * This component demonstrates how to overload the Set method
 * in order to execute some code each time a given property value
 * changes.
*/

#include "maps_SetProperty_Overload.h"	// Includes the header of this component

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSSetProperty_Overload)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSSetProperty_Overload)
    MAPS_OUTPUT("outputPropValue",MAPS::Integer32,NULL,NULL,1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSSetProperty_Overload)
    MAPS_PROPERTY("value",10,false,true)
    MAPS_PROPERTY("otherValue",20,false,true)
    MAPS_PROPERTY("string","hello world",false,true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSSetProperty_Overload)
    //MAPS_ACTION("aName",MAPSSetProperty_Overload::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (SetProperty_Overload) behaviour
MAPS_COMPONENT_DEFINITION(MAPSSetProperty_Overload,"SetProperty_Overload","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  0, // Nb of inputs
			  1, // Nb of outputs
			  3, // Nb of properties
			  0) // Nb of actions


//This method will be called by the RTMaps engine each time an Integer type property
//value is modified.
void MAPSSetProperty_Overload::Set(MAPSProperty &p, MAPSInt64 newVal)
{
	ReportInfo("An integer property has just changed.");
	//If the property that is being modified is the "value" property...
	if (&p == &Property("value")) {
		ReportInfo("It is the value property.");
		//Here, I can make some tests on the validity of user input, in order to bound the
		//new value to an acceptable one.
		if (newVal < 0) {
			ReportInfo("Incorrect value for this property. It has to be positive. Value is set to 0.");
			newVal = 0;
		}
		m_Value = newVal;   //I store the new value in my member variable.
		//This will avoid necessary calls to GetIntegerProperty
		//within the Core() method each time we want to use it for example.
	}
	//Since the Set method is declared as virtual in MAPSComponent,
	//we have to explicitly call MAPSComponent::Set so that the
	//RTMaps engine can process the property change.
	MAPSComponent::Set(p,newVal);
}

//Same operation: this method will be called by the RTMaps engine
//when a property of type String is modified.
void MAPSSetProperty_Overload::Set(MAPSProperty &p, const MAPSString& newString)
{
	ReportInfo("A string property has just changed.");
	ReportInfo(MAPSString("The new string is ") + newString);
	//Since the Set method is declared as virtual in MAPSComponent,
	//we have to explicitly call MAPSComponent::Set so that the
	//RTMaps engine can process the property change.
	MAPSComponent::Set(p,newString);
}

void MAPSSetProperty_Overload::Birth()
{
	//If the property has not been changed since the component
	//was instantiated, I have to initialize my member variable.
	m_Value = GetIntegerProperty("value");
}

void MAPSSetProperty_Overload::Core() 
{
	//Output the current value of property "value".
	MAPSIOElt* ioElt = StartWriting(Output(0));
	ioElt->Integer32() = (int)m_Value;
	StopWriting(ioElt);

	//Pause a little while.
	Rest(500000);
}

void MAPSSetProperty_Overload::Death()
{
}
