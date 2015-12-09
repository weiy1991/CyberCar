////////////////////////////////
// RTMaps SDK Component
////////////////////////////////
/*
Yuan Wei
CyberC3_Yuan
2015.3
*/
////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Area_Able.h"	// Includes the header of this component
#include "math.h"
#include <iostream>

#include <stdio.h>
#include "opencv2\opencv.hpp"
#include "opencv2\legacy\legacy.hpp"
using namespace cv;
using namespace std;
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSArea_Able)
	MAPS_INPUT("D_Map",MAPS::FilterIplImage,MAPS::FifoReader)
	MAPS_INPUT("R_Image",MAPS::FilterIplImage,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSArea_Able)
    MAPS_OUTPUT("V_Disparity",MAPS::IplImage,NULL,NULL,1)
	MAPS_OUTPUT("Areaable_Image",MAPS::IplImage,NULL,NULL,1)
	MAPS_OUTPUT("Out_info",MAPS::Float,NULL,NULL,1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSArea_Able)

	MAPS_PROPERTY("sleepTime",500000,false,true)
	MAPS_PROPERTY("imageIn_period_us",20000,false,true)

MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSArea_Able)
    //MAPS_ACTION("aName",MAPSArea_Able::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Area_Able) behaviour
MAPS_COMPONENT_DEFINITION(MAPSArea_Able,"Area_Able","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  2, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  3, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  2, // Nb of properties. Leave -1 to use the number of declared property definitions
			  0) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSArea_Able::Birth()
{
	m_nextTimestamp = MAPS::CurrentTime();
	sleepTime = GetIntegerProperty("sleepTime");
	m_firstTime = true;

	//init time estimation
	/*	startTime_V = 0.0;
		endTime_V = 0.0;*/
}


/***************************************************************************/
void MAPSArea_Able::Core() 
{
    /*****************************image in*****************************/
	
	MAPSIOElt* ioEltIn_DMap = StartReading(Input("D_Map"));//读入视差图数据
	if(NULL==ioEltIn_DMap)
		return;
	IplImage& imageIn_DMap = ioEltIn_DMap->IplImage();
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
		
	//ioEltIn_RImage = StartReading(Input("R_Image"));//读入右侧摄像头原始数据
	//if(NULL==ioEltIn_RImage)
	//	return;
	//IplImage& imageIn_RImage = ioEltIn_RImage->IplImage();
		
	//m_nextTimestamp = MAPS::CurrentTime() + GetIntegerProperty("imageIn_period_us");

	/*****************************V-disparity*****************************/
	Mat M_imageIn = &imageIn_DMap;
	int row_imageIn = M_imageIn.rows;
	int col_imageIn = M_imageIn.cols;
	CvMat* v_dis = cvCreateMat(row_imageIn,255,CV_8UC1);

	//time estimation
	//ftime(&startTime_V);

	//V视差算法
	for(int row = 0;row<row_imageIn;row++)
	{
		unsigned char* q = (unsigned char*)(v_dis->data.ptr+row*v_dis->step);//像素点的行指针
		for(int i = 0;i<255;i++)
		{
			int j = 0;
			for(int col = 0;col<col_imageIn;col++)
			{
				if(M_imageIn.at<unsigned char>(row,col)<10 || M_imageIn.at<unsigned char>(row,col)>245)//获取像素并判断像素值的大小
				{
					j = j;
				}
				else if(M_imageIn.at<unsigned char>(row,col)==i)
				{
					j++;
				}
			}
			*q = j;
			q++;
		}
	}

	//cvReleaseMat(&v_dis);

	//ftime(&endTime_V);
	//time_V = (endTime_V.time-startTime_V.time)*1000+(endTime_V.millitm-startTime_V.millitm);
	
	Mat M_vdis(v_dis,true);

	IplImage image_vdis = M_vdis;

	/*****************************image out*****************************/
	//输出算法的估计时间
	//MAPSIOElt* Out_info = StartWriting(Output("Out_info"));
	//Out_info->Float(0) =  time_V;
	//StopWriting(Out_info);

	//输出V-视差图
	MAPSIOElt* ioEltOut  = StartWriting(Output("V_Disparity"));
	IplImage& imageOut = ioEltOut->IplImage();
	imageOut = M_vdis;
	ioEltOut->Timestamp() = ioEltIn_DMap->Timestamp();
	StopWriting(ioEltOut);

	//休眠设置
	Rest(sleepTime);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSArea_Able::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}

