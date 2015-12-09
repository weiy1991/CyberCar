////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

// The vectorizer_sample components demonstrate how to (and how NOT to...) 
// write RTMaps components that have multiple inputs.
// These samples show how to get integer data on several inputs, make a vector,
// and output it.

/*! \ingroup Chapter2 
 * \file
 * \brief vectorizer_sample_5_generic: 
 * Component vectorizer_sample_0 demonstrated the wrong way of writing
 * a component with multiple inputs.
 * Here is presented a way to write a generic vectorizer (as the one provided in
 * the standard components), able to be parameterized at runtime in order to dynamically
 * choose the sampling mode from the ones presented in the previous samples.
 * This component makes use of the Dynamic function overloading which allows
 * the creation/removal of inputs, outputs and properties at runtime.
*/

#include "maps_vectorizer_sample_5_generic.h"	// Includes the header of this component

//We declare 2 input models (but these models will no be instanciated
//automatically (see the MAPS_COMPONENT_DEFINITION). The first one
//will be used to dynamically create inputs of type FifoReader, the second one
//will be used to dynamically create inputs of type SamplingReader.
//The MAPS::FilterNumer allows to connect either Integer type data or Float type data.
MAPS_BEGIN_INPUTS_DEFINITION(MAPSvectorizer_sample_5_generic)
    MAPS_INPUT("input",MAPS::FilterNumber,MAPS::FifoReader)
    MAPS_INPUT("inputS",MAPS::FilterNumber,MAPS::SamplingReader)
MAPS_END_INPUTS_DEFINITION

//We declare 3 outputs, the size of the output vectors is unknown at this point,
//(it depends on the number of inputs that will be chosen by the user).
//so the output buffers will be manually allocated.
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSvectorizer_sample_5_generic)
    MAPS_OUTPUT("outputFloat",MAPS::VectorFlag|MAPS::Float64,NULL,NULL,0)
	MAPS_OUTPUT("outputInteger",MAPS::VectorFlag|MAPS::Integer32,NULL,NULL,0)
    MAPS_OUTPUT("outputInteger64",MAPS::VectorFlag|MAPS::Integer64,NULL,NULL,0)
    MAPS_OUTPUT("outputFloat32",MAPS::VectorFlag|MAPS::Float32,NULL,NULL,0)
MAPS_END_OUTPUTS_DEFINITION

//The 2 first properties declared here allow to specify the number of required inputs
//and the sampling mode.
//The 3 last properties will be dynamically created depending on the chosen sampling mode.
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSvectorizer_sample_5_generic)
    MAPS_PROPERTY("nbInputs",2,false,false)
	MAPS_PROPERTY_ENUM("samplingMode","Synchronized|Periodic resampling|Asynchronous arrival on any of the inputs|Triggered by first input|Wait for at least 1 arrival on each input",1,false,false)
    MAPS_PROPERTY("samplingPeriod",50000, false,false)
	MAPS_PROPERTY("synchroTolerance",0,false,false)
	MAPS_PROPERTY("ignoreTriggerInputContent",false,false,false)
MAPS_END_PROPERTIES_DEFINITION

MAPS_BEGIN_ACTIONS_DEFINITION(MAPSvectorizer_sample_5_generic)
MAPS_END_ACTIONS_DEFINITION

MAPS_COMPONENT_DEFINITION(MAPSvectorizer_sample_5_generic,"vectorizer_sample_5_generic","1.0",128,MAPS::Threaded|MAPS::Sequential,MAPS::Threaded,
						  0, //2 input models have been declared by none is automatically instanciated.
						  4, 
						  2, //the first 2 properties are created automaticaly. The others ones will be created dynamically.
						  0)

