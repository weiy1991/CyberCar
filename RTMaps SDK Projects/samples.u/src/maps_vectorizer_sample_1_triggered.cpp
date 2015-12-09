////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

// The vectorizer_sample components demonstrate how to (and how NOT to...) 
// write RTMaps components that have multiple inputs.
// These samples show how to get integer data on several inputs, make a vector,
// and output it.

/*! \ingroup Chapter2 
 * \file
 * \brief vectorizer_sample_1_triggered: 
 * Component vectorizer_sample_0 demonstrated the wrong way of writing
 * a component with multiple inputs.
 * Here is one of the right ways.
 * This component generates a new vector, each time a new piece of data
 * arrives on the first input. ("triggered by first input")
 * The way to achieve this is to declare the first input as FifoReader,
 * the second input as SamplingReader and call 2 successive StartReading functions.
*/

#include "maps_vectorizer_sample_1_triggered.h"

//We declare one input with FiFoReader type, then one input with SamplingReader type.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSvectorizer_sample_1_triggered)
    MAPS_INPUT("input1_trigger",MAPS::FilterInteger32,MAPS::FifoReader)
    MAPS_INPUT("input2_resampling",MAPS::FilterInteger32,MAPS::SamplingReader)
MAPS_END_INPUTS_DEFINITION

// We declare 1 output for vectors of size 2.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSvectorizer_sample_1_triggered)
    MAPS_OUTPUT("vectorOut",MAPS::Integer32,NULL,NULL,2)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSvectorizer_sample_1_triggered)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSvectorizer_sample_1_triggered)
MAPS_END_ACTIONS_DEFINITION

//Multiple inputs components have to be threaded. Don't allow sequential behaviour.
MAPS_COMPONENT_DEFINITION(MAPSvectorizer_sample_1_triggered,"vectorizer_sample_1_triggered","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  2, // Nb of inputs
			  1, // Nb of outputs
			  0, // Nb of properties
			  0) // Nb of actions

void MAPSvectorizer_sample_1_triggered::Birth()
{
}

void MAPSvectorizer_sample_1_triggered::Core() 
{
	//This StartReading is blocking (first input is of FifoReader type)
	//until a new piece of data arrives on this input.
	MAPSIOElt* ioEltIn1 = StartReading(Input(0));
	if (NULL == ioEltIn1)
		return;
	
	//This StartReading is not blocking and immediately returns the
	//current available value on the second input.
	//Indeed the second input is of SamplinReader type.
	//Well... in case no data at all has been received on the second
	//input, the StartReading blocks until the first data is received
	//however we can check this with the DataAvailableInFIFO function.
	if (false == DataAvailableInFIFO(Input(1))) {
		ReportWarning("No data at all has been received yet on the second input... we cannot make a vector yet. Ignoring.");
		return;
	}
	MAPSIOElt* ioEltIn2 = StartReading(Input(1));
	if (NULL == ioEltIn2)
		return;

	//Generate the output vector.
	MAPSIOElt* ioEltOut = StartWriting(Output(0));
	ioEltOut->Integer32() = ioEltIn1->Integer32();
	ioEltOut->Integer32(1) = ioEltIn2->Integer32();
	ioEltOut->VectorSize() = 2;
	ioEltOut->Timestamp() = ioEltIn1->Timestamp(); // We choose to transfer the timestamp of ioEltIn1 because it has just arrived, and it is the trigger.

	StopWriting(ioEltOut);

}

void MAPSvectorizer_sample_1_triggered::Death()
{
}
