////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

// The vectorizer_sample components demonstrate how to (and how NOT to...) 
// write RTMaps components that have multiple inputs.
// These samples show how to get integer data on several inputs, make a vector,
// and output it.

/*! \ingroup Chapter2 
 * \file
 * \brief vectorizer_sample_3_resampling: 
 * Component vectorizer_sample_0 demonstrated the wrong way of writing
 * a component with multiple inputs.
 * Here is one of the right ways.
 * This component generates a new vector at a constant frequency
 * independently of the frequency of the data received on the inputs.
 * This is achieved with 2 inputs of type SamplingReader.
*/

#include "maps_vectorizer_sample_3_resampling.h"

//We declare 2 inputs with SamplingReader type.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSvectorizer_sample_3_resampling)
	MAPS_INPUT("input1",MAPS::FilterInteger32,MAPS::SamplingReader)
	MAPS_INPUT("input2",MAPS::FilterInteger32,MAPS::SamplingReader)
MAPS_END_INPUTS_DEFINITION

//We declare 1 output for vectors of size 2.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSvectorizer_sample_3_resampling)
    MAPS_OUTPUT("vectorOut",MAPS::Integer32,NULL,NULL,2)
MAPS_END_OUTPUTS_DEFINITION

//We declare 1 property which will allow to set the sampling frequency.
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSvectorizer_sample_3_resampling)
    MAPS_PROPERTY("sampling_period",100000,false,true)
MAPS_END_PROPERTIES_DEFINITION


MAPS_BEGIN_ACTIONS_DEFINITION(MAPSvectorizer_sample_3_resampling)
MAPS_END_ACTIONS_DEFINITION

//Multiple inputs components have to be threaded. Don't allow sequential behaviour.
MAPS_COMPONENT_DEFINITION(MAPSvectorizer_sample_3_resampling,"vectorizer_sample_3_resampling","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  2, // Nb of inputs
			  1, // Nb of outputs
			  1, // Nb of properties
			  0) // Nb of actions

void MAPSvectorizer_sample_3_resampling::Birth()
{
	//Initialize the default values and the start time.
	m_val1 = 0;
	m_val2 = 0;
	m_appointment = MAPS::CurrentTime();
}

void MAPSvectorizer_sample_3_resampling::Core() 
{
	//Wait for the next sampling date.
	m_appointment += GetIntegerProperty("sampling_period");
	Wait(m_appointment);

	//Get the most recent piece of data (if available) on inputs 1 and 2.
	//StartReading on SamplingReader inputs is not blocking unless no data
	//at all has been sent to the input (so we check this with the DataAvailableInFIFO function.
	if (DataAvailableInFIFO(Input(0))) {
		MAPSIOElt* ioEltIn1 = StartReading(Input(0));
		if (ioEltIn1 == NULL)
			return;
		m_val1 = ioEltIn1->Integer32();
	}
	if (DataAvailableInFIFO(Input(1))) {
		MAPSIOElt* ioEltIn2 = StartReading(Input(1));
		if (ioEltIn2 == NULL)
			return;
		m_val2 = ioEltIn2->Integer32();
	}

	//Generate the output vector.
	MAPSIOElt* ioEltOut = StartWriting(Output(0));
	ioEltOut->Integer32(0) = m_val1;
	ioEltOut->Integer32(1) = m_val2;
	ioEltOut->VectorSize() = 2;
	ioEltOut->Timestamp() = m_appointment; //The chosen timestamp is here the sampling time.
	StopWriting(ioEltOut);
}

void MAPSvectorizer_sample_3_resampling::Death()
{
}
