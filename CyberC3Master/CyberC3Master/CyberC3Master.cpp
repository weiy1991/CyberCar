// CyberC3Master.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

void erzhihuatuxiang();


float matchRate[4];
int imageGray[300][400];
int _imageGray[120000];

char Template[4] = { 'D', 'P', '7', '0' };

/****************************************************************************/
/*                                                                          */
/*              BMP 定义                                                    */
/*                                                                          */
/****************************************************************************/
typedef struct tagRGBQUAD
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
}RGBQUAD;

typedef struct tagGRAY
{
	unsigned char value;
}GRAY;

typedef struct tagBITMAPFILEHEADER
{
	unsigned short int		bfType;         // 位图文件的类型，必须为 BM
	unsigned int			bfSize;       	// 文件大小，以字节为单位
	unsigned short int		bfReserverd1; 	// 位图文件保留字，必须为0
	unsigned short int		bfReserverd2; 	// 位图文件保留字，必须为0
	unsigned int 			bfbfOffBits;  	// 位图文件头到数据的偏移量，以字节为单位
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	int 		biSize;					    // 该结构大小，字节为单位
	int 		biWidth;				    // 图形宽度以象素为单位
	int 		biHeight;				    // 图形高度以象素为单位
	short int 	biPlanes;				    // 目标设备的级别，必须为1
	short int 	biBitcount;				    // 颜色深度，每个象素所需要的位数
	int 		biCompression;			    // 位图的压缩类型
	int 		biSizeImage;			    // 位图的大小，以字节为单位
	int 		biXPelsPermeter;		    // 位图水平分辨率，每米像素数
	int 		biYPelsPermeter;		    // 位图垂直分辨率，每米像素数
	int 		biClrUsed;				    // 位图实际使用的颜色表中的颜色数
	int 		biClrImportant;			    // 位图显示过程中重要的颜色数
}BITMAPINFOHEADER;

typedef struct
{
	BITMAPFILEHEADER file;                  // 文件信息区
	BITMAPINFOHEADER info;                  // 图象信息区
	RGBQUAD *pColorTable;                   // 调色板
	unsigned char *imgBuf;					// 位图数据
}bmp;
/************************************BMP 定义**************************************/
/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
unsigned char *newBmpBuf;
unsigned char newpColorTable[256 * 4];

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
bmp ReadBmp(char *bmpName);				// 读取 bmp 图片
int SaveBmp(char *bmpName, bmp *m);		// 保存 bmp 图片
bmp Canny(bmp *m, char *mask_ptr);		//边缘检测
char* SerialNumberRecognition(bmp *m); 	// 人民币冠字号识别
bmp ImagePreProcess(bmp srcImage);//图像预处理
bmp ImageBinarization(bmp srcImage, int threshold);//二值化图像
bmp ImageNormalization(bmp srcImage, bmp Template);//字符归一化
float ImageMatch(bmp srcImage, bmp Template);//模板匹配率计算
int MaxMatchValue(float matchvalue[], int n);//寻找最大的匹配值
void ImageShowValue(bmp srcImage);//显示图像的像素值


int _tmain(int argc, _TCHAR* argv[])
{

	bmp testImage;
	bmp templateImage[4];


	testImage = ReadBmp("../image/testImage/TestMoney.bmp");
	//templateImage[0] = ReadBmp("../image/templateImage/D_Template.bmp");
	//templateImage[1] = ReadBmp("../image/templateImage/P_Template.bmp");
	//templateImage[2] = ReadBmp("../image/templateImage/7_Template.bmp");
	//templateImage[3] = ReadBmp("../image/templateImage/0_Template.bmp");

	printf("Successfully get the BMP image!");

	//printf("size:%d", sizeof(testImage.imgBuf));
	int cout = 0;
	/*while ((testImage.imgBuf))
	{
		cout++;
		testImage.imgBuf++;
	}*/

	printf("cout:%d",testImage.info.biSizeImage);

	RGBQUAD* p = (RGBQUAD*)testImage.pColorTable;


	int j = 0;
	for (int i = 0; i < testImage.info.biSizeImage; i++)
	{
		//p[testImage.imgBuf[i++]];
		p = (RGBQUAD*)testImage.pColorTable + testImage.imgBuf[i];
		_imageGray[i] = 0.3*p->rgbRed + 0.59*p->rgbGreen + 0.11*p->rgbBlue;
		imageGray[i/300][j] = 0.3*p->rgbRed + 0.59*p->rgbGreen + 0.11*p->rgbBlue;
	}

	for (int i = 0; i < 120000; i++)
	{
		//printf("ImageGray[%d]:%d",i,_imageGray[i]);
	}
	//保存到一个二维数组里



	//进行冠字号区域的确定
	//进行冠字号的平滑处理
	//进行冠字号的字符分割，并分别保存
	//然后就是我之前写的进入识别的阶段

	//保存处理后的图片

	//printf("cout:%d,  %d,  %d", p[testImage.imgBuf[0]].rgbBlue, p[testImage.imgBuf[0]].rgbGreen, p[testImage.imgBuf[0]].rgbRed);

	SaveBmp("../Image/processImage/Out10.bmp", &testImage);

	printf("Successfully save the BMP image!");
	
	system("PAUSE");

	return 0;
}

