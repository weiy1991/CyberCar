////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter1 
 * \file
 * \brief multiplier_3: This module extends the multiplier2 component also provided in this section.
 * It will be able to handle vectors of number of any size.
 * This component demonstrates how to :
 * - declare an output able without the automatic output buffers allocation (see the MAPS_OUTPUT macro)
 * - "manual" allocation of the output buffers when receiving an input for the first time (see MAPSOutput::AllocOutputBuffer).
 * - use of the MAPSIOElt::VectorSize() method
 * - use of the MAPSIOEtl::BufferSize() method
 * This component is finally much more generic than components mutliplier1 and multiplier2
 */

#include "maps_multiplier_3.h"	// Includes the header of this component

// Declaration of an input called "input", which accepts connections with
// outputs of type Integer, and which has a FifoReader behaviour by default.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSmultiplier3)
    MAPS_INPUT("input",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Declaration of an output called "output" of type Integer, which will
// handle vectors of any size. The last argument set to 0 specified to the RTMaps
// engine not to allocate the output buffers: we will perform this allocation manually
// when we receive the first input data, and before calling the first StartWriting.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSmultiplier3)
    MAPS_OUTPUT("output",MAPS::Integer32,NULL,NULL,0)
MAPS_END_OUTPUTS_DEFINITION

// Declaration of a property called "factor" of type integer (due to the type
// of the default value), with default value = 2, and which is allowed to be 
// modified by the user during the diagram execution.
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSmultiplier3)
    MAPS_PROPERTY("factor",2,false,true)
MAPS_END_PROPERTIES_DEFINITION

// No action here yet.
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSmultiplier3)
    //MAPS_ACTION("aName",MAPSmultiplier3::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Declaration of the component attributes: named "multiplier3", with
// version 1.0, default priority of 128 (medium) if the component is threaded,
// the component accepts threaded or sequential behaviours, is threaded by default
// and has 1 input, 1 output and 1 property (taken from the above declarations).
MAPS_COMPONENT_DEFINITION(MAPSmultiplier3,"multiplier3","1.0",128,
			MAPS::Threaded|MAPS::Sequential,MAPS::Threaded,
			  1, // Nb of inputs
			  1, // Nb of outputs
			  1, // Nb of properties
			  0) // Nb of actions

void MAPSmultiplier3::Birth()
{
	//Alloctation of the _firstTime member variable (see the .h file)
	m_firstTime = true;
}

void MAPSmultiplier3::Core() 
{
	//Request data on the input called "input".
	//This function will block until a new data is available on the input.
	//The MAPSIOElt class is the generic object that contains a piece of data (numbers, packets of bytes, images, etc...)
	MAPSIOElt* ioEltIn = StartReading(Input("input"));
	if (ioEltIn == NULL)
		return;

	//If this element is the first one received in the session, we need
	//to manually allocate the output buffers.
	if (m_firstTime) {
		m_firstTime = false; //(we won't get into this block again during the session).

		//Allocate the output buffers so that they can contain vectors up to the
		//maximum size handled by the output we are connected to.
		//MAPSIOElt::BufferSize() specifies the allocated size in the MAPSIOElt
		//so the maximum length of vectors that can be handled.
		//(for a given MAPSIOElt object, VectorSize() is always lower or equal than BufferSize()).
		Output(0).AllocOutputBuffer(ioEltIn->BufferSize());
	}

	//Now the output buffers have been allocated, we can request an element to write in.
	MAPSIOElt* ioEltOut = StartWriting(Output("output"));

	//Get the current value of the "factor" property (whose type is integer).
	int factor = (int)GetIntegerProperty("factor");

	//For each integer in the input vector, compute the result and write it in
	//the output vector.
	int vectorsize = ioEltIn->VectorSize();
	for (int i=0; i<vectorsize; i++) {
		ioEltOut->Integer32(i) = ioEltIn->Integer32(i)*factor;
	}

	//Specify the number of valid elements in the output vector (less or equal than 32).
	ioEltOut->VectorSize() = vectorsize;

	//Transfer the timestamp (don't forget).
	ioEltOut->Timestamp() = ioEltIn->Timestamp();

	//Writing in the ioEltOut object is complete.
	//The only thing left is to release it and make it available to downstream components :
	StopWriting(ioEltOut);
}

void MAPSmultiplier3::Death()
{
}
