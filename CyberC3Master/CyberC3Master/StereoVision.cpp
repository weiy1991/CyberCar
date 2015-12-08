#include "stdafx.h"
#include "StereoVision.h"


StereoVision::StereoVision()
{
	DImage = NULL;	
	CDImage = NULL;  
	LImage = NULL;	
	RImage = NULL;
	VImage = NULL;	 
	UImage = NULL;	
}

StereoVision::StereoVision(IplImage* _DImage)
{
	DImage = _DImage;
	CDImage = NULL;
	LImage = NULL;
	RImage = NULL;
	VImage = cvCreateImage(cvSize(_DImage->height, 255), CV_8UC1, 1);
	UImage = cvCreateImage(cvSize(255, _DImage->width), IPL_DEPTH_8U, 1);
	VImageCvMat = cvCreateMat(_DImage->height, 255, CV_8UC1);
	UImageCvMat = cvCreateMat(255, _DImage->width, CV_8UC1);
	GridCvMat = cvCreateMat(200, 200, CV_8UC1);
}

StereoVision::StereoVision(IplImage* _DImage, IplImage* _CDImage)
{
	DImage = _DImage;
	CDImage = _CDImage;
	LImage = NULL;
	RImage = NULL;
	VImage = cvCreateImage(cvSize(_DImage->height, 255), CV_8UC1, 1);
	UImage = cvCreateImage(cvSize(255, _DImage->width), IPL_DEPTH_8U, 1);
	VImageCvMat = cvCreateMat(_DImage->height, 255, CV_8UC1);
	UImageCvMat = cvCreateMat(255, _DImage->width, CV_8UC1);
	GridCvMat = cvCreateMat(200, 200, CV_8UC1);
}

StereoVision::StereoVision(IplImage* _DImage, IplImage* _CDImage, IplImage* _LRImage)
{
	DImage = _DImage;
	CDImage = _CDImage;
	LImage = _LRImage;
	RImage = NULL;
	VImage = cvCreateImage(cvSize(_DImage->height, 255), CV_8UC1, 1);
	UImage = cvCreateImage(cvSize(255, _DImage->width), IPL_DEPTH_8U, 1);
	VImageCvMat = cvCreateMat(_DImage->height, 255, CV_8UC1);
	UImageCvMat = cvCreateMat(255, _DImage->width, CV_8UC1);
	ResultCvMat = cvCreateMat(_LRImage->height, _LRImage->width, CV_8UC1);
	GridCvMat = cvCreateMat(200, 200, CV_8UC1);
}

StereoVision::StereoVision(IplImage* _DImage, IplImage* _CDImage, IplImage* _LImage, IplImage* _RImage)
{
	DImage = _DImage;
	CDImage = _CDImage;
	LImage = _LImage;
	RImage = _RImage;
	VImage = cvCreateImage(cvSize(_DImage->height, 255), CV_8UC1, 1);
	UImage = cvCreateImage(cvSize(255, _DImage->width), IPL_DEPTH_8U, 1);
	VImageCvMat = cvCreateMat(_DImage->height, 255, CV_8UC1);
	UImageCvMat = cvCreateMat(255, _DImage->width, CV_8UC1);
	ResultCvMat = cvCreateMat(_RImage->height, _RImage->width, CV_8UC1);
	GridCvMat = cvCreateMat(200, 200, CV_8UC1);
}

StereoVision::~StereoVision()
{
	cvReleaseImage(&DImage);
	cvReleaseImage(&CDImage);
	cvReleaseImage(&LImage);
	cvReleaseImage(&RImage);
	cvReleaseImage(&VImage);
	cvReleaseImage(&UImage);
	cvReleaseMat(&VImageCvMat);
	cvReleaseMat(&UImageCvMat);
	cvReleaseMat(&ResultCvMat);
	cvReleaseMat(&GridCvMat);
}

CvMat* StereoVision::VDisparityCalculate(IplImage* _DImage, int VMinValue, int VMaxValue)
{
	Mat tempMat = _DImage;

    //#pragma omp parallel for

	for (int row = 0; row<tempMat.rows; row++)
	{
		for (int col = 0; col<tempMat.cols; col++)
		{
			if (tempMat.at<unsigned char>(row, col)>9 && tempMat.at<unsigned char>(row, col)<246)
				(VImageCvMat->data.ptr + row * VImageCvMat->step)[(tempMat.at<unsigned char>(row, col))]++;
		}
	}
	return VImageCvMat;
}

CvMat* StereoVision::UDisparityCalculate(IplImage* _DImage, int UMinValue, int UMaxValue)
{
	Mat tempMat = _DImage;
	for (int row = 0; row<tempMat.rows; row++)
	{
		for (int col = 0; col<tempMat.cols; col++)
		{
			if (tempMat.at<unsigned char>(row, col)>9 && tempMat.at<unsigned char>(row, col)<246)
				(UImageCvMat->data.ptr + (tempMat.at<unsigned char>(row, col)) * (UImageCvMat->step))[col]++;
		}
	}
	return UImageCvMat;
}

