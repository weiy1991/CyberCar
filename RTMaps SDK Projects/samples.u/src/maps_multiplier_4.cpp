////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter1 
 * \file
 * \brief: multiplier_4: This module has the same behaviour as multiplier3, but introduces some slight optimizations in the code
 * that can be useful sometimes for high frequency components.
 * This component demonstrates how to :
 * - access inputs and outputs by their index and not their names
 * - maintain a member variable which remains synchronized with the value of a given property.
 */

#include "maps_multiplier_4.h"	// Includes the header of this component

// Declaration of an input called "input", which accepts connections with
// outputs of type Integer, and which has a FifoReader behaviour by default.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSmultiplier4)
    MAPS_INPUT("input",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Declaration of an output called "output" of type Integer, which will
// handle vectors of any size. The last argument set to 0 specified to the RTMaps
// engine not to allocate the output buffers: we will perform this allocation manually
// when we receive the first input data, and before calling the first StartWriting.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSmultiplier4)
    MAPS_OUTPUT("output",MAPS::Integer32,NULL,NULL,0)
MAPS_END_OUTPUTS_DEFINITION

// Declaration of a property called "factor" of type integer (due to the type
// of the default value), with default value = 2, and which is allowed to be 
// modified by the user during the diagram execution.
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSmultiplier4)
    MAPS_PROPERTY("factor",2,false,true)
MAPS_END_PROPERTIES_DEFINITION

// No action here yet.
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSmultiplier4)
    //MAPS_ACTION("aName",MAPSmultiplier4::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Declaration of the component attributes: named "multiplier3", with
// version 1.0, default priority of 128 (medium) if the component is threaded,
// the component accepts threaded or sequential behaviours, is threaded by default
// and has 1 input, 1 output and 1 property (taken from the above declarations).
MAPS_COMPONENT_DEFINITION(MAPSmultiplier4,"multiplier4","1.0",128,
			MAPS::Threaded|MAPS::Sequential,MAPS::Threaded,
			  1, // Nb of inputs
			  1, // Nb of outputs
			  1, // Nb of properties
			  0) // Nb of actions

void MAPSmultiplier4::Birth()
{
	//Alloctation of the _firstTime member variable (see the .h file)
	m_firstTime = true;

	//Initialize the _factor member variable when starting the diagram execution.
	m_factor = (int)GetIntegerProperty("factor");
}

void MAPSmultiplier4::Set(MAPSProperty &p, MAPSInt64 value) 
{
	if (&p == &Property("factor")) {
		m_factor = (int)value;
	}
	MAPSComponent::Set(p,value);
}

void MAPSmultiplier4::Core() 
{
	//Here we retrieve the input by its index and not by its name. It saves some instructions (hashing the characters string,
	//acessing a hashtable, etc...) but the code is more difficult to maintain (if you insert new inputs, 
	//you'll have to update all the indexes.
	MAPSIOElt* ioEltIn = StartReading(Input(0));
	if (ioEltIn == NULL)
		return;

	//Same as multiplier3 (manual allocation of the output buffers).
	if (m_firstTime) {
		m_firstTime = false;
		Output(0).AllocOutputBuffer(ioEltIn->BufferSize());
	}

	//Access to the output via its index (same as above with the inputs).
	MAPSIOElt* ioEltOut = StartWriting(Output(0));

	//No need to call this function anymore : we now use the _factor member variable
	//which update at Birth and any time the property value changes (see the Set method overload).
	//int factor = (int)GetIntegerProperty("factor");

	//For each integer in the input vector, compute the result and write it in
	//the output vector.
	int vectorsize = ioEltIn->VectorSize();
	//Paranoid (and somehow dangerous...) optims here.
	MAPSInt32* srcPtr = (MAPSInt32*) ioEltIn->Data();
	MAPSInt32* dstPtr = (MAPSInt32*) ioEltOut->Data();
	for (int i=0; i<vectorsize; i++) {
		*(dstPtr++) = *(srcPtr++) * m_factor;
	}

	//Specify the number of valid elements in the output vector (less or equal than 32).
	ioEltOut->VectorSize() = vectorsize;

	//Transfer the timestamp (don't forget).
	ioEltOut->Timestamp() = ioEltIn->Timestamp();

	//Writing in the ioEltOut object is complete.
	//The only thing left is to release it and make it available to downstream components :
	StopWriting(ioEltOut);
}

void MAPSmultiplier4::Death()
{
}
