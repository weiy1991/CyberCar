////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter3 
 * \file
 * \brief Threshold
 * This component is a sample of image processing in RTMaps using the ImageProcessing1Src1Dest
 * model. This simple component just threshold image inputs according to a user-specified
 * property containing the threshold level.
*/

#include "maps_Threshold.h"

// Add here more inputs to your components
MAPS_BEGIN_INPUTS_DEFINITION(MAPSThreshold)
MAPS_END_INPUTS_DEFINITION

// Add here more outputs to your components
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSThreshold)
MAPS_END_OUTPUTS_DEFINITION

// Add here more properties to your components
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSThreshold)
    MAPS_PROPERTY("pThreshold",128,false,true)
MAPS_END_PROPERTIES_DEFINITION

// Add here more actions to your components
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSThreshold)
MAPS_END_ACTIONS_DEFINITION

// Put here the operations your component will implement
MAPS_BEGIN_IMAGEPROCESSING_1SRC1DEST_DEFINITION(MAPSThreshold)
    // To add operations, use the MAPS_IMAGEPROCESSING_1SRC1DEST macro
    MAPS_IMAGEPROCESSING_1SRC1DEST(1,// The id of the operation
				   "THRESHOLD",// The name of the operation
				   0,			// The number of integer parameters your operation needs
				   0,			// The number of double parameters your operation needs
				   NULL,		// Specify the color model of your operation output if it is different than the inputs images ones
				   0)			// Ends the operation definition with the number of click your operation needs
MAPS_END_IMAGEPROCESSING_1SRC1DEST_DEFINITION

// Define your ImageProcessing name, version, behaviour, etc. Define also its inputs, outputs, properties, and actions number.
MAPS_IMAGEPROCESSING_1SRC1DEST_COMPONENT_DEFINITION(MAPSThreshold,"Threshold","1.0",128,MAPS::Sequential|MAPS::Threaded,
						    MAPS::Threaded,0,0,1,0,
						    -1) // Reserved. Always put -1 here

// The Birth() method
void MAPSThreshold::Birth()
{
    // Always call the MAPSImageProcessing1Src1Dest::Birth() method first (the parent of this component)
    MAPSImageProcessing1Src1Dest::Birth();
    // Make your own initialisations here
}

// The Core() method
void MAPSThreshold::Core()
{
    // Always call the MAPSImageProcessing1Src1Dest::Core() method first (the parent of this component)
    MAPSImageProcessing1Src1Dest::Core();
    // Most of the time, you don't have to put code in the Core() method
}

// The Death() method
void MAPSThreshold::Death()
{
    // Make your own closure here
    
    // Always call the MAPSImageProcessing1Src1Dest::Death() method at the end (the parent of this component)
    MAPSImageProcessing1Src1Dest::Death();
}

// The main function for a RTMaps Image Processing operations with 1 input and 1 output. This method
// is called each time a set of 2 input data has been read and stacked by the parent class MAPSImageProcessing1Src1Dest.
// This function is called with the following parameters :
//	- input : the image got from the iImage input
//	- output : the image to be written by your operation, image that will be sent to oImage ouput
//	- desc   : the operation description as given by the MAPS_IMAGEPROCESSING_1SRC1DEST macros above (in the operation 
//				definition location)
//	- params : the parameters (integers, doubles, clicks) of the operation
bool MAPSThreshold::Transform(IplImage *input, IplImage *output, const MAPSImageProcessing1Src1DestDefinition *desc, const MAPSImageProcessing1Src1DestParams *params)
{
    // Get the pointer to output image pixels
    unsigned char *oData=(unsigned char *)output->imageData;
    // Get the pointer to input image pixels
    const unsigned char *iData=(const unsigned char *)input->imageData;
    // Get the threshold value
    unsigned char threshold=(unsigned char)GetIntegerProperty("pThreshold");
    // Use the integer parameter as the reference for the threshold
    for(int i=0;i<output->imageSize;i++)
    {
	if (iData[i]>threshold)
	    oData[i]=255;
	else
	    oData[i]=0;
    }
    return(true);
}