//This function will be called by the RTMaps engine: 
//	- when the component is created (placed on a diagram).
//	- when an input is connected.
//	- when a property value changes.
//	- just before running the diagram.
//This function allows to dynamicall create inputs, outputs and properties.
//NEVER call NewInput, NewOutput or NewProperty methods outside of this Dynamic function.
void MAPSvectorizer_sample_5_generic::Dynamic()
{
	m_nbInputs=(int)GetIntegerProperty("nbInputs");
	m_samplingMode = (int)GetIntegerProperty("samplingMode");

	//Clear the _inputs and _gotData member arrays.
	m_inputs.SetSize(0);
	m_gotData.SetSize(0);
	for (int i=0;i<m_nbInputs;i++) {
		MAPSInput *input;
		if (m_samplingMode == 1) //Periodic resampling -> all the inputs are samplingreader.
			input=&NewInput(1,MAPSString("input0",i+1));
		else if (m_samplingMode == 3 && i>0) { //Triggered by 1rst input : 1rst input is FifoReader, all the others are samplingreaders
			input=&NewInput(1,MAPSString("input0",i+1));
		} else { //Else, all inputs have FifoReader type.
			input=&NewInput(0,MAPSString("input0",i+1));
		}
		m_inputs(i)=input; //Fill in the _inputs member array.
		m_gotData(i)=false; //Fill in the _gotData member array.
	}

	if (m_samplingMode == 0) //Synchronized mode,
		NewProperty(3);		//create the "synchroTolerance" property.
	if (m_samplingMode == 1) //Periodic resampling mode,
		NewProperty(2);		//create the "samplingPeriod" property.
	if (m_samplingMode == 3) //Triggered by first input,
		NewProperty(4);		//create the "ignoreTriggerInputContent" property.
}

void MAPSvectorizer_sample_5_generic::Birth()
{
	//Manual allocation of the output buffers.
    Output(0).AllocOutputBuffer(m_nbInputs);
    Output(1).AllocOutputBuffer(m_nbInputs);
	Output(2).AllocOutputBuffer(m_nbInputs);
	Output(3).AllocOutputBuffer(m_nbInputs);

	if (m_samplingMode == 0) //Synchronized mode.
		m_synchroTolerance = GetIntegerProperty("synchroTolerance");
	if (m_samplingMode == 1) //Periodic resampling mode.
	{
		m_samplingPeriod = GetIntegerProperty("samplingPeriod");
		m_appointment = MAPS::CurrentTime();
	}
	if (m_samplingMode == 3) //Triggered by first input.
		m_ignoreTriggerInputContent = GetBoolProperty("ignoreTriggerInputContent");
	else
		m_ignoreTriggerInputContent = false;


    m_dataReady=false;
    if (m_samplingMode == 0) {
		m_synchroIOElts.SetSize(m_nbInputs);
    } else {
		m_data.SetSize(m_nbInputs);
    }
	for (int i=0; i<m_nbInputs;i++) {
		m_gotData[i] = false;
	}

}

void MAPSvectorizer_sample_5_generic::Death()
{
}