Mat StereoVision::CheckFaceV(CvMat* RImageMat, CvMat* UVImageMat, CvMat* DImageMat, float SLOPE_THRESHOLD)
{
	Mat tempRImageMat = RImageMat;
	Mat tempMat = DImageMat;

	Mat Rimg(tempRImageMat, Rect(0, 0, 640, 480));//感兴趣区域
	//Mat vdisp2(VImageMat,true);
	Mat vdisp2(UVImageMat, true);

	IplImage ipl_Rimg = Rimg;
	IplImage *Rimg_red;
	Rimg_red = cvCreateImage(cvSize(ipl_Rimg.width, ipl_Rimg.height), IPL_DEPTH_8U, 3);//3通道图像
	cvCvtColor(&ipl_Rimg, Rimg_red, CV_GRAY2BGR);
	/*cvNamedWindow("三通道图", CV_WINDOW_AUTOSIZE);*/
	cvShowImage("三通道图", Rimg_red);
	Mat Rimg_redm = Rimg_red;//3通道矩阵

	/*************Hough变换***************/

	IplImage ipl_disp = vdisp2;
	IplImage *ipl_dispB = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
	int pos = 12;
	cvThreshold(&ipl_disp, ipl_dispB, pos, 255, CV_THRESH_BINARY);//二值化处理
	cvNamedWindow("二值化图", CV_WINDOW_AUTOSIZE);
	cvShowImage("二值化图", ipl_dispB);

	IplImage *ipl_dispCanny = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
	int threshold = 60;
	cvCanny(&ipl_disp, ipl_dispCanny, threshold, threshold * 3, 3);    //边缘检测处理
	cvNamedWindow("边缘检测图", CV_WINDOW_AUTOSIZE);
	cvShowImage("边缘检测图", ipl_dispCanny);

	CvMemStorage *storage = cvCreateMemStorage();
	double fRho = 1;
	double fTheta = CV_PI / 180;
	int nMaxLineNumber = 50;   //概率累积值
	double fMinLineLen = 50;   //最小线段长度
	double fMinLineGap = 10;   //最小线段间隔
	CvSeq *pcvSeqLines = cvHoughLines2(ipl_dispB, storage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);

	IplImage *pColorImage = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 3);
	cvCvtColor(ipl_dispB, pColorImage, CV_GRAY2BGR);
	int i, j;
	int n0 = 0, n1 = 0;
	int m0 = 0, m1 = 0;
	int m = 0;
	int q, p;
	float b;
	Mat dispnew = Mat::zeros(RImageMat->rows, RImageMat->cols, CV_8UC1);

	for (i = 0; i < pcvSeqLines->total; i++)
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, i);
		cvLine(pColorImage, line[0], line[1], CV_RGB(255, 0, 0), 2);//在二值化图像中画直线

		if (line[0].x != line[1].x){
			float slope = abs((line[0].y - line[1].y) / (line[0].x - line[1].x));
			/* b = (line[1].x - line[0].x)/(line[1].y - line[0].y);*/
			b = line[0].y - slope * line[0].x;
			if (slope<SLOPE_THRESHOLD){
				for (int q = line[0].y; q<line[1].y; q++)
				{
					/*j = line[0].x;
					j += b;*/
					j = (q - b) / slope;
					//cout<<"深度值"<<j<<endl;
					for (int p = 0; p<tempMat.cols; p++)
					{
						if (tempMat.at<unsigned char>(q, p) <= (j + 2) && tempMat.at<unsigned char>(q, p) >= (j - 30)) 
						{
							dispnew.at<unsigned char>(q, p) = 255;
							Rimg_redm.at<unsigned char>(q, p*Rimg_redm.channels()) = 0;//B通道
							Rimg_redm.at<unsigned char>(q, p*Rimg_redm.channels() + 1) = 255;//G通道
							Rimg_redm.at<unsigned char>(q, p*Rimg_redm.channels() + 2) = 0;//R通道
							(ResultCvMat->data.ptr + q * ResultCvMat->step)[p] = 255;//可通行区域
							/*if(q==line[0].y)
							cvCircle(&IplImage(Rimg_redm),cvPoint(q,p),6,CV_RGB(255,255,0),1,6,0);*/
						}
						else dispnew.at<unsigned char>(q, p) = 0;
					}
				}
			}
		}
	}
	

	cvNamedWindow("houghLine", CV_WINDOW_AUTOSIZE);
	cvShowImage("houghLine", pColorImage);

	return Rimg_redm;
}