/****************************************************************************/
/*                                                                          */
/*              读取 bmp 图片                                               */
/*                                                                          */
/****************************************************************************/
bmp ReadBmp(char *bmpName)
{
	bmp m;			// 定义一个位图结构
	FILE *fp;
	unsigned char *BmpBuf;
	unsigned char *a;
	int i;
	int bmpWidth;	// 图像的宽
	int bmpHeight;	// 图像的高
	int biBitCount;	// 图像类型，每像素位数
	int lineByte;

	if ((fp = fopen(bmpName, "rb")) == NULL)
	{
		printf("can't open the bmp imgae.\n ");
		exit(0);
	}
	else
	{
		fread(&m.file.bfType, sizeof(char), 2, fp);				// 类型
		fread(&m.file.bfSize, sizeof(int), 1, fp);				    // 文件长度
		fread(&m.file.bfReserverd1, sizeof(short int), 1, fp);	// 保留字 1
		fread(&m.file.bfReserverd2, sizeof(short int), 1, fp);	// 保留字 2
		fread(&m.file.bfbfOffBits, sizeof(int), 1, fp);			// 偏移量
		fread(&m.info.biSize, sizeof(int), 1, fp);				    // 此结构大小
		fread(&m.info.biWidth, sizeof(int), 1, fp);				// 位图的宽度
		fread(&m.info.biHeight, sizeof(int), 1, fp);			    // 位图的高度
		fread(&m.info.biPlanes, sizeof(short), 1, fp);			    // 目标设备位图数
		fread(&m.info.biBitcount, sizeof(short), 1, fp);		    // 颜色深度
		fread(&m.info.biCompression, sizeof(int), 1, fp);		    // 位图压缩类型
		fread(&m.info.biSizeImage, sizeof(int), 1, fp);			// 位图大小
		fread(&m.info.biXPelsPermeter, sizeof(int), 1, fp);		// 位图水平分辨率
		fread(&m.info.biYPelsPermeter, sizeof(int), 1, fp);		// 位图垂直分辨率
		fread(&m.info.biClrUsed, sizeof(int), 1, fp);			    // 位图实际使用颜色数
		fread(&m.info.biClrImportant, sizeof(int), 1, fp);		    // 位图显示中比较重要颜色数
	}

	// 获取图像宽、高、每像素所占位数等信息
	bmpWidth = m.info.biWidth;
	bmpHeight = m.info.biHeight;
	biBitCount = m.info.biBitcount;

	printf("%d,%d,%d",bmpHeight,bmpWidth,biBitCount);

	// 定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;

	// 灰度图像有颜色表，且颜色表表项为256
	if (biBitCount == 8)
	{
		// 分配内存空间
		m.pColorTable = (RGBQUAD *)malloc(256 * 4);
		// 申请颜色表所需要的空间，读颜色表进内存
		fread(m.pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	// 分配内存空间
	m.imgBuf = (unsigned char *)malloc(m.info.biSizeImage);
	BmpBuf = (unsigned char *)malloc(m.info.biSizeImage);

	/*a = (unsigned char *)malloc(bmpHeight*lineByte);
	for (int i = 0; i <  bmpHeight; i++)
	{
		for (int j = 0; j < lineByte; j++)
		{
			fread(a + j, 1, 1, fp);
		}
		printf("TT:%d", a);

	}
	printf("XT:%d", a);*/
	//for (int i = 0; i < lineByte/4; i++)
	//{
	//	/*if (*(a+i*4)>50)
	//	{*/
	//		*(a+i*4) = 0;
	//		*(a+i*4+1) = 255;
	//		*(a+i*4+2) = 255;
	//		*(a + i * 4 + 3) = 0;
	//	/*}*/
	//}

	//fwrite(a, lineByte, 1, fp);


	// 读位图数据
	fread(BmpBuf, lineByte * bmpHeight, 1, fp);

	// 将读到的数据垂直镜像翻转
	for (i = 0; i < bmpHeight; i++)
		memcpy((void *)(m.imgBuf + lineByte * i),
		(void *)(BmpBuf + lineByte * (bmpHeight - i - 1)), lineByte);

	
	
	// 关闭文件
	fclose(fp);

	return m;
}

/****************************************************************************/
/*                                                                          */
/*              保存 bmp 图片                                               */
/*                                                                          */
/****************************************************************************/
int SaveBmp(char *bmpName, bmp *m)
{
	unsigned char *BmpBuf;
	int i;
	int bmpWidth;
	int bmpHeight;
	int biBitCount;
	RGBQUAD *pColorTable;

	bmpWidth = m->info.biWidth;
	bmpHeight = m->info.biHeight;
	biBitCount = m->info.biBitcount;
	pColorTable = m->pColorTable;

	// 如果位图数据指针为0，则没有数据传入，函数返回
	if (!m->imgBuf)
		return 0;

	// 待存储图像数据每行字节数为4的倍数
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;
	//以二进制写的方式打开文件
	FILE *fp = fopen(bmpName, "wb");
	if (fp == 0)
		return 0;

	fwrite(&m->file.bfType, sizeof(short), 1, fp);
	fwrite(&m->file.bfSize, sizeof(int), 1, fp);
	fwrite(&m->file.bfReserverd1, sizeof(short int), 1, fp);
	fwrite(&m->file.bfReserverd2, sizeof(short int), 1, fp);
	fwrite(&m->file.bfbfOffBits, sizeof(int), 1, fp);
	fwrite(&m->info.biSize, sizeof(int), 1, fp);
	fwrite(&m->info.biWidth, sizeof(int), 1, fp);
	fwrite(&m->info.biHeight, sizeof(int), 1, fp);
	fwrite(&m->info.biPlanes, sizeof(short), 1, fp);
	fwrite(&m->info.biBitcount, sizeof(short), 1, fp);
	fwrite(&m->info.biCompression, sizeof(int), 1, fp);
	fwrite(&m->info.biSizeImage, sizeof(int), 1, fp);
	fwrite(&m->info.biXPelsPermeter, sizeof(int), 1, fp);
	fwrite(&m->info.biYPelsPermeter, sizeof(int), 1, fp);
	fwrite(&m->info.biClrUsed, sizeof(int), 1, fp);
	fwrite(&m->info.biClrImportant, sizeof(int), 1, fp);

	// 如果灰度图像，有颜色表，写入文件
	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD)* 256, 1, fp);

	/*for (i = 0; i < lineByte * bmpHeight; m->imgBuf + 4, i++)
	{
		
			*(m->imgBuf) = 0;
			*(m->imgBuf + 1) = 0;
			*(m->imgBuf + 2) = 0;
		
	}*/

	// 分配数据缓冲
	BmpBuf = (unsigned char *)malloc(m->info.biSizeImage);
	// 将位图数据垂直镜像翻转再写入
	for (i = 0; i < bmpHeight; i++)
		memcpy((void *)(BmpBuf + lineByte * i),
		(void *)(m->imgBuf + lineByte * (bmpHeight - i - 1)), lineByte);

	// 写位图数据进文件
	fwrite(BmpBuf, bmpHeight*lineByte, 1, fp);

	// 关闭文件
	fclose(fp);

	return 1;
}

