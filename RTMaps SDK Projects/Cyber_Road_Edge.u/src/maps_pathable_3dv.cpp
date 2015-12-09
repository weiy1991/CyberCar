////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_pathable_3dv.h"	// Includes the header of this component
#include "opencv2\opencv.hpp"
#include "opencv2\legacy\legacy.hpp"
using namespace cv;
//#include "lib3dv\device.h"

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSpathable_3dv)
	MAPS_INPUT("DV_Image",MAPS::FilterIplImage,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSpathable_3dv)
    MAPS_OUTPUT("Pathable_Image",MAPS::IplImage,NULL,NULL,0)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSpathable_3dv)
    MAPS_PROPERTY("threshold",128,false,true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSpathable_3dv)
    //MAPS_ACTION("aName",MAPSpathable_3dv::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (pathable_3dv) behaviour
MAPS_COMPONENT_DEFINITION(MAPSpathable_3dv,"pathable_3dv","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  0) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSpathable_3dv::Birth()
{
	m_firstTime = true;
}

/***************************************************************************/
void MAPSpathable_3dv::Core() 
{
	/*--------------------------image load---------------------------*/
	MAPSIOElt* ioEltIn = StartReading(Input(0));
	if(NULL==ioEltIn)
		return;

	IplImage& imageIn = ioEltIn->IplImage();
	if (m_firstTime) {
		m_firstTime = false;
		if (*(MAPSUInt32*)imageIn.channelSeq != MAPS_CHANNELSEQ_GRAY &&
			*(MAPSUInt32*)imageIn.channelSeq != MAPS_CHANNELSEQ_BGR &&
			*(MAPSUInt32*)imageIn.channelSeq != MAPS_CHANNELSEQ_RGB)
		
			Error("Unsupported image format.");

		Output(0).AllocOutputBufferIplImage(imageIn);
	}


	/*--------------------------image out---------------------------*/
	MAPSIOElt* ioEltOut  = StartWriting(Output(0));
	IplImage& imageOut = ioEltOut->IplImage();

	ThresholdImage(imageIn, imageOut,(unsigned char)GetIntegerProperty("threshold"));
	ioEltOut->Timestamp() = ioEltIn->Timestamp();

	StopWriting(ioEltOut);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSpathable_3dv::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}

/****************************************************************************/
/*int HoughLine(Mat mat,int pos,int threshold){
	IplImage ipl_disp = mat;
	IplImage *ipl_dispB = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
	cvThreshold(&ipl_disp,ipl_dispB,pos,255,CV_THRESH_BINARY);//二值化处理
	cvNamedWindow("二值化图", CV_WINDOW_AUTOSIZE);
    cvShowImage("二值化图",ipl_dispB);

	IplImage *ipl_dispCanny = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
	cvCanny(&ipl_disp,ipl_dispCanny,threshold,threshold*3,3);    //边缘检测处理
	cvNamedWindow("边缘检测图", CV_WINDOW_AUTOSIZE);
	cvShowImage("边缘检测图",ipl_dispCanny);

	CvMemStorage *storage = cvCreateMemStorage();
	double fRho = 1;
	double fTheta = CV_PI / 180;
	int nMaxLineNumber = 50;   //最多检测条直线
	double fMinLineLen = 50;   //最小线段长度
	double fMinLineGap = 10;   //最小线段间隔
	CvSeq *pcvSeqLines = cvHoughLines2(ipl_dispB, storage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);
	IplImage *pColorImage = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 3);
	cvCvtColor(ipl_dispB, pColorImage, CV_GRAY2BGR);
	int i;
	int n0,n1;
	for(i = 0; i < pcvSeqLines->total; i++){
		CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, i);
		cvLine(pColorImage, line[0], line[1], CV_RGB(255,0,0), 2);
		
		float slope = abs((line[0].y - line[1].y)/(line[0].x - line[1].x));
		if(slope<3.8){
			n0 = line[0].y;
			n1 = line[1].y;
		}
	}
	cvNamedWindow("houghLine", CV_WINDOW_AUTOSIZE);
	cvShowImage("houghLine", pColorImage);

	return n0,n1;
}*/

void MAPSpathable_3dv::ThresholdImage(const IplImage& imageIn, IplImage& imageOut, unsigned char threshold)
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
