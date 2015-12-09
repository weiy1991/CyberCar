////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter3 
 * \file
 * \brief simple_imageprocessing
 * Apply a simple threshold to the bytes in the pixels buffer of the
 * received images.
 */

#include "maps_simple_imageprocessing.h"	// Includes the header of this component

// Declaration of an input accepting uncompressed images (in the IplImage format).
MAPS_BEGIN_INPUTS_DEFINITION(MAPSsimple_imageprocessing)
    MAPS_INPUT("input",MAPS::FilterIplImage,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Declaration of an output of IplImage type (uncompressed images).
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSsimple_imageprocessing)
    MAPS_OUTPUT("output",MAPS::IplImage,NULL,NULL,0)
MAPS_END_OUTPUTS_DEFINITION

// Declaration of an integer property with default value set to 128,
// and able to be modified during the diagram execution.
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSsimple_imageprocessing)
    MAPS_PROPERTY("threshold",128,false,true)
MAPS_END_PROPERTIES_DEFINITION

// No actions in this component
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSsimple_imageprocessing)
MAPS_END_ACTIONS_DEFINITION

// Definition of the comopnent (name, version, default priority, accepted behaviours, etc...
MAPS_COMPONENT_DEFINITION(MAPSsimple_imageprocessing,"simple_imageprocessing","1.0",128,
						  MAPS::Threaded|MAPS::Sequential,MAPS::Threaded,
			  1, // Nb of inputs
			  1, // Nb of outputs
			  1, // Nb of properties
			  0) // Nb of actions


void MAPSsimple_imageprocessing::Birth()
{
	m_firstTime = true;
}

void MAPSsimple_imageprocessing::Core() 
{
	//Request data on the input called "input".
	//This function will block until a new data is available on the input.
	//The MAPSIOElt class is the generic object that contains a piece of data (numbers, packets of bytes, images, etc...)
	MAPSIOElt* ioEltIn = StartReading(Input(0));
	if (NULL==ioEltIn)
		return;

	//Get the uncompressed image contained in the received MAPSIOElt.
	IplImage& imageIn = ioEltIn->IplImage();

	// The first time we get an image data, we have to allocate the output buffers
	// Indeed, the size of the output buffer depends on the size of input image
	// (we want here the output image to have the same dimensions and color model as the input image)
	if (m_firstTime) {
		m_firstTime = false;

		//Example of how to test the incoming images color format,
		//and abort if the input images color format is not supported.
		if (*(MAPSUInt32*)imageIn.channelSeq != MAPS_CHANNELSEQ_GRAY &&
			*(MAPSUInt32*)imageIn.channelSeq != MAPS_CHANNELSEQ_BGR &&
			*(MAPSUInt32*)imageIn.channelSeq != MAPS_CHANNELSEQ_RGB)
			//The Error method will throw an exception, which will be caught by the RTMaps
			//engine outside of the Core() function, so that execution of this component
			//will stop and the Death function will be called immediately.
			//The error message is displayed in red in the console as it is with the ReportError method.
			//(All the other components in the diagram keep-on working).
			Error("Unsupported image format.");

		Output(0).AllocOutputBufferIplImage(imageIn);
	}

	//Output buffers are allocated at this point. We can request a MAPSIOElt object for the
	//output buffers to write in.
	MAPSIOElt* ioEltOut = StartWriting(Output(0));

	//Retrieve the IplImage object contained in the output MAPSIOElt.
	IplImage& imageOut = ioEltOut->IplImage();

	//Apply the image processing function.
	ThresholdImage(imageIn, imageOut,(unsigned char)GetIntegerProperty("threshold"));

	//Transfer the timestamp (don't forget).
	ioEltOut->Timestamp() = ioEltIn->Timestamp();

	// OK, send the output buffer (and don't write into it any more !)
	StopWriting(ioEltOut);
}

//The image processing function.
void MAPSsimple_imageprocessing::ThresholdImage(const IplImage& imageIn, IplImage& imageOut, unsigned char threshold)
{
	//These pointers will be placed at the beginning of the source and destination images lines.
	unsigned char* lineSrcStart = (unsigned char*)imageIn.imageData;
	unsigned char* lineDstStart = (unsigned char*)imageOut.imageData;
	//These pointers will be used to go through the bytes within eahc line of the source and destination images.
	unsigned char* pixSrc;
	unsigned char* pixDst;

	//For each line
	for (int i=imageIn.height; i>0 ; i--) {
		//Rest the bytes pointers at the beginning of the line.
		pixSrc = lineSrcStart;
		pixDst = lineDstStart;
		//For each byte in the line, apply the threshold.
		for (int j=imageIn.widthStep; j>0 ;  j--) {
			if (*(pixSrc++) > threshold)
				*(pixDst++) = 255;
			else
				*(pixDst++) = 0;
		}
		//Step to next line
		lineSrcStart += imageIn.widthStep;
		lineDstStart += imageIn.widthStep;
	}
}

void MAPSsimple_imageprocessing::Death()
{
}
