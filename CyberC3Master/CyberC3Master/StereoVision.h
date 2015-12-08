/*
CopyRight @ Yuan Wei

Shanghai Jiao Tong University 

2015.10

CyberC3

Class Description:

This class can easily let you solve the problem of stereovision project.
The main function is to show the raw images , such as disparity image,
left image , right image, the color image of disparity image, and then 
caculate the V-disparity and U-disparity, and then transform the frame 
from camera to the world.
*/
#pragma once
#include<iostream>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

class StereoVision
{
private:

	IplImage* DImage;	//disparity image
	IplImage* CDImage;  //color image of disparity image
	IplImage* LImage;	//left image of stereovision 
	IplImage* RImage;	//right image of stereovision
	IplImage* VImage;	//V disparity image 
	IplImage* UImage;	//U disparity image

	//CvMat* DImageCvMat;
	//CvMat* RImageCvMat;
	//CvMat* CImageCvMat;
	CvMat* VImageCvMat;
	CvMat* UImageCvMat;
	CvMat* ResultCvMat;
	CvMat* GridCvMat;

public:

	//Init Functions
	StereoVision();
	StereoVision(IplImage* _DImage);
	StereoVision(IplImage* _DImage, IplImage* _CDImage);
	StereoVision(IplImage* _DImage, IplImage* _CDImage, IplImage* _LRImage);
	StereoVision(IplImage* _DImage, IplImage* _CDImage, IplImage* _LImage, IplImage* _RImage);
	~StereoVision();

	//Solution Functions
	CvMat* VDisparityCalculate(IplImage* _DImage, int VMinValue, int VMaxValue);
	CvMat* UDisparityCalculate(IplImage* _DImage, int UMinValue, int UMaxValue);
	Mat CheckFaceV(CvMat* RImageMat, CvMat* UVImageMat, CvMat* DImageMat, float SLOPE_THRESHOLD);
	Mat CheckFaceU(CvMat* RImageMat, CvMat* UVImageMat, CvMat* DImageMat, float SLOPE_THRESHOLD);
	Mat GridMap(IplImage* _DImage, float OPTICAL_X, float BASELINE, float FU, int X_THRESHOLD, int Z_THRESHOLD);
};

