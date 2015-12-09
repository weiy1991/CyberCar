////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter1 
 * \file
 * \brief multiplier_2: This module extends the multiplier1 component also provided in this section.
 * It reads integer vectors data on its input, multiplies the incoming integers
 *  by a factor specified in a property, and outputs the result as a vector of integers.
 * This component demonstrates how to :
 * - declare an output able to handle vectors of limited size (see the MAPS_OUTPUT macro)
 * - use of the MAPSIOElt::VectorSize() method
 * This component will be set to handle vectors of integers with a limited 
 * maximum size (set to 32 in this example).
 * Component multiplier3 also provided in this section
 * will demonstrate how to extend this component to handle vectors of data
 * of any size to be more generic.
 */

#include "maps_multiplier_2.h"	// Includes the header of this component

// Declaration of an input called "input", which accepts connections with
// outputs of type Integer, and which has a FifoReader behaviour by default.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSmultiplier2)
    MAPS_INPUT("input",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Declaration of an output called "output" of type Integer, which will
// handle vectors of size 32 max.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSmultiplier2)
    MAPS_OUTPUT("output",MAPS::Integer32,NULL,NULL,32)
MAPS_END_OUTPUTS_DEFINITION

// Declaration of a property called "factor" of type integer (due to the type
// of the default value), with default value = 2, and which is allowed to be 
// modified by the user during the diagram execution.
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSmultiplier2)
    MAPS_PROPERTY("factor",2,false,true)
MAPS_END_PROPERTIES_DEFINITION

// No action here yet.
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSmultiplier2)
    //MAPS_ACTION("aName",MAPSmultiplier2::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Declaration of the component attributes: named "multiplier2", with
// version 1.0, default priority of 128 (medium) if the component is threaded,
// the component accepts threaded or sequential behaviours, is threaded by default
// and has 1 input, 1 output and 1 property (taken from the above declarations).
MAPS_COMPONENT_DEFINITION(MAPSmultiplier2,"multiplier2","1.0",128,
			MAPS::Threaded|MAPS::Sequential,MAPS::Threaded,
			  1, // Nb of inputs
			  1, // Nb of outputs
			  1, // Nb of properties
			  0) // Nb of actions

void MAPSmultiplier2::Birth()
{
}

void MAPSmultiplier2::Core() 
{
	//Request data on the input called "input".
	//This function will block until a new data is available on the input.
	//The MAPSIOElt class is the generic object that contains a piece of data (numbers, packets of bytes, images, etc...)
	MAPSIOElt* ioEltIn = StartReading(Input("input"));
	if (ioEltIn == NULL)
		return;

	//First get a MAPSIOElt object in which we can write the result to be output.
	//The StartWriting function is non-blocking.
	MAPSIOElt* ioEltOut = StartWriting(Output("output"));

	//Bound the vector size to 32 if the input vector is larger that 32 elements.
	int vectorsizeout = MIN(ioEltIn->VectorSize(),32);

	//Get the current value of the "factor" property (whose type is integer).
	int factor = (int)GetIntegerProperty("factor");

	//For each element in the input MAPSIOElt (up to 32 if input vector is larger), compute the result
	//and write it in the output MAPSIOElt.
	for (int i=0; i<vectorsizeout; i++) {
		ioEltOut->Integer32(i) = factor * ioEltIn->Integer32(i);
	}

	//Specify the number of valid elements in the output vector (less or equal than 32).
	ioEltOut->VectorSize() = vectorsizeout;

	//Transfer the timestamp (don't forget).
	ioEltOut->Timestamp() = ioEltIn->Timestamp();

	//Writing in the ioEltOut object is complete.
	//The only thing left is to release it and make it available to downstream components :
	StopWriting(ioEltOut);
}

//This function is called once by the RTMaps engine when the diagram execution stops.
//Nothing to do here for this simple component.
void MAPSmultiplier2::Death()
{
}
