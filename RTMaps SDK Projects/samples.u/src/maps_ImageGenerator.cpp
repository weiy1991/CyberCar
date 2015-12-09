////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter3 
 * \file
 * \brief ImageGenerator
 * This simple component generates some random images according to user-specified properties
*/

#include "maps_ImageGenerator.h"	// Includes the header of this component
#include <cstdlib>


// Use the macros to declare the inputs (here, no inputs are present)
MAPS_BEGIN_INPUTS_DEFINITION(MAPSImageGenerator)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSImageGenerator)
    MAPS_OUTPUT("output",MAPS::IplImage,NULL,NULL,0)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSImageGenerator)
    MAPS_PROPERTY("pWidth",384,false,false)
    MAPS_PROPERTY("pHeight",288,false,false)
    MAPS_PROPERTY("pColorModel","GRAY",false,false)
    MAPS_PROPERTY("pPeriod",100000,false,true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions (here, no actions are present)
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSImageGenerator)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (ImageGenerator) behaviour
MAPS_COMPONENT_DEFINITION(MAPSImageGenerator,"ImageGenerator","1.0",128,MAPS::Threaded,MAPS::Threaded,
			  0, // Number of inputs
			  1, // Number of outputs
			  4, // Number of properties
			  0) // Number of actions

void MAPSImageGenerator::Birth()
{
	IplImage model;
	// Get a model of image according to properties specifications
	model=MAPS::IplImageModel((int)GetIntegerProperty("pWidth"),(int)GetIntegerProperty("pHeight"),GetStringProperty("pColorModel"));
	// Allocate output image according to the model
	Output("output").AllocOutputBufferIplImage(model);
	// Get the start time
	m_nextTimestamp=MAPS::CurrentTime();
	// Initialize the random number generation
	std::srand((int)m_nextTimestamp);
}

void MAPSImageGenerator::Core() 
{
	// Wait for the user-specified period
	Wait(m_nextTimestamp);	
	// Want to write a new image data
	MAPSIOElt* ioElt=StartWriting(Output("output"));	

	// Get the image structure encapsulated into the MAPSIOElt
	IplImage &iplimage=ioElt->IplImage();
	// The image structure contains a pointer to the image bytes (pixels) : imageData
	unsigned char *imageData=(unsigned char *)iplimage.imageData;
	// Randomly fill the image bytes : use the size of image bytes contained in imageSize member
	for(int i=0;i<iplimage.imageSize;i++)
		imageData[i]=(unsigned char)(std::rand()%256);
	// Use the timestamp field of MAPSIOElt to store the real time of the new data
	ioElt->Timestamp()=m_nextTimestamp;
	// The timeOfIssue field is not changed and will be set in the StopWriting to MAPS::CurrentTime()
	// Send the output data
	StopWriting(ioElt);
	// Compute the next time to wait for
	m_nextTimestamp+=GetIntegerProperty("pPeriod");
}

void MAPSImageGenerator::Death()
{
}
