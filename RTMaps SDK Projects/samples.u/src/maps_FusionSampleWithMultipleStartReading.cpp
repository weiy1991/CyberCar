////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/* Chapter 3 - Image processing.
 * This section demonstrates how to perform image processing
 * within RTMaps components: the IplImage structure, how to 
 * get images, how to generate images, etc...
 * Start with:
 *	- ImageHeaderDisplay
 *	- ImageGenerator
 *	- simple_imageprocessing
 *	- MonoVisionIPUSample
 *	- then FusionSamples and StereoVision
 */

/*! \ingroup Chapter3 
 * \file
 * \brief FusionSampleWithMultipleStartReading
 * This sample illustrates the use of the StartReading method (multiple input variant). Here we just
 * display the color value of pixel (x,y) of input image. (x,y) are computer by another 
 * component and read from the iPoint input. See the difference between this component and
 * the FusionSampleWithSamplingReaders sample.
*/

#include "maps_FusionSampleWithMultipleStartReading.h"	// Includes the header of this component

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSFusionSampleWithMultipleStartReading)
    MAPS_INPUT("iImage",MAPS::FilterIplImage,MAPS::FifoReader)
    MAPS_INPUT("iPoint",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSFusionSampleWithMultipleStartReading)
    MAPS_OUTPUT("oColor",MAPS::Integer32,NULL,NULL,4)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSFusionSampleWithMultipleStartReading)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSFusionSampleWithMultipleStartReading)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (FusionSampleWithMultipleStartReading) behaviour
MAPS_COMPONENT_DEFINITION(MAPSFusionSampleWithMultipleStartReading,"FusionSampleWithMultipleStartReading","1.0",128,MAPS::Threaded,MAPS::Threaded,
			  2,
			  1,
			  0,
			  0)

void MAPSFusionSampleWithMultipleStartReading::Birth()
{
	m_lastImageIOElt=NULL;
	m_lastPointIOElt=NULL;
}

void MAPSFusionSampleWithMultipleStartReading::Core() 
{
	MAPSInput *inputs[]={&Input("iImage"),&Input("iPoint")};
	int inputThatAnswered;

	// Get the I/O buffer element containing the new image data
	MAPSIOElt* iIOElt=StartReading(2,inputs,&inputThatAnswered);
	// We don't have to check the returned pointer, because a NULL value will cancel our
	// processing algorithm. The NULL pointer could be returned if an input has been 
	// disconnected
	switch(inputThatAnswered)
	{
	case 0 : 
		m_lastImageIOElt=iIOElt;
		break;
	case 1:
		m_lastPointIOElt=iIOElt;
		break;
	}

	if ((m_lastImageIOElt!=NULL)&&(m_lastPointIOElt!=NULL))
	{
		// Inside this I/O element, we get :
		//		- the IPL image
		IplImage &iIPLImage=m_lastImageIOElt->IplImage();
		// The pixel width is the number of bytes per pixel
		int	iPixelWidth=(iIPLImage.nChannels*iIPLImage.depth+7)/8;

		// Check if the vector has at least 2 coordinates
		if (m_lastPointIOElt->VectorSize()<2)
			return;
		// Get the pixel coordinates
		int &x=m_lastPointIOElt->Integer32(0);
		int &y=m_lastPointIOElt->Integer32(1);
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
		ioEltColor->Timestamp()=m_lastImageIOElt->Timestamp();
		// Send the output data
		StopWriting(ioEltColor);
	}
}

void MAPSFusionSampleWithMultipleStartReading::Death()
{
}
