////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter3 
 * \file
 * \brief FusionSampleWithSamplingReaders
 * This sample illustrates the use of sampling reader in a fusion algorithm. Here we just
 * display the color value of pixel (x,y) of input image. (x,y) are computer by another 
 * component and read from the iPoint input. See the difference between this component and
 * the FusionSampleWithMultipleStartReading sample.
 * The main input is set to a FIFOReader (or LastOrNextReader), other inputs are SamplingReader
 * A StartReading call on a SamplingReader never blocks, except if no data have ever been
 * received on this input.
*/

#include "maps_FusionSampleWithSamplingReaders.h"	// Includes the header of this component

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSFusionSampleWithSamplingReaders)
    MAPS_INPUT("iImage",MAPS::FilterIplImage,MAPS::FifoReader)
    MAPS_INPUT("iPoint",MAPS::FilterInteger32,MAPS::SamplingReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSFusionSampleWithSamplingReaders)
    MAPS_OUTPUT("oColor",MAPS::Integer32,NULL,NULL,4)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSFusionSampleWithSamplingReaders)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSFusionSampleWithSamplingReaders)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (FusionSampleWithSamplingReaders) behaviour
MAPS_COMPONENT_DEFINITION(MAPSFusionSampleWithSamplingReaders,"FusionSampleWithSamplingReaders","1.0",128,MAPS::Threaded,MAPS::Threaded,
			  2,
			  1,
			  0,
			  0)

void MAPSFusionSampleWithSamplingReaders::Birth()
{
}

void MAPSFusionSampleWithSamplingReaders::Core() 
{
	// Get the I/O buffer element containing the new image data
	// This StartReading blocks until a new image is received (FifoReader).
	MAPSIOElt* iIOEltImage=StartReading(Input("iImage"));
	// Check the returned pointer
	if (iIOEltImage==NULL)
		return;

	// Inside this I/O element, we get :
	//		- the IPL image
	IplImage &iIPLImage=iIOEltImage->IplImage();
	// The pixel width is the number of bytes per pixel
	int	iPixelWidth=(iIPLImage.nChannels*iIPLImage.depth+7)/8;

	// Get the I/O buffer element containing the last point data
	// A StartReading call on a SamplingReader never blocks, except if no data at all have ever been
	// received on this input.
	if (false == DataAvailableInFIFO(Input("iPoint")))
		return;
	MAPSIOElt* iIOEltPoint=StartReading(Input("iPoint"));
	// Check the returned pointer
	if (iIOEltPoint==NULL)
		return;
	// Check if the vector has at least 2 coordinates
	if (iIOEltPoint->VectorSize()<2)
		return;
	// Get the pixel coordinates
	int &x=iIOEltPoint->Integer32(0);
	int &y=iIOEltPoint->Integer32(1);
	// The following variable point to the first byte of the pixel (x,y) of input image
	unsigned char	*ib=(unsigned char *)(iIPLImage.imageData+x*iPixelWidth+y*iIPLImage.widthStep);
	// The we can write the result of our fusion algorithm to oColor output...
	MAPSIOElt *ioEltColor=StartWriting(Output("oColor"));
	// Check the returned pointer
	if (ioEltColor==NULL)
		return;
	// Write the color to the output vector
	int i;
	for(i=0;i<MIN(iPixelWidth,ioEltColor->BufferSize());i++)
		ioEltColor->Integer32(i)=ib[i];
	// Set the output vector size to the number of color component (not greater than the vector max size)
	ioEltColor->VectorSize()=i;
	// Set the output timestamp
	ioEltColor->Timestamp()=iIOEltImage->Timestamp();
	// Send the output data
	StopWriting(ioEltColor);
}

void MAPSFusionSampleWithSamplingReaders::Death()
{
}
