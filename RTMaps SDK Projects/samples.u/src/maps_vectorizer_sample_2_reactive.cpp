////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

// The vectorizer_sample components demonstrate how to (and how NOT to...) 
// write RTMaps components that have multiple inputs.
// These samples show how to get integer data on several inputs, make a vector,
// and output it.

/*! \ingroup Chapter2 
 * \file
 * \brief vectorizer_sample_2_reactive: 
 * Component vectorizer_sample_0 demonstrated the wrong way of writing
 * a component with multiple inputs.
 * Here is one of the right ways.
 * This component generates a new vector, each time a new piece of data
 * arrives on either the first or the second input. ("reactive on the 2 inputs").
 * The way to achieve this is to declare 2 inputs as FifoReader, but use a single
 * StartReading function for the 2 inputs.
*/

#include "maps_vectorizer_sample_2_reactive.h"

//We declare 2 inputs with FifoReader type.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSvectorizer_sample_2_reactive)
    MAPS_INPUT("input1",MAPS::FilterInteger32,MAPS::FifoReader)
    MAPS_INPUT("input2",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// We declare 1 output for vectors of size 2.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSvectorizer_sample_2_reactive)
    MAPS_OUTPUT("vectorOut",MAPS::Integer32,NULL,NULL,2)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSvectorizer_sample_2_reactive)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSvectorizer_sample_2_reactive)
MAPS_END_ACTIONS_DEFINITION

//Multiple inputs components have to be threaded. Don't allow sequential behaviour.
MAPS_COMPONENT_DEFINITION(MAPSvectorizer_sample_2_reactive,"vectorizer_sample_2_reactive","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  2, // Nb of inputs
			  1, // Nb of outputs
			  0, // Nb of properties
			  0) // Nb of actions

void MAPSvectorizer_sample_2_reactive::Birth()
{
	//Initialize the _inputs member array.
	m_inputs[0] = &Input(0);
	m_inputs[1] = &Input(1);

	//Initialize the default values.
	m_val1 = 0;
	m_val2 = 0;
}

void MAPSvectorizer_sample_2_reactive::Core() 
{
	int inputThatAnswered;

	//This StartReading function blocks until a new piece
	//of data arrives on one of the inputs indicated in the _inputs
	//array. It also indicates which of the inputs has received the
	//data in inputThatAnswered.
	MAPSIOElt* ioEltIn = StartReading(2,m_inputs, &inputThatAnswered);
	if (ioEltIn == NULL)
		return;

	MAPSTimestamp t;
	switch (inputThatAnswered) {
		case 0: //We've received new data on the first input.
			m_val1 = ioEltIn->Integer32();
			t = ioEltIn->Timestamp();
			break;
		case 1: //We've received new data on the second input.
			m_val2 = ioEltIn->Integer32();
			t = ioEltIn->Timestamp();
			break;
		default:
			Error("Unknown input.");
	}

	MAPSIOElt* ioEltOut = StartWriting(Output(0));
	ioEltOut->Integer32(0) = m_val1;
	ioEltOut->Integer32(1) = m_val2;
	ioEltOut->VectorSize() = 2;
	ioEltOut->Timestamp() = t; //We transfer the timestamp of the data that just arrived.
	StopWriting(ioEltOut);
}

void MAPSvectorizer_sample_2_reactive::Death()
{
}
