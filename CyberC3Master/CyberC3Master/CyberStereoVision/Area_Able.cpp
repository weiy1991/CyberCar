// Area_Able.cpp : 定义控制台应用程序的入口点。
// 单帧算法验证
// U视差和V视差分割

#include <iostream>
#include "opencv2\opencv.hpp"
#include "opencv2\legacy\legacy.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <Windows.h>
#include <sys/timeb.h>
#include <string.h>

using namespace std;
using namespace cv;

//阈值参数
#define SLOPE_THRESHOLD 35  //斜率阈值
#define WIDTH 640
#define HEIGHT 480
#define _WIDTH 200
#define _HEIGHT 200
#define VALUE 255
#define BASELINE 149.532
#define OPTICAL_X 317.3
#define OPTICAL_Y 247.2
#define FU 430
#define SCALE 0.5//图像缩放比例系数
#define CANNY_THRESHOLD 240
#define OBJECT_THRESHOLD 10
#define OBJECT_BLOCK 20
#define RECT_OFFSET 10 //矩形框的偏移量
#define IMAGE_OFFSET 10 //原图和视差图的偏移量

#define X_THRESHOLD 5000
#define Z_THRESHOLD 5000

//全局变量
Mat VResult;
Mat UResult;
Mat UVResult;
CvMat* UVResultMat = cvCreateMat(HEIGHT,WIDTH,CV_8UC1);//定义可通行区域矩阵
CvMat* GridMat = cvCreateMat(_WIDTH,_WIDTH,CV_8UC1);//定义可通行区域栅格图矩阵

//IplImage* UVResultImage =  cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3);//定义三通道图像

Mat CheckFace(CvMat* RImageMat,CvMat* UVImageMat,CvMat* DImageMat,int UV);//直线检测及区域检测
void ObjectSplit(IplImage* _DImage,IplImage* _RImageRGB);//目标分割
void GridMap(CvMat* DImageMat);//栅格图映射

