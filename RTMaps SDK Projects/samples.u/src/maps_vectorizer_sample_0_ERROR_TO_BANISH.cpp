////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/* Chapter 2 - Mutliple inputs (IMPORTANT !).
* The vectorizer_sample components demonstrate how to (and how NOT to...) 
* write RTMaps components that have multiple inputs.
* These samples show how to get integer data on several inputs, make a vector,
* and output it.
* This chapter is VERY IMPORTANT for the RTMaps programmer in order to understand
* correctly the different behaviours of the RTMaps inputs (FifoReader,
* SamplingReader, etc...) and the correct ways to use them.
*/


/*! \ingroup Chapter2 
 * \file
 * \brief vectorizer_sample_0_ERROR_TO_BANISH: 
 * Programming correctly components with multiple inputs in RTMaps is easy,
 * providing the user avoids one mistake: calling successive StartReading
 * functions on FifoReader, LastOrNextReader, Wait4NextReader or NeverskippingReader inputs.
 * StartReading is blocking when called on those types of inputs, so, successive StartReading
 * functions will synchronize the component on the input that receives data at lowest frequency,
 * thus generating FIFO overflows and latency on the other inputs.
 * This component demonstrate what is STRICTLY FORBIDDEN.
 * The next samples (vectorize_sample_1 to 5) will demonstrates the correct ways
 * depending on when you want the output vectors to be generated.
 * In order to test this wrong behaviour, you can connect 2 Random Integers components
 * with different data generation periods, then watch what happens.
*/

#include "maps_vectorizer_sample_0_ERROR_TO_BANISH.h"

//We declare 2 inputs with FifoReader type...
MAPS_BEGIN_INPUTS_DEFINITION(MAPSvectorizer_sample_0_ERROR_TO_BANISH)
    MAPS_INPUT("input1",MAPS::FilterInteger32,MAPS::FifoReader)
    MAPS_INPUT("input2",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// We declare 1 output for vectors of size 2.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSvectorizer_sample_0_ERROR_TO_BANISH)
    MAPS_OUTPUT("vectorOut",MAPS::Integer32,NULL,NULL,2)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSvectorizer_sample_0_ERROR_TO_BANISH)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSvectorizer_sample_0_ERROR_TO_BANISH)
MAPS_END_ACTIONS_DEFINITION

//Multiple inputs components have to be threaded. Don't allow sequential behaviour.
MAPS_COMPONENT_DEFINITION(MAPSvectorizer_sample_0_ERROR_TO_BANISH,"vectorizer_sample_0_ERROR_TO_BANISH","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  2, // Nb of inputs
			  1, // Nb of outputs
			  0, // Nb of properties
			  0) // Nb of actions

void MAPSvectorizer_sample_0_ERROR_TO_BANISH::Birth()
{
}

void MAPSvectorizer_sample_0_ERROR_TO_BANISH::Core() 
{

	//This call is blocking until the first input
	//receives a piece of data.
	MAPSIOElt* ioElt1 = StartReading(Input(0));
	if (NULL == ioElt1)
		return;

	//This call will be blocking until the second input
	//receives a piece of data.
	MAPSIOElt* ioElt2 = StartReading(Input(1));
	if (NULL == ioElt2)
		return;

	//Imagine that the first input is connected to an output
	//that generates integers at 100 Hz, and the second
	//input is connected to an output that generates integers
	//at 10 Hz only...
	//While we are waiting on the second call to StartReading (on second input),
	//10 pieces of data have arrived on the first input -> this induces latency
	//and FifoOverflow !!!!


	ReportWarning("AAAAAARGL: 2 successive StartReading on FifoReader inputs !");


	MAPSIOElt* ioEltOut = StartWriting(Output(0));

	ioEltOut->Integer32(0) = ioElt1->Integer32(0);
	ioEltOut->Integer32(1) = ioElt2->Integer32(0);
	//That is a good indication that we made something wrong: how to choose the timestamp ???
	//Why choose the timestamp from ioElt1 instead of ioElt2 ? ... no reason.
	ioEltOut->Timestamp() = ioElt2->Timestamp(); 
	ioEltOut->VectorSize() = 2;

	StopWriting(ioEltOut);
}

void MAPSvectorizer_sample_0_ERROR_TO_BANISH::Death()
{
}