/****************************************************************************/
/*                                                                          */
/*               冠字号识别                                                 */
/*                                                                          */
/****************************************************************************/
char* SerialNumberRecognition(bmp *m)
{
	int i;
	bmp newm;
	int bmpWidth;		// 图像的宽
	int bmpHeight;		// 图像的高
	int biBitCount;		// 图像类型，每像素位数

	int newBmpWidth; 	// 新图像的宽
	int newBmpHeight; 	// 新图像的高
	int newLineByte;

	// 获取图像宽、高、每像素所占位数等信息
	bmpWidth = m->info.biWidth;
	bmpHeight = m->info.biHeight;
	biBitCount = m->info.biBitcount;

	// 新图像的宽高
	newBmpWidth = bmpWidth;
	newBmpHeight = bmpHeight;
	newLineByte = (newBmpWidth * biBitCount / 8 + 3) / 4 * 4;
	// 申请位图数据区
	newBmpBuf = (unsigned char *)malloc(newLineByte * newBmpHeight);

	// 像素数
	int pixelNums;
	// 图像数据区
	unsigned char* imageData;
	// 过程量
	unsigned char* scratchData;
	// x方向梯度
	short* gradX;
	// y方向梯度
	short* gradY;
	// 梯度幅值
	short* gradM;
	// 非极大值处理结果
	unsigned char* nonMax;
	// 新加入的变量
	int* numItems;
	// 边缘保存
	unsigned int* listptr;
	// 高阈值
	unsigned char hi = 20;
	// 低阈值
	unsigned char lo = 2;

	pixelNums = bmpWidth*bmpHeight;

	imageData = (unsigned char*)malloc(sizeof(unsigned char)*pixelNums);
	scratchData = (unsigned char*)malloc(sizeof(unsigned char)*pixelNums);
	gradX = (short*)malloc(sizeof(short)*pixelNums);
	gradY = (short*)malloc(sizeof(short)*pixelNums);
	gradM = (short*)malloc(sizeof(short)*pixelNums);
	nonMax = (unsigned char*)malloc(sizeof(unsigned char)*pixelNums);
	numItems = (int*)malloc(sizeof(numItems));
	listptr = (unsigned int*)malloc(sizeof(unsigned int)*pixelNums);

	imageData = m->imgBuf;

	// 高斯滤波
	pixelNums = (bmpHeight - 6)*bmpWidth - 6;
	

	// 将 nonMax 首末5行置零
	for (i = 0; i < bmpWidth * 5; i++)
	{
		nonMax[i] = 0;
	}

	for (i = 0; i < bmpWidth * 5; i++)
	{
		nonMax[bmpWidth*(bmpHeight - 5) + i] = 0;
	}

	pixelNums = bmpWidth*(bmpHeight - 10);
	

	for (i = 0; i < bmpWidth * bmpHeight; i++)
	{
		*(newBmpBuf + i) = (*(nonMax + i) >> 7) * 255;
		
	}

	newm.file.bfType = 0x4d42;							// 类型
	newm.file.bfSize = 54 + 246 * 4 +
		newLineByte * newBmpHeight;		// 文件长度
	newm.file.bfReserverd1 = 0;							// 保留字 1
	newm.file.bfReserverd2 = 0;							// 保留字 2
	newm.file.bfbfOffBits = 54 + 256 * 4;				// 偏移量
	newm.info.biSize = 40;								// 此结构大小
	newm.info.biWidth = newBmpWidth;					// 位图的宽度
	newm.info.biHeight = newBmpHeight;					// 位图的高度
	newm.info.biPlanes = 1;								// 目标设备位图数
	newm.info.biBitcount = 8;							// 颜色深度
	newm.info.biCompression = 0;						// 位图压缩类型
	newm.info.biSizeImage = newLineByte * newBmpHeight;	// 位图大小
	newm.info.biXPelsPermeter = newBmpWidth;			// 位图水平分辨率
	newm.info.biYPelsPermeter = newBmpHeight;			// 位图垂直分辨率
	newm.info.biClrUsed = 256;							// 位图实际使用颜色数
	newm.info.biClrImportant = 256;						// 位图显示中比较重要颜色数

	// 写入调色板
	memcpy((void *)(newpColorTable), (void *)(m->pColorTable), 256 * 4);
	newm.pColorTable = (RGBQUAD*)newpColorTable;

	// 写入位图数据
	newm.imgBuf = newBmpBuf;

	char *p;
	p = (char *)malloc(100);
	p = "helloworld";

	return p;
}