int MAPSArea_Able::HoughLine(Mat mat,int pos,int threshold)
{
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

}

/*cvReleaseImage(imageIn_DMap);
	cvReleaseImage(imageIn_RImage);
	cvReleaseImage(imageOut);*/
	
	/***********************ROI of right image*************************/
	//Mat M_rimage(&imageIn_RImage,Rect(0,0,480,400));  //ROI of right image
	//IplImage ipl_Rimg = M_rimage;
	//IplImage* Rimg_red;
	//Rimg_red =  cvCreateImage(cvSize(ipl_Rimg.width,ipl_Rimg.height), IPL_DEPTH_8U, 3);
	//cvCvtColor(&ipl_Rimg, Rimg_red, CV_GRAY2BGR);
	//Mat Rimg_redm = Rimg_red;

	/***********************Hough transformation************************/
	/*IplImage ipl_disp = M_vdis;
	IplImage *ipl_dispB = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
	int pos = 12;
	cvThreshold(&ipl_disp,ipl_dispB,pos,255,CV_THRESH_BINARY);//二值化处理
	
	IplImage *ipl_dispCanny = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
	int threshold = 60;
	cvCanny(&ipl_disp,ipl_dispCanny,threshold,threshold*3,3);    //边缘检测处理


	CvMemStorage *storage = cvCreateMemStorage();
	double fRho = 1;
	double fTheta = CV_PI / 180;
	int nMaxLineNumber = 50;   //最多检测条直线
	double fMinLineLen = 50;   //最小线段长度
	double fMinLineGap = 10;   //最小线段间隔
	CvSeq *pcvSeqLines = cvHoughLines2(ipl_dispB, storage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);
	IplImage *pColorImage = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 3);
	cvCvtColor(ipl_dispB, pColorImage, CV_GRAY2BGR);
	int i,j;
	int n0 = 0,n1 = 0;
	int m0 = 0,m1 = 0;
	int m =0;
	int q,p;
	float b;
	Mat dispnew = Mat::zeros(row_imageIn,col_imageIn,CV_8UC1);
	
	for(i = 0; i < pcvSeqLines->total; i++){
		CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, i);
		cvLine(pColorImage, line[0], line[1], CV_RGB(255,0,0), 2);
	
		
		if(line[0].x != line[1].x){
		      float slope = abs((line[0].y - line[1].y)/(line[0].x - line[1].x));
			 /* b = (line[1].x - line[0].x)/(line[1].y - line[0].y);*/
             /* b = line[0].y - slope * line[0].x;
		      if(slope<3.8){
					  for(int q = line[0].y;q<line[1].y;q++){
						  /*j = line[0].x;
						  j += b;*/
						/*  j = (q-b)/slope;
						  for(int p=0;p<col_imageIn;p++){
							  if(M_imageIn.at<unsigned char>(q,p)<= (j+2) && M_imageIn.at<unsigned char>(q,p)>=(j-30)) {
								  dispnew.at<unsigned char>(q,p)=255;
								  Rimg_redm.at<unsigned char>(q,p*Rimg_redm.channels())=0;
								  Rimg_redm.at<unsigned char>(q,p*Rimg_redm.channels()+1)=0;
								  Rimg_redm.at<unsigned char>(q,p*Rimg_redm.channels()+2)=255;
							  }
							  else dispnew.at<unsigned char>(q,p)=0;
						  }
					  }
		      }
		}
	}

	IplImage image_detect = Rimg_redm;*/