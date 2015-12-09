////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

// The vectorizer_sample components demonstrate how to (and how NOT to...) 
// write RTMaps components that have multiple inputs.
// These samples show how to get integer data on several inputs, make a vector,
// and output it.

/*! \ingroup Chapter2 
 * \file
 * \brief vectorizer_sample_4_resynchronizer: 
 * Component vectorizer_sample_0 demonstrated the wrong way of writing
 * a component with multiple inputs.
 * Here is one of the right ways.
 * This component generates a new vector when its 2 inputs can get
 * synchronized data, which means data with the same timestamps (+/- a synch. tolerance).
 * This is achieved with a special StartReading function: the SynchroStartReading,
 * which provides 2 MAPSIOElts at the same time.
*/

#include "maps_vectorizer_sample_4_resynchronizer.h"	// Includes the header of this component

//We declare 2 inputs with FifoReader type.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSvectorizer_sample_4_resynchronizer)
	MAPS_INPUT("input1",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("input2",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

//We declare 1 output for vectors of size 2.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSvectorizer_sample_4_resynchronizer)
    MAPS_OUTPUT("vectorOut",MAPS::Integer32,NULL,NULL,2)
MAPS_END_OUTPUTS_DEFINITION

MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSvectorizer_sample_4_resynchronizer)
    MAPS_PROPERTY("synch_tolerance",0,false,false)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSvectorizer_sample_4_resynchronizer)
MAPS_END_ACTIONS_DEFINITION

//Multiple inputs components have to be threaded. Don't allow sequential behaviour.
MAPS_COMPONENT_DEFINITION(MAPSvectorizer_sample_4_resynchronizer,"vectorizer_sample_4_resynchronizer","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  2, // Nb of inputs
			  1, // Nb of outputs
			  1, // Nb of properties
			  0) // Nb of actions

void MAPSvectorizer_sample_4_resynchronizer::Birth()
{
	//Initialize the _inputs member array.
	m_inputs[0] = &Input(0);
	m_inputs[1] = &Input(1);

	m_synch_tolerance = GetIntegerProperty("synch_tolerance");
}

void MAPSvectorizer_sample_4_resynchronizer::Core() 
{
	//Blocks until 2 ioElts are found (1 on each input) with the same
	//timestamp +/- the synchronization tolerance.
	MAPSTimestamp t = SynchroStartReading(2,m_inputs,m_ioElts,m_synch_tolerance);
	if (m_ioElts[0] == NULL || m_ioElts[1] == NULL)
		return;

	//Generate the output vector.
	MAPSIOElt* ioEltOut = StartWriting(Output(0));
	ioEltOut->Integer32(0) = m_ioElts[0]->Integer32();
	ioEltOut->Integer32(1) = m_ioElts[1]->Integer32();
	ioEltOut->VectorSize() = 2;
	ioEltOut->Timestamp() = t; //The chosen timestamp is the oldest of the 2 received MAPSIOElts. (if _synch_tolerance is 0, then the 2 MAPSIOElt timestamps are equal).
	StopWriting(ioEltOut);
}

void MAPSvectorizer_sample_4_resynchronizer::Death()
{
}