//bmp ImagePreProcess(bmp srcImage)
//{
//	int up, down, left, right;
//	up = 0;
//	down = 0;
//	left = 0;
//	right = 0;
//	for (int i = 0; i<srcImage.info.biHeight; i++)
//	{
//		for (int j = 0; j<srcImage.cols; j++)
//		{
//			if (srcImage.at<unsigned char>(i, j)>0)
//			{
//				up = i;
//				cout << "up:" << up << endl;
//				break;
//			}
//		}
//		if (up>0)
//			break;
//	}
//	for (int i = srcImage.rows - 1; i >= 0; i--)
//	{
//		for (int j = 0; j<srcImage.cols; j++)
//		{
//			if (srcImage.at<unsigned char>(i, j)>0)
//			{
//				down = i;
//				cout << "down:" << down << endl;
//				break;
//			}
//		}
//		if (down>0)
//			break;
//	}
//	for (int i = 0; i<srcImage.cols; i++)
//	{
//		for (int j = 0; j<srcImage.rows; j++)
//		{
//			if (srcImage.at<unsigned char>(j, i)>0)
//			{
//				left = i;
//				cout << "left:" << left << endl;
//				break;
//			}
//		}
//		if (left>0)
//			break;
//	}
//	for (int i = srcImage.cols - 1; i >= 0; i--)
//	{
//		for (int j = srcImage.rows - 1; j >= 0; j--)
//		{
//			if (srcImage.at<unsigned char>(j, i)>0)
//			{
//				right = i;
//				cout << "right:" << right << endl;
//				break;
//			}
//		}
//		if (right>0)
//			break;
//	}
//
//	Mat PreImage(down - up + 1, right - left + 1, CV_8UC1);
//
//	for (int i = 0; i<PreImage.rows; i++)
//	for (int j = 0; j<PreImage.cols; j++)
//	{
//		PreImage.at<unsigned char>(i, j) = _srcImage.at<unsigned char>(up + i, left + j);
//	}
//
//	return PreImage;
//}