Mat StereoVision::CheckFaceU(CvMat* RImageMat, CvMat* UVImageMat, CvMat* DImageMat, float SLOPE_THRESHOLD)
{
	Mat tempRImageMat = RImageMat;
	Mat tempMat = DImageMat;

	Mat Rimg(tempRImageMat, Rect(0, 0, 640, 480));//感兴趣区域
	//Mat vdisp2(VImageMat,true);
	Mat vdisp2(UVImageMat, true);

	IplImage ipl_Rimg = Rimg;
	IplImage *Rimg_red;
	Rimg_red = cvCreateImage(cvSize(ipl_Rimg.width, ipl_Rimg.height), IPL_DEPTH_8U, 3);//3通道图像
	cvCvtColor(&ipl_Rimg, Rimg_red, CV_GRAY2BGR);
	/*cvNamedWindow("三通道图", CV_WINDOW_AUTOSIZE);*/
	cvShowImage("三通道图", Rimg_red);
	Mat Rimg_redm = Rimg_red;//3通道矩阵

	/*************Hough变换***************/

	IplImage ipl_disp = vdisp2;
	IplImage *ipl_dispB = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
	int pos = 12;
	cvThreshold(&ipl_disp, ipl_dispB, pos, 255, CV_THRESH_BINARY);//二值化处理
	cvNamedWindow("二值化图", CV_WINDOW_AUTOSIZE);
	cvShowImage("二值化图", ipl_dispB);

	IplImage *ipl_dispCanny = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
	int threshold = 60;
	cvCanny(&ipl_disp, ipl_dispCanny, threshold, threshold * 3, 3);    //边缘检测处理
	cvNamedWindow("边缘检测图", CV_WINDOW_AUTOSIZE);
	cvShowImage("边缘检测图", ipl_dispCanny);

	CvMemStorage *storage = cvCreateMemStorage();
	double fRho = 1;
	double fTheta = CV_PI / 180;
	int nMaxLineNumber = 50;   //概率累积值
	double fMinLineLen = 50;   //最小线段长度
	double fMinLineGap = 10;   //最小线段间隔
	CvSeq *pcvSeqLines = cvHoughLines2(ipl_dispB, storage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);

	IplImage *pColorImage = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 3);
	cvCvtColor(ipl_dispB, pColorImage, CV_GRAY2BGR);
	int i, j;
	int n0 = 0, n1 = 0;
	int m0 = 0, m1 = 0;
	int m = 0;
	int q, p;
	float b;
	Mat dispnew = Mat::zeros(RImageMat->rows, RImageMat->cols, CV_8UC1);

	//IplImage

	
	for (i = 0; i < pcvSeqLines->total; i++){
		CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, i);
		cvLine(pColorImage, line[0], line[1], CV_RGB(255, 0, 0), 2);//在二值化图像中画直线

		if (line[0].x != line[1].x){
			//float slope = abs((line[0].y - line[1].y)/(line[0].x - line[1].x));
			float slope = (line[0].y - line[1].y) / (line[0].x - line[1].x);
			/* b = (line[1].x - line[0].x)/(line[1].y - line[0].y);*/
			b = line[0].y - slope * line[0].x;
			if (/*slope>=0&&*/slope<SLOPE_THRESHOLD){
				for (int q = line[0].x; q<line[1].x; q++){
					/*j = line[0].x;
					j += b;*/
					j = slope*q + b;
					//cout<<"深度值"<<j<<endl;
					for (int p = 0; p<tempMat.rows; p++){
						if (tempMat.at<unsigned char>(p, q) <= (j + 2) && tempMat.at<unsigned char>(p, q) >= (j - 30)) {//对像素进行操作
							dispnew.at<unsigned char>(p, q) = 255;
							Rimg_redm.at<unsigned char>(p, q*Rimg_redm.channels()) = 0;//B通道
							Rimg_redm.at<unsigned char>(p, q*Rimg_redm.channels() + 1) = 0;//G通道
							Rimg_redm.at<unsigned char>(p, q*Rimg_redm.channels() + 2) = 255;//R通道
							(ResultCvMat->data.ptr + q * ResultCvMat->step)[p] = 255;//可通行区域
							/*if(q==line[0].y)
							cvCircle(&IplImage(Rimg_redm),cvPoint(q,p),6,CV_RGB(255,255,0),1,6,0);*/
						}
						else dispnew.at<unsigned char>(p, q) = 0;
					}
				}
			}
		}
	}
	
	cvNamedWindow("houghLine", CV_WINDOW_AUTOSIZE);
	cvShowImage("houghLine", pColorImage);

	return Rimg_redm;
}

Mat StereoVision::GridMap(IplImage* _DImage, float OPTICAL_X,float BASELINE, float FU, int X_THRESHOLD, int Z_THRESHOLD)
{
	Mat GridMatResult;
	Mat _DImageMat = _DImage;
	//clear the mat
	cvZero(GridCvMat);
	//摄像头坐标到世界坐标的坐标映射
	for (int row = 0; row<ResultCvMat->rows; row++)
	{
		for (int col = 0; col<ResultCvMat->cols; col++)
		{
			if ((ResultCvMat->data.ptr + row * ResultCvMat->step)[col] == 255)
			{
				float Bd = BASELINE / (_DImageMat.at<unsigned char>(row, col));
				float x = (col - OPTICAL_X)*Bd;
				float z = FU*Bd;
				//写入栅格图，每个像素代表50mm*50mm
				if (abs(x)<X_THRESHOLD&&z<Z_THRESHOLD)
				{
					int _row = GridCvMat->rows - (int)z / 50;
					int _col = GridCvMat->cols / 2 + (int)x / 50;
					(GridCvMat->data.ptr + _row * GridCvMat->step)[_col] = 255;
				}
			}
		}
	}
	GridMatResult = GridCvMat;
	return GridMatResult;
}