void MAPSvectorizer_sample_5_generic::Core() 
{
	bool doOutput = false;;
	MAPSTimestamp outputTimestamp = -1;
    //*********************************************************************************
	switch (m_samplingMode) {
		//**************************************************************************************
		case 0 : //Synchro
		{
			MAPSIOElt *IOEltW0, *IOEltW1, *IOEltW2, *IOEltW3;
			IOEltW0=StartWriting(Output(0));
			IOEltW1=StartWriting(Output(1));
			IOEltW2=StartWriting(Output(2));
			IOEltW3=StartWriting(Output(3));
			MAPSTimestamp t=SynchroStartReading(m_nbInputs,m_inputs,m_synchroIOElts,m_synchroTolerance);
			for (int i=0;i<m_nbInputs;i++) {
				//Check the data type (Integer, Integer64, Float or Float32): this is necessary
				//since the input filters accepts any type of number.
				if (MAPS::TypeFilter(m_synchroIOElts[i]->Type(),MAPS::FilterInteger32)) {
					IOEltW0->Float64(i)=m_synchroIOElts[i]->Integer32();
					IOEltW1->Integer32(i)=m_synchroIOElts[i]->Integer32();
					IOEltW2->Integer64(i)=m_synchroIOElts[i]->Integer32();
					IOEltW3->Float32(i)=(MAPSFloat32)m_synchroIOElts[i]->Integer32();
				} else if (MAPS::TypeFilter(m_synchroIOElts[i]->Type(),MAPS::FilterInteger64)) {
					IOEltW0->Float64(i)=(MAPSFloat64)m_synchroIOElts[i]->Integer64();
					IOEltW1->Integer32(i)=(MAPSInt32)m_synchroIOElts[i]->Integer64();
					IOEltW2->Integer64(i)=m_synchroIOElts[i]->Integer64();
					IOEltW3->Float32(i)=(MAPSFloat32)m_synchroIOElts[i]->Integer64();
				} else if (MAPS::TypeFilter(m_synchroIOElts[i]->Type(),MAPS::FilterFloat64)) {
					IOEltW0->Float64(i)=m_synchroIOElts[i]->Float64();
					IOEltW1->Integer32(i)=(MAPSInt32)m_synchroIOElts[i]->Float64();
					IOEltW2->Integer64(i)=(MAPSInt64)(m_synchroIOElts[i]->Float64()+0.5);
					IOEltW3->Float32(i)=(MAPSFloat32)m_synchroIOElts[i]->Float64();
				} else if (MAPS::TypeFilter(m_synchroIOElts[i]->Type(),MAPS::FilterFloat32)) {
					IOEltW0->Float64(i)=m_synchroIOElts[i]->Float32();
					IOEltW1->Integer32(i)=(MAPSInt32)m_synchroIOElts[i]->Float32();
					IOEltW2->Integer64(i)=(MAPSInt64)(m_synchroIOElts[i]->Float32()+0.5);
					IOEltW3->Float32(i)=m_synchroIOElts[i]->Float32();
				}
			}
			IOEltW0->Timestamp()=t;
			StopWriting(IOEltW0);
			IOEltW1->Timestamp()=t;
			StopWriting(IOEltW1);
			IOEltW2->Timestamp()=t;
			StopWriting(IOEltW2);
			IOEltW3->Timestamp()=t;
			StopWriting(IOEltW3);

			return; //No need to use the standard output way after switch block.
		}
		//**************************************************************************************
		case 1 : //Sampling
		{
			MAPSIOElt *IOElt;
			Wait(m_appointment);
			m_appointment+=m_samplingPeriod;

			//When at least one piece of data has arrived on each input
			//there is no more need to check for data availability anymore.
			if (false == m_dataReady) {
				int i;
				for (i=0; i<m_nbInputs; i++) {
					if (DataAvailableInFIFO(Input(i)))
						m_gotData[i] = true;
				}
				// Check that all data are available.
				m_dataReady=true;
				for (i=0;i<NbInputs();i++) {
					if (!m_gotData[i]) {
						m_dataReady=false;
						break;
					}
				}
				if (!m_dataReady)
					return;
			}

			for (int i=0; i<m_nbInputs;i++) {
				IOElt = StartReading(Input(i));
				if (IOElt==NULL) {
					return;
				} else {
					if (MAPS::TypeFilter(IOElt->Type(),MAPS::FilterInteger32)) {
						m_data[i]=IOElt->Integer32();
					} else if (MAPS::TypeFilter(IOElt->Type(),MAPS::FilterInteger64)) {
						m_data[i]= (MAPSFloat64)IOElt->Integer64();
					} else if (MAPS::TypeFilter(IOElt->Type(),MAPS::FilterFloat64)) {
						m_data[i]=IOElt->Float64();
					} else if (MAPS::TypeFilter(IOElt->Type(),MAPS::FilterFloat32)) {
						m_data[i]=IOElt->Float32();
					}
				}
			}

			doOutput = true;
			outputTimestamp = m_appointment;
		}
		break;
		//**************************************************************************************
		case 2 : //Asynchronous
		{
			int inputThatAnswered;
			MAPSIOElt *IOElt=StartReading(m_nbInputs,m_inputs,&inputThatAnswered);
			if (IOElt==NULL) {
				// There was a detachment
				m_gotData(inputThatAnswered)=false;
				m_dataReady=false;
			} else {
				if (MAPS::TypeFilter(Input(inputThatAnswered).Type(),MAPS::FilterInteger32)) {
					m_data[inputThatAnswered]=IOElt->Integer32();
				} else if (MAPS::TypeFilter(Input(inputThatAnswered).Type(),MAPS::FilterInteger64)) {
					m_data[inputThatAnswered]=(MAPSFloat64)IOElt->Integer64();
				} else if (MAPS::TypeFilter(Input(inputThatAnswered).Type(),MAPS::FilterFloat64)) {
					m_data[inputThatAnswered]=IOElt->Float64();
				} else if (MAPS::TypeFilter(Input(inputThatAnswered).Type(),MAPS::FilterFloat32)) {
					m_data[inputThatAnswered]=IOElt->Float32();
				}
				m_gotData[inputThatAnswered]=true;
				// Check that all data have arrived
				if (!m_dataReady) {
					m_dataReady=true;
					for (int i=0;i<m_nbInputs;i++) {
						if (!m_gotData[i]) {
							m_dataReady=false;
							break;
						}
					}
				}

				if (m_dataReady) {
					doOutput = true;
					outputTimestamp = IOElt->Timestamp();
				}
			}
		}
		break;
		//**************************************************************************************
		case 3: //Triggered
		{
			//First input is FifoReader, so this StartReading is blocking until
			//it receives a new piece of data.
			MAPSIOElt* ioEltIn = StartReading(Input(0));
			if (ioEltIn == NULL) {
				m_dataReady = false;
				m_gotData[0] = false;
				return;
			}
			if (false == m_ignoreTriggerInputContent) {
				if (MAPS::TypeFilter(ioEltIn->Type(),MAPS::FilterInteger32)) {
					m_data[0] = (MAPSFloat64)ioEltIn->Integer32();
				} else if (MAPS::TypeFilter(ioEltIn->Type(),MAPS::FilterFloat64)) {
					m_data[0] = ioEltIn->Float64();
				} else if (MAPS::TypeFilter(ioEltIn->Type(),MAPS::FilterInteger64)) {
					m_data[0] = (MAPSFloat64)ioEltIn->Integer64();
				} else if (MAPS::TypeFilter(ioEltIn->Type(),MAPS::FilterFloat32)) {
					m_data[0] = (MAPSFloat64)ioEltIn->Float32();
				}
				m_gotData[0] = true;
			}
			
			if (false == m_dataReady) {
				int i;
				for (i=1; i< m_nbInputs; i++) {
					if (DataAvailableInFIFO(Input(i)))
						m_gotData[i] = true;
				}
				// Check that all data have arrived
				m_dataReady=true;
				int startIndex = m_ignoreTriggerInputContent ? 1 : 0;
				for (i=startIndex;i<m_nbInputs;i++) {
					if (!m_gotData[i]) {
						m_dataReady=false;
						break;
					}
				}
				if (false == m_dataReady)
					return;
			}

			for (int i=1; i<m_nbInputs; i++) {
				MAPSIOElt* ioElt = StartReading(Input(i));
				if (ioElt==NULL)
					continue;
				if (MAPS::TypeFilter(ioElt->Type(),MAPS::FilterInteger32)) {
					m_data[i] = (MAPSFloat64)ioElt->Integer32();
				} else if (MAPS::TypeFilter(ioElt->Type(),MAPS::FilterFloat64)) {
					m_data[i] = ioElt->Float64();
				} else if (MAPS::TypeFilter(ioElt->Type(),MAPS::FilterInteger64)) {
					m_data[i] = (MAPSFloat64)ioElt->Integer64();
				} else if (MAPS::TypeFilter(ioElt->Type(),MAPS::FilterFloat32)) {
					m_data[i] = (MAPSFloat64)ioElt->Float32();
				}
			}

			outputTimestamp = ioEltIn->Timestamp();
			MAPSIOElt *IOEltW0, *IOEltW1, *IOEltW2, *IOEltW3;
			IOEltW0=StartWriting(Output(0));
			IOEltW1=StartWriting(Output(1));
			IOEltW2=StartWriting(Output(2));
			IOEltW3=StartWriting(Output(3));
			if (m_ignoreTriggerInputContent) {
				for (int i=1;i<m_nbInputs;i++) {
					IOEltW0->Float64(i-1)=m_data[i];
					IOEltW1->Integer32(i-1)=(MAPSInt32)(m_data[i]+0.5);
					IOEltW2->Integer64(i-1)=(MAPSInt64)(m_data[i]+0.5);
					IOEltW3->Float32(i-1)=(MAPSFloat32)m_data[i];
				}
				IOEltW0->VectorSize() = m_nbInputs-1;
				IOEltW1->VectorSize() = m_nbInputs-1;
				IOEltW2->VectorSize() = m_nbInputs-1;
				IOEltW3->VectorSize() = m_nbInputs-1;
			} else {
				for (int i=0;i<m_nbInputs;i++) {
					IOEltW0->Float64(i)=m_data[i];
					IOEltW1->Integer32(i)=(MAPSInt32)(m_data[i]+0.5);
					IOEltW2->Integer64(i)=(MAPSInt64)(m_data[i]+0.5);
					IOEltW3->Float32(i)=(MAPSFloat32)m_data[i];
				}
			}
			IOEltW0->Timestamp()=outputTimestamp;
			IOEltW1->Timestamp()=outputTimestamp;
			IOEltW2->Timestamp()=outputTimestamp;
			IOEltW3->Timestamp()=outputTimestamp;
			StopWriting(IOEltW0);
			StopWriting(IOEltW1);
			StopWriting(IOEltW2);
			StopWriting(IOEltW3);
		}
		return; //The output is done just above.
		//**************************************************************************************
		case 4 : //Wait for at least 1 arrival on every input
		{
			MAPSIOElt *IOElt;
			int inputThatAnswered;
			IOElt=StartReading(m_nbInputs,m_inputs,&inputThatAnswered);
			if (IOElt==NULL) {
				// There was a detachment
				m_gotData[inputThatAnswered]=false;
				m_dataReady=false;
				return;
			}

			if (MAPS::TypeFilter(IOElt->Type(),MAPS::FilterInteger32)) {
				m_data[inputThatAnswered]=(MAPSFloat64)IOElt->Integer32();
			} else if (MAPS::TypeFilter(IOElt->Type(),MAPS::FilterInteger64)) {
				m_data[inputThatAnswered]=(MAPSFloat64)IOElt->Integer64();
			} else if (MAPS::TypeFilter(IOElt->Type(),MAPS::FilterFloat64)) {
				m_data[inputThatAnswered]=IOElt->Float64();
			} else if (MAPS::TypeFilter(IOElt->Type(),MAPS::FilterFloat32)) {
				m_data[inputThatAnswered]=IOElt->Float32();
			}
			m_gotData[inputThatAnswered]=true;
			// Check that all data have arrived
			if (!m_dataReady) {
				m_dataReady=true;
				for (int i=0;i<m_nbInputs;i++) {
					if (!m_gotData[i]) {
						m_dataReady=false;
						break;
					}
				}
			}

			if (m_dataReady) { //All data has changed : output.
				//Output.
				doOutput = true;
				outputTimestamp = IOElt->Timestamp();

				m_dataReady = false;
				for (int i=0;i<m_nbInputs;i++) {
					m_gotData[i] = false;
				}
			}


		}
		break;
		//**************************************************************************************
		default :
			Error("Unknown samling mode.");
			break;
	}

	if (doOutput) {
		MAPSIOElt *IOEltW0, *IOEltW1, *IOEltW2, *IOEltW3;
		IOEltW0=StartWriting(Output(0));
		IOEltW1=StartWriting(Output(1));
		IOEltW2=StartWriting(Output(2));
		IOEltW3=StartWriting(Output(3));
		for (int i=0;i<m_nbInputs;i++) {
			IOEltW0->Float64(i)=m_data[i];
			IOEltW1->Integer32(i)=(MAPSInt32)(m_data[i]+0.5);
			IOEltW2->Integer64(i)=(MAPSInt64)(m_data[i]+0.5);
			IOEltW3->Float32(i)=(MAPSFloat32)m_data[i];
		}
		IOEltW0->Timestamp()=outputTimestamp;
		IOEltW1->Timestamp()=outputTimestamp;
		IOEltW2->Timestamp()=outputTimestamp;
		IOEltW3->Timestamp()=outputTimestamp;
		StopWriting(IOEltW0);
		StopWriting(IOEltW1);
		StopWriting(IOEltW2);
		StopWriting(IOEltW3);
	}
}