//打印函数，打印相关的矩阵、图像信息
void printInfo(char* name, Mat* mat, CvMat* cvmat, IplImage* image)
{
	if(mat!=NULL)
	{
		int x=mat->rows;
		int y=mat->cols;
		cout<<name<<":"<<x<<"*"<<y<<endl;
	}
	if(cvmat!=NULL)
	{
		int x=cvmat->rows;
		int y=cvmat->cols;
		cout<<name<<":"<<x<<"*"<<y<<endl;
	}
	if(image!=NULL)
	{
		int x=image->height;
		int y=image->width;
		cout<<name<<":"<<x<<"*"<<y<<endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//声明图像指针
	IplImage* DImageRGB = NULL;//深度图的RGB图
	IplImage* RImageRGB = NULL;//右摄像头的RGB图
	IplImage* DImage = NULL;//深度图的灰度图
	IplImage* RImage = NULL;//右摄像头的灰度图
	IplImage* CImage = NULL;//颜色视差图
	IplImage* VImage = NULL;//V视差图
	IplImage* UImage = NULL;//U视差图
	//IplImage* DCannyImg = NULL;//canny检测图

	CvMat* DImageMat = NULL;
	CvMat* RImageMat = NULL;
	CvMat* CImageMat = NULL;
	CvMat* VImageMat = NULL;
	CvMat* UImageMat = NULL;

	//CvCapture*  DCapture = cvCaptureFromAVI("RecFile_2_20150412_114419_Vislab_3DV_1_DSI.avi");
	CvCapture*  RCapture = cvCaptureFromAVI("H:/DATA/wurenchuanyanzheng/20150523_042458_RecFile_2/RecFile_2_20150523_042458_Vislab_3DV_1_Right_Rectified.avi");
	CvCapture*  CCapture = cvCaptureFromAVI("H:/DATA/20150507_144932_RecFile_3_jpeghuojia_low2/RecFile_3_20150507_144932_Vislab_DSI_To_Color_6_DSI_To_Color.avi");

	//图像帧数
	int nFrmNum = 0;

	//创建窗口
	cvNamedWindow("DImage",1);
	cvNamedWindow("RImage",1);
	cvNamedWindow("CImage",1);
	cvNamedWindow("VImage",1);
	cvNamedWindow("UImage",1);
	//cvNamedWindow("DCannyImg",1);

	//使窗口有序排列
	cvMoveWindow("DImage",30,0);
	cvMoveWindow("RImage",300,0);
	cvMoveWindow("CImage",570,0);
	cvMoveWindow("VImage",840,0);
	cvMoveWindow("UImage",30,0);
	//cvMoveWindow("DCannyImg",300,0);

	//初始化重要的矩阵
	for(int row=0;row<HEIGHT;row++)
	{
		for(int col=0;col<WIDTH;col++)
			(UVResultMat->data.ptr + row * UVResultMat->step)[col] = 0;   
	}

	for(int row=0;row<_HEIGHT;row++)
	{
		for(int col=0;col<_WIDTH;col++)
			(GridMat->data.ptr + row * GridMat->step)[col] = 0;   
	}

	
	int image_num = 500;	
	//逐帧读取视频
	while( /*(DImageRGB = cvQueryFrame(DCapture)) &&*/ (RImageRGB = cvQueryFrame(RCapture)) && (CImage = cvQueryFrame(CCapture)) )
	{
		
		char a[2]={' ','0'};
		char buffer[100];
		itoa( image_num, buffer, 10 ); 
		
		//a[0] = image_num++;
		string str1="H:/DATA/wurenchuanyanzheng/wurechuan_yanzheng/20150523_133046_RecFile_6/RecFile_6_20150523_133046_Vislab_3DV_1_DSI_";
		string str2="450";
		string str3=".jpg";
		string str=str1+buffer+str3;
		cout<<"帧数：  "<<nFrmNum<<endl;
		nFrmNum++;	
		//DImageRGB = cvLoadImage("H:/DATA/wurenchuanyanzheng/wurechuan_yanzheng/20150523_133046_RecFile_6/RecFile_6_20150523_133046_Vislab_3DV_1_DSI_450.jpg",1);
		DImageRGB = cvLoadImage(str.data(),1);
		//若是第一帧，需要申请内存
		if(nFrmNum ==1 )
		{
			DImage = cvCreateImage(cvGetSize(DImageRGB), IPL_DEPTH_8U, 1);
			RImage = cvCreateImage(cvGetSize(RImageRGB), IPL_DEPTH_8U, 1);
			VImage = cvCreateImage(cvSize(DImageRGB->height,255), IPL_DEPTH_8U, 1);//V视差图尺寸：255*480
			UImage = cvCreateImage(cvSize(255,DImageRGB->width), IPL_DEPTH_8U, 1);//U视差图尺寸：640*255
			//DCannyImg= cvCreateImage(cvGetSize(DImageRGB),IPL_DEPTH_8U,1);

			DImageMat = cvCreateMat(DImageRGB->height,DImageRGB->width,CV_8UC1);
			RImageMat = cvCreateMat(RImageRGB->height,RImageRGB->width,CV_8UC1);
			CImageMat = cvCreateMat(CImage->height,CImage->width,CV_8UC3);
			VImageMat = cvCreateMat(DImageRGB->height,255,CV_8UC1);
			UImageMat = cvCreateMat(255,DImageRGB->width,CV_8UC1);

			for(int row=0;row<HEIGHT;row++)
			{
				for(int col=0;col<VALUE;col++)
					(VImageMat->data.ptr + row * VImageMat->step)[col] = 0;   
			}
			for(int row=0;row<VALUE;row++)
			{
				for(int col=0;col<WIDTH;col++)
					(UImageMat->data.ptr + row * UImageMat->step)[col] = 0;   
			}
		}

		//控制处理帧数
		if(nFrmNum<2)
			continue;

		//将3通道图转化为单通道图
		cvCvtColor(DImageRGB,DImage,CV_BGR2GRAY);
	    cvCvtColor(RImageRGB,RImage,CV_BGR2GRAY);

		//将图像转化为矩阵形式
		cvConvert( DImage, DImageMat );
		cvConvert( RImage, RImageMat );

		/****************************深度图边缘检测**************************/
		//cvCanny(DImage,DCannyImg,50,150,3);
		ObjectSplit(DImage,RImageRGB);

		//对原始图像进行预处理
		/*****************************V-disparity*****************************/
		Mat tempMat = DImageMat;
		
		struct timeb startTime,endTime;
	    ftime(&startTime);

		/*for(int row=0;row<tempMat.rows;row++){
			unsigned char* q = (unsigned char*)(VImageMat->data.ptr + row * VImageMat->step);   
			for(int i=0;i< 255;i++){                            
				int j=0;
				for(int col=0;col<tempMat.cols;col++){                  
					if(tempMat.at<unsigned char>(row,col)<10 || tempMat.at<unsigned char>(row,col)>245) j=j;
					else if(tempMat.at<unsigned char>(row,col)==i){
						j++;
					}
				}
				*q = j;
				q++;	                       
			}
        }*/

		//新的V视差统计算法
		for(int row=0;row<tempMat.rows;row++)
		{
			for(int col=0;col<tempMat.cols;col++)
			{
				if(tempMat.at<unsigned char>(row,col)>9 && tempMat.at<unsigned char>(row,col)<246)
					(VImageMat->data.ptr + row * VImageMat->step)[(tempMat.at<unsigned char>(row,col))]++;
			}
		}
		
		cvGetImage(VImageMat,VImage);
		
		/*****************************U-disparity******************************/
		Mat _tempMat = DImageMat;

		
		//for(int col=0;col<_tempMat.cols;col++){
		//	unsigned char* _q = (unsigned char*)(UImageMat->data.ptr + col*1);//首行的首地址（即是首列的首地址）
		//	for(int i=0;i< 255;i++){                            
		//		int j=0;
		//		for(int row=0;row<_tempMat.rows;row++){                  
		//			if(_tempMat.at<unsigned char>(row,col)<10 || _tempMat.at<unsigned char>(row,col)>245) j=j;
		//			else if(_tempMat.at<unsigned char>(row,col)==i){
		//				j++;
		//			}
		//		}
		//		*_q = j;
		//		_q = _q+UImageMat->step;	                       
		//	}
  //      }

		//新的U视差统计算法
		for(int col=0;col<_tempMat.cols;col++)
		{
			for(int row=0;row<_tempMat.rows;row++)
			{
				if(_tempMat.at<unsigned char>(row,col)>9 && _tempMat.at<unsigned char>(row,col)<246)
					(UImageMat->data.ptr + (tempMat.at<unsigned char>(row,col)) * (UImageMat->step))[col]++;
			}
		}
		
		cvGetImage(UImageMat,UImage);

		
		/*****************************霍夫变换寻找可通行区域*****************************/
		
		VResult = CheckFace(RImageMat,VImageMat,DImageMat,0);
		UResult = CheckFace(RImageMat,UImageMat,DImageMat,1);
		UVResult = UResult;

		/*****************************栅格图计算*****************************/
		GridMap(DImageMat);
		
		//显示图像
		cvShowImage("DImage",DImage);
		cvShowImage("RImage",RImage);
		cvShowImage("CImage",CImage);
		cvShowImage("VImage",VImage);
		cvShowImage("UImage",UImage);
		//cvShowImage("DCannyImg",DCannyImg);

		IplImage Rimg_redmimage = UVResult;
		cvNamedWindow("FaceCkeck", CV_WINDOW_AUTOSIZE);
	    cvShowImage("FaceCkeck", &Rimg_redmimage);

		IplImage* UVResultImage;
		cvGetImage(UVResultMat,UVResultImage);
		cvNamedWindow("UVResultImage", CV_WINDOW_AUTOSIZE);
	    cvShowImage("UVResultImage", UVResultImage);

		IplImage* GridMatImage;
		cvGetImage(GridMat,GridMatImage);
		cvNamedWindow("GridMatImage", CV_WINDOW_AUTOSIZE);
	    cvShowImage("GridMatImage", GridMatImage);
		

		/*cvNamedWindow("houghLine", CV_WINDOW_AUTOSIZE);
	    cvShowImage("houghLine", pColorImage);*/

		cvWaitKey(1);
		//Sleep(1000);
		ftime(&endTime);
	    float time = (endTime.time-startTime.time)*1000+(endTime.millitm-startTime.millitm);
	    cout <<"V视差图的计算时间打印"<<endl<<"time为"<<time<<"原始的计算时间为"<<(endTime.time-startTime.time)*1000+(endTime.millitm-startTime.millitm)<<"毫秒"<<endl;

		//只处理一帧数据
		if(nFrmNum>1)
		{
			while(1)
				cvWaitKey(1);
		}	
	}

	//销毁窗口
    cvDestroyWindow("DImage");
    cvDestroyWindow("RImage");
    cvDestroyWindow("CImage");
	cvDestroyWindow("VImage");
	cvDestroyWindow("UImage");
	//cvDestroyWindow("DCannyImg");
 
    //释放图像和矩阵
	cvReleaseImage(&DImage);
	cvReleaseImage(&RImage);
	cvReleaseImage(&VImage);
	cvReleaseImage(&UImage);
	//cvReleaseImage(&DCannyImg);

    cvReleaseMat(&DImageMat);
    cvReleaseMat(&RImageMat);
    cvReleaseMat(&CImageMat);
	cvReleaseMat(&VImageMat);
	cvReleaseMat(&UImageMat);
 
    //cvReleaseCapture(&DCapture);
	cvReleaseCapture(&RCapture);
	cvReleaseCapture(&CCapture);

	return 0;
}


Mat CheckFace(CvMat* RImageMat,CvMat* UVImageMat,CvMat* DImageMat,int UV)
{
	Mat tempRImageMat = RImageMat;
	Mat tempMat = DImageMat;

		Mat Rimg(tempRImageMat,Rect(0,0,640,480));//感兴趣区域
		//Mat vdisp2(VImageMat,true);
		Mat vdisp2(UVImageMat,true);

		IplImage ipl_Rimg = Rimg;
		IplImage *Rimg_red;
		Rimg_red =  cvCreateImage(cvSize(ipl_Rimg.width,ipl_Rimg.height), IPL_DEPTH_8U, 3);//3通道图像
		cvCvtColor(&ipl_Rimg, Rimg_red, CV_GRAY2BGR);
		/*cvNamedWindow("三通道图", CV_WINDOW_AUTOSIZE);*/
		cvShowImage("三通道图",Rimg_red);
		Mat Rimg_redm = Rimg_red;//3通道矩阵

		//判断是否是V视差后进行U视差的检测
		if(UV==1)
			Rimg_redm = VResult;

		//time estimation
		struct timeb startTime1,endTime1;
		ftime(&startTime1);
		/*************Hough变换***************/

		IplImage ipl_disp = vdisp2;
		IplImage *ipl_dispB = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
		int pos = 12;
		cvThreshold(&ipl_disp,ipl_dispB,pos,255,CV_THRESH_BINARY);//二值化处理
		cvNamedWindow("二值化图", CV_WINDOW_AUTOSIZE);
		cvShowImage("二值化图",ipl_dispB);

		IplImage *ipl_dispCanny = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 1);
		int threshold = 60;
		cvCanny(&ipl_disp,ipl_dispCanny,threshold,threshold*3,3);    //边缘检测处理
		cvNamedWindow("边缘检测图", CV_WINDOW_AUTOSIZE);
		cvShowImage("边缘检测图",ipl_dispCanny);

		CvMemStorage *storage = cvCreateMemStorage();
		double fRho = 1;
		double fTheta = CV_PI / 180;
		int nMaxLineNumber = 50;   //概率累积值
		double fMinLineLen = 50;   //最小线段长度
		double fMinLineGap = 10;   //最小线段间隔
		CvSeq *pcvSeqLines = cvHoughLines2(ipl_dispB, storage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);
		
		cout<<"直线条数"<<pcvSeqLines->total<<endl;

		IplImage *pColorImage = cvCreateImage(cvGetSize(&ipl_disp), IPL_DEPTH_8U, 3);
		cvCvtColor(ipl_dispB, pColorImage, CV_GRAY2BGR);
		int i,j;
		int n0 = 0,n1 = 0;
		int m0 = 0,m1 = 0;
		int m =0;
		int q,p;
		float b;
		Mat dispnew = Mat::zeros(RImageMat->rows,RImageMat->cols,CV_8UC1);
		
		//IplImage
	
		//V视差的平面检测
		if(UV==0)
		{
			for(i = 0; i < pcvSeqLines->total; i++){
				CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, i);
				cvLine(pColorImage, line[0], line[1], CV_RGB(255,0,0), 2);//在二值化图像中画直线
		
				if(line[0].x != line[1].x){
					  float slope = abs((line[0].y - line[1].y)/(line[0].x - line[1].x));
					 /* b = (line[1].x - line[0].x)/(line[1].y - line[0].y);*/
					  b = line[0].y - slope * line[0].x;
					  if(slope<SLOPE_THRESHOLD){
							  for(int q = line[0].y;q<line[1].y;q++){
								  /*j = line[0].x;
								  j += b;*/
								  j = (q-b)/slope;
								  //cout<<"深度值"<<j<<endl;
								  for(int p=0;p<tempMat.cols;p++){
									  if(tempMat.at<unsigned char>(q,p)<= (j+2) && tempMat.at<unsigned char>(q,p)>=(j-30)) {//对像素进行操作
										  dispnew.at<unsigned char>(q,p)=255;
										  Rimg_redm.at<unsigned char>(q,p*Rimg_redm.channels())=0;//B通道
										  Rimg_redm.at<unsigned char>(q,p*Rimg_redm.channels()+1)=255;//G通道
										  Rimg_redm.at<unsigned char>(q,p*Rimg_redm.channels()+2)=0;//R通道
										  (UVResultMat->data.ptr + q * UVResultMat->step)[p] = 255;//可通行区域
										  /*if(q==line[0].y)
											  cvCircle(&IplImage(Rimg_redm),cvPoint(q,p),6,CV_RGB(255,255,0),1,6,0);*/
									  }
									  else dispnew.at<unsigned char>(q,p)=0;
								  }
							  }
					  }
				}
			}
		}

		//U视差的平面检测
		else if(UV==1)
		{
			for(i = 0; i < pcvSeqLines->total; i++){
				CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, i);
				cvLine(pColorImage, line[0], line[1], CV_RGB(255,0,0), 2);//在二值化图像中画直线
		
				if(line[0].x != line[1].x){
					  //float slope = abs((line[0].y - line[1].y)/(line[0].x - line[1].x));
					float slope = (line[0].y - line[1].y)/(line[0].x - line[1].x);
					 /* b = (line[1].x - line[0].x)/(line[1].y - line[0].y);*/
					  b = line[0].y - slope * line[0].x;
					  if(/*slope>=0&&*/slope<SLOPE_THRESHOLD){
							  for(int q = line[0].x;q<line[1].x;q++){
								  /*j = line[0].x;
								  j += b;*/
								  j = slope*q + b;
								  //cout<<"深度值"<<j<<endl;
								  for(int p=0;p<tempMat.rows;p++){
									  if(tempMat.at<unsigned char>(p,q)<= (j+2) && tempMat.at<unsigned char>(p,q)>=(j-30)) {//对像素进行操作
										  dispnew.at<unsigned char>(p,q)=255;
										  Rimg_redm.at<unsigned char>(p,q*Rimg_redm.channels())=0;//B通道
										  Rimg_redm.at<unsigned char>(p,q*Rimg_redm.channels()+1)=0;//G通道
										  Rimg_redm.at<unsigned char>(p,q*Rimg_redm.channels()+2)=255;//R通道
										  /*if(q==line[0].y)
											  cvCircle(&IplImage(Rimg_redm),cvPoint(q,p),6,CV_RGB(255,255,0),1,6,0);*/
									  }
									  else dispnew.at<unsigned char>(p,q)=0;
								  }
							  }
					  }
				}
			}
		}

		cvNamedWindow("houghLine", CV_WINDOW_AUTOSIZE);
	    cvShowImage("houghLine", pColorImage);

		return Rimg_redm;

}

