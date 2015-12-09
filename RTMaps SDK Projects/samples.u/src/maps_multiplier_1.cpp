////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/* Chapter 1 - Numeric data handling.
 * This chapter contains some simple samples that show how to program
 * components that deal with numeric data (how to get data on an input,
 * how to access properties, how to provide data on an output.
 */

/*! \ingroup Chapter1 
 * \file
 * \brief multiplier_1: This module is one of the simplest RTMaps modules for data processing.
 * It reads integer scalar data on its input, multiplies the incoming integer
 * by a factor specified in a property, and outputs the result as a scalar integer.
 * This component demonstrates how to :
 * - define an input
 * - define an output
 * - define a property
 * - get scalar data from its input
 * - output scalar data
 * Components multiplier2 and multiplier3 also provided in this section
 * will demonstrate how to extend this component to handle vectors of data
 * in order to be more generic.
 */

#include "maps_multiplier_1.h"	// Includes the header of this component

// Declaration of an input called "input", which accepts connections with
// outputs of type Integer, and which has a FifoReader behaviour by default.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSmultiplier1)
    MAPS_INPUT("input",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Declaration of an output called "output" of type Integer, which will
// handle vectors of size 1 max (which means... scalars).
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSmultiplier1)
    MAPS_OUTPUT("output",MAPS::Integer32,NULL,NULL,1)
MAPS_END_OUTPUTS_DEFINITION

// Declaration of a property called "factor" of type integer (due to the type
// of the default value), with default value = 2, and which is allowed to be 
// modified by the user during the diagram execution.
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSmultiplier1)
    MAPS_PROPERTY("factor",2,false,true)
MAPS_END_PROPERTIES_DEFINITION

// No action here yet.
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSmultiplier1)
    //MAPS_ACTION("aName",MAPSmultiplier1::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Declaration of the component attributes: named "multiplier1", with
// version 1.0, default priority of 128 (medium) if the component is threaded,
// the component accepts threaded or sequential behaviours, is threaded by default
// and has 1 input, 1 output and 1 property (taken from the above declarations).
MAPS_COMPONENT_DEFINITION(MAPSmultiplier1,"multiplier1","1.0",128,
			MAPS::Threaded|MAPS::Sequential,MAPS::Threaded,
			  1, // Nb of inputs
			  1, // Nb of outputs
			  1, // Nb of properties
			  0) // Nb of actions

//This function is called once by the RTMaps engine when the diagram execution starts.
//Nothing to do here for this simple component.
void MAPSmultiplier1::Birth()
{
}

//This function is called in a loop during diagram execution.
//Always check you use a blocking function somewhere (here StartReading on a FifoReader input)
//otherwise this component would run into an infinite loop, hence consuming 100% CPU.
void MAPSmultiplier1::Core() 
{
	//Request data on the input called "input".
	//This function will block until a new data is available on the input.
	//The MAPSIOElt class is the generic object that contains a piece of data (numbers, packets of bytes, images, etc...)
	MAPSIOElt* ioEltIn = StartReading(Input("input"));
	if (ioEltIn == NULL)
		return;

	//Get the integer contained in the MAPSIOElt object that has just been received.
	int valueIn = ioEltIn->Integer32();
	//Get the current value of the "factor" property (whose type is integer).
	int factor = (int)GetIntegerProperty("factor");
	//Compute the result.
	int result = valueIn *factor;

	//Output the result.
	//First get a MAPSIOElt object in which we can write the result to be output.
	//The StartWriting function is non-blocking.
	MAPSIOElt* ioEltOut = StartWriting(Output("output"));

	//Write the result in the MAPSIOElt object (MAPSIOElt::Integer32() returns a C++ reference to the contained integer).
	ioEltOut->Integer32() = result;

	//Transfer the timestamp (don't forget).
	ioEltOut->Timestamp() = ioEltIn->Timestamp();

	//Writing in the ioEltOut object is complete.
	//The only thing left is to release it and make it available to downstream components :
	StopWriting(ioEltOut);
}

//This function is called once by the RTMaps engine when the diagram execution stops.
//Nothing to do here for this simple component.
void MAPSmultiplier1::Death()
{
}