//Mat ImageBinarization(Mat srcImage, int threshold)
//{
//	Mat binImage(srcImage);
//	for (int i = 0; i<binImage.rows; i++)
//	{
//		for (int j = 0; j<binImage.cols; j++)
//		{
//			if (binImage.at<unsigned char>(i, j)>threshold)
//				binImage.at<unsigned char>(i, j) = 0;
//			else
//				binImage.at<unsigned char>(i, j) = 1;
//		}
//	}
//	return binImage;
//}

//Mat ImageNormalization(Mat srcImage, Mat Template)
//{
//	Mat norImage(Template.rows, Template.cols, CV_8UC1);
//
//	for (int i = 0; i<Template.rows; i++)
//	{
//		for (int j = 0; j<Template.cols; j++)
//		{
//			int row = int(i*Template.rows / srcImage.rows);
//			int col = int(j*Template.cols / srcImage.cols);
//			while (row >= srcImage.rows)
//			{
//				row--;
//			}
//			while (col >= srcImage.cols)
//			{
//				col--;
//			}
//			norImage.at<unsigned char>(i, j) = srcImage.at<unsigned char>(row, col);
//		}
//	}
//
//	return norImage;
//}

//float ImageMatch(Mat srcImage, Mat Template)
//{
//	float rate;
//	float sum, sum_src, sum_Tem;
//	sum = 0;
//	sum_src = 0;
//	sum_Tem = 0;
//	for (int i = 0; i<Template.rows; i++)
//	{
//		for (int j = 0; j<Template.cols; j++)
//		{
//			sum_src += srcImage.at<unsigned char>(i, j);
//			sum_Tem += Template.at<unsigned char>(i, j);
//			sum += (srcImage.at<unsigned char>(i, j))*(Template.at<unsigned char>(i, j));
//		}
//	}
//	rate = 2 * sum / (sum_src + sum_Tem);
//	return rate;
//}

//int MaxMatchValue(float matchvalue[], int n)
//{
//	int MaxNum;
//	float MaxValue;
//	MaxValue = matchvalue[0];
//	MaxNum = 0;
//	for (int i = 0; i < n; i++)
//	{
//		if (matchvalue[i]>MaxValue)
//		{
//			MaxNum = i;
//			MaxValue = matchvalue[i];
//		}
//	}
//	return MaxNum;
//}

//void ImageShowValue(Mat srcImage)
//{
//	Mat showImage(srcImage);
//	for (int i = 0; i<showImage.rows; i++)
//	{
//		for (int j = 0; j<showImage.cols; j++)
//		{
//			cout << int(showImage.at<unsigned char>(i, j)) << "next";
//		}
//		cout << endl;
//	}
//}


void erzhihuatuxiang()
{
	//处理军舰
	IplImage *g_pGrayImage = NULL;
	IplImage *g_pBinaryImage = NULL;

	// 从文件中加载原图  
	IplImage *pSrcImage = cvLoadImage("../image/junjian.jpg", CV_LOAD_IMAGE_UNCHANGED);

	// 转为灰度图  
	g_pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);

	// 创建二值图  
	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	cvThreshold(g_pGrayImage, g_pBinaryImage, 130, 255, CV_THRESH_BINARY);

	cvSaveImage("../image/junjianerzhi.jpg", g_pBinaryImage);
}