void GridMap(CvMat* DImageMat)
{
	Mat GridMatResult;
	Mat _DImageMat = DImageMat;
	//clear the mat
	for(int row=0;row<GridMat->rows;row++)
		for(int col=0;col<GridMat->cols;col++)
			(GridMat->data.ptr + row * GridMat->step)[col] = 0;
	//摄像头坐标到世界坐标的坐标映射
	for(int row=0;row<UVResultMat->rows;row++)
	{
		for(int col=0;col<UVResultMat->cols;col++)
		{
			if((UVResultMat->data.ptr + row * UVResultMat->step)[col] == 255)
			{
				float Bd = BASELINE/(_DImageMat.at<unsigned char>(row,col));
				float x = (col-OPTICAL_X)*Bd;
				float z = FU*Bd;
				//cout<<x<<"   "<<z<<endl;
				//写入栅格图，每个像素代表50mm*50mm
				if(abs(x)<X_THRESHOLD&&z<Z_THRESHOLD)
				{
					int _row = _HEIGHT - (int)z/50;
					int _col = _WIDTH/2 + (int)x/50;
					(GridMat->data.ptr + _row * GridMat->step)[_col] = 255;
				}
			}
		}
	}

}

void ObjectSplit(IplImage* _DImage,IplImage* _RImageRGB)
{
	struct timeb startTime,endTime;
	ftime(&startTime);

	IplImage* DImage = NULL;
	IplImage* DCannyImage = NULL;
	IplImage* LineColorImage = NULL;
	IplImage* DCannyImageB = NULL;
	IplImage* DCannyImageOpt = NULL;
	IplImage* ObjectImage = NULL;
	IplImage* RObjectImage = NULL;


	cvNamedWindow("_DImage",1);
	cvNamedWindow("DCannyImage",1);
	cvNamedWindow("LineColorImage",1);
	cvNamedWindow("DCannyImageB",1);
	cvNamedWindow("DCannyImageOpt",1);
	cvNamedWindow("ObjectImage",1);
	cvNamedWindow("RObjectImage",1);
	
	cvMoveWindow("_DImage",30,0);
	cvMoveWindow("DCannyImage",60,0);
	cvMoveWindow("LineColorImage",90,0);
	cvMoveWindow("DCannyImageB",120,0);
	cvMoveWindow("DCannyImageOpt",30,30);
	cvMoveWindow("ObjectImage",60,30);
	cvMoveWindow("RObjectImage",90,30);

	DImage = cvCreateImage(cvGetSize(_DImage),IPL_DEPTH_8U,1);
	DCannyImage = cvCreateImage(cvGetSize(_DImage),IPL_DEPTH_8U,1);
    LineColorImage = cvCreateImage(cvGetSize(_DImage), IPL_DEPTH_8U, 3);
	DCannyImageB = cvCreateImage(cvGetSize(_DImage), IPL_DEPTH_8U, 1);
	DCannyImageOpt = cvCreateImage(cvGetSize(_DImage), IPL_DEPTH_8U, 1);
	ObjectImage = cvCreateImage(cvGetSize(_DImage), IPL_DEPTH_8U, 3);
	RObjectImage = cvCreateImage(cvGetSize(_DImage), IPL_DEPTH_8U, 3);
		
	//梯度边缘检测
	DImage = _DImage;
    cvCanny(DImage,DCannyImage,50,150,3);
	cvCvtColor(DImage, LineColorImage, CV_GRAY2BGR);

	//二值化处理
	int pos = 12;
	cvThreshold(DCannyImage,DCannyImageB,pos,255,CV_THRESH_BINARY);//二值化处理
	
	//霍夫变换检测直线
	CvMemStorage *storage = cvCreateMemStorage();
	double fRho = 1;
	double fTheta = CV_PI / 180;
	int nMaxLineNumber = 50;   //概率累积值
	double fMinLineLen = 50;   //最小线段长度
	double fMinLineGap = 10;   //最小线段间隔
	CvSeq *pcvSeqLines = cvHoughLines2(DCannyImageB, storage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);
	cout<<"目标分割直线条数"<<pcvSeqLines->total<<endl;

	//在边缘检测图中画出检测出的直线
	for(int i=0;i<pcvSeqLines->total;i++)
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, i);
		cvLine(LineColorImage, line[0], line[1], CV_RGB(255,0,0), 2);
	}

	//对梯度计算二值化后的图进行未匹配区域的消除
	DCannyImageOpt = DCannyImageB;
	CvMat* DCannyImageOptMat = cvCreateMat(DCannyImageOpt->height,DCannyImageOpt->width,CV_8UC1);
	CvMat* DImageMat = cvCreateMat(DImage->height,DImage->width,CV_8UC1);
	cvConvert( DCannyImageOpt, DCannyImageOptMat );
	cvConvert( DImage, DImageMat );
	for(int row=0;row<DCannyImageOpt->height;row++)
	{
		for(int col=0;col<DCannyImageOpt->width;col++)
		{
			if((DCannyImageOptMat->data.ptr + row * DCannyImageOptMat->step)[col]!=0)
			{
				if((DImageMat->data.ptr + row * DImageMat->step)[col-1]>CANNY_THRESHOLD || 
					(DImageMat->data.ptr + row * DImageMat->step)[col+1]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + row * DImageMat->step)[col-2]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + row * DImageMat->step)[col+2]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + (row+1) * DImageMat->step)[col]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + (row-1) * DImageMat->step)[col]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + (row+2) * DImageMat->step)[col]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + (row-2) * DImageMat->step)[col]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + (row+1) * DImageMat->step)[col+1]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + (row-1) * DImageMat->step)[col-1]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + (row+2) * DImageMat->step)[col+1]>CANNY_THRESHOLD ||
					(DImageMat->data.ptr + (row-2) * DImageMat->step)[col-1]>CANNY_THRESHOLD)
					(DCannyImageOptMat->data.ptr + row * DCannyImageOptMat->step)[col]=0;
				
			}
		}
	}
	cvGetImage(DCannyImageOptMat,DCannyImageOpt);

	//对处理后的图片进行点的聚类处理
	int unit_row = (DCannyImageOpt->height)/OBJECT_BLOCK;
	int unit_col = (DCannyImageOpt->width)/OBJECT_BLOCK;
	int point_num;
	int Block[OBJECT_BLOCK][OBJECT_BLOCK];
	for(int i=0;i<OBJECT_BLOCK;i++)
		for(int j=0;j<OBJECT_BLOCK;j++)
			Block[i][j]=0;

	for(int i=0;i<OBJECT_BLOCK;i++)
	{
		for(int j=0;j<OBJECT_BLOCK;j++)
		{
			point_num = 0;
			for(int row=i*unit_row;row<((i+1)*unit_row);row++)
			{
				for(int col=j*unit_col;col<((j+1)*unit_col);col++)
				{
					if((DCannyImageOptMat->data.ptr + row * DCannyImageOptMat->step)[col]!=0)
					{
						point_num++;
					}
				}
			}
			if(point_num>OBJECT_THRESHOLD)
			{
				Block[i][j] = 1;
			}
		}
	}

	//聚类完成一个物体后进行框选,定义一个矩形框的四个点
	int Max_Row,Min_Row,Max_Col,Min_Col;
	Max_Row = 0;Min_Row = 0;Max_Col = 0;Min_Col = 0;

	bool flag = true;
	Mat MatObjectImage = ObjectImage;

	for(int row=0;row<MatObjectImage.rows;row++)
	{
		for(int col=0;col<MatObjectImage.cols;col++)
		{
			MatObjectImage.at<unsigned char>(row,col*MatObjectImage.channels())=0;//B通道
			MatObjectImage.at<unsigned char>(row,col*MatObjectImage.channels()+1)=0;//G通道
			MatObjectImage.at<unsigned char>(row,col*MatObjectImage.channels()+2)=0;//R通道
		}
	}
	for(int i=0;i<OBJECT_BLOCK;i++)
	{
		for(int j=0;j<OBJECT_BLOCK;j++)
		{
			if(Block[i][j]==1)
			{
				cout<<"行块"<<i<<"列块"<<j<<endl;
				for(int row=i*unit_row;row<((i+1)*unit_row);row++)
				{
					for(int col=j*unit_col;col<((j+1)*unit_col);col++)
					{
						if((DCannyImageOptMat->data.ptr + row * DCannyImageOptMat->step)[col]!=0)
						{
							if(flag)
							{
								Max_Row = row;
								Min_Row = row;
								Max_Col = col;
								Min_Col = col;
								flag = false;
							}

							MatObjectImage.at<unsigned char>(row,col*MatObjectImage.channels())=0;//B通道
							MatObjectImage.at<unsigned char>(row,col*MatObjectImage.channels()+1)=0;//G通道
							MatObjectImage.at<unsigned char>(row,col*MatObjectImage.channels()+2)=255;//R通道

							//寻找行列边界值
							if(row<Min_Row)
								Min_Row = row;
							if(row>Max_Row)
								Max_Row = row;
							if(col<Min_Col)
								Min_Col = col;
							if(col>Max_Col)
								Max_Col = col;
						}
					}
				}
			}
		}
	}
	*ObjectImage = MatObjectImage;
	cout<<"Min_Row:"<<Min_Row<<"Max_Row:"<<Max_Row<<"Min_Col:"<<Min_Col<<"Max_Col:"<<Max_Col<<endl;

	RObjectImage = _RImageRGB;
	//偏移处理
	Min_Col = Min_Col - RECT_OFFSET;
	Max_Col = Max_Col + RECT_OFFSET;
	Min_Row = Min_Row - RECT_OFFSET;
	Max_Row = Max_Row + RECT_OFFSET;

	cvLine(RObjectImage,cvPoint(Min_Col,Min_Row),cvPoint(Max_Col,Min_Row),CV_RGB(255,0,0),2,8,0);
	cvLine(RObjectImage,cvPoint(Min_Col,Max_Row),cvPoint(Max_Col,Max_Row),CV_RGB(255,0,0),2,8,0);
	cvLine(RObjectImage,cvPoint(Min_Col,Min_Row),cvPoint(Min_Col,Max_Row),CV_RGB(255,0,0),2,8,0);
	cvLine(RObjectImage,cvPoint(Max_Col,Min_Row),cvPoint(Max_Col,Max_Row),CV_RGB(255,0,0),2,8,0);
	
	//完成框选后计算真实的距离值显示在原图片上
	Mat MatDImage = DImage;
	int Object_row = (Min_Row+Max_Row)/2;
	int Object_col = (Min_Col+Max_Col)/2;

	double Bd = BASELINE/(MatDImage.at<unsigned char>(Object_row,Object_col));
    double Object_Z = FU*Bd;
	cout<<"Object_Z"<<Object_Z<<endl;

	//写文字在图片上
	CvFont font;
	char buffer[100];
	cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX_SMALL,1,1,0.5,1,8);
	sprintf(buffer,"Object distance:%f mm", Object_Z);
	cvPutText(RObjectImage, buffer, cvPoint(10, 40), &font, CV_RGB(255,255,0));

	ftime(&endTime);
	    float time = (endTime.time-startTime.time)*1000+(endTime.millitm-startTime.millitm);
	    cout <<"分割目标"<<endl<<"time为"<<time<<"原始的计算时间为"<<(endTime.time-startTime.time)*1000+(endTime.millitm-startTime.millitm)<<"毫秒"<<endl;
	//显示图像
	cvShowImage("_DImage",DImage);
	cvShowImage("DCannyImage",DCannyImage);
	cvShowImage("LineColorImage",LineColorImage);
	cvShowImage("DCannyImageB",DCannyImageB);
	cvShowImage("DCannyImageOpt",DCannyImageOpt);
	cvShowImage("ObjectImage",ObjectImage);
	cvShowImage("RObjectImage",RObjectImage);

	cvWaitKey(1);
}