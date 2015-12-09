////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter5
 * \file
 * \brief DynamicInputs
 * Creating a component with dynamic creation of inputs (same method for
 * components with dynamic outputs, properties or actions).
*/

#include "maps_DynamicInputs.h"


//This input is declared here as a model. (the number of inputs of
//this component is declared as 0 in the MAPS_COMPONENT_DEFINITION macro).
//Dynamic inputs will then be created on this model.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSDynamicInputs)
	MAPS_INPUT("input",MAPS::FilterNumbers,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSDynamicInputs)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSDynamicInputs)
	MAPS_PROPERTY("nbInputs",1,false,false)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSDynamicInputs)
MAPS_END_ACTIONS_DEFINITION

//0 inputs are declared in this definition macro.
MAPS_COMPONENT_DEFINITION(MAPSDynamicInputs,"DynamicInputs","1.0",128,MAPS::Threaded,MAPS::Threaded,
						  0, //Nb inputs = 0 !!!
						  0,
						  1,
						  0)


//Dynamic : this function is called at component creation
//and then each time a property is changed when not in run mode.
void MAPSDynamicInputs::Dynamic()
{
	m_nbInputs=(int)GetIntegerProperty("nbInputs");
	for(int i=0;i<m_nbInputs;i++)
	{
		MAPSString nameInput("input");
		MAPSString number;
		number.Format("0",i);
		nameInput+=number;
		//Creates a new input with the given name.
		//When called within Dynamic(), the created input is compared
		//to the existing ones. If this input did not exist, it is created.
		//If it already exists, the existing input remains unmodified (connections are
		//preserved. At the end, the remaining inputs that were not "recreated", are deleted.
		NewInput(0,(const char *)nameInput);

		//The same kind of operation can be performed with outputs, properties and actions
		//via the NewOutput, NewProperty and NewAction methods.
	}
}


void MAPSDynamicInputs::Birth()
{
    //Nothing here
}

void MAPSDynamicInputs::Core() 
{
    //Nothing there
    Rest(500000);	
}

void MAPSDynamicInputs::Death()
{
}
