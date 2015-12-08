// CyberC3Master.cpp : �������̨Ӧ�ó������ڵ㡣
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
/*              BMP ����                                                    */
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
	unsigned short int		bfType;         // λͼ�ļ������ͣ�����Ϊ BM
	unsigned int			bfSize;       	// �ļ���С�����ֽ�Ϊ��λ
	unsigned short int		bfReserverd1; 	// λͼ�ļ������֣�����Ϊ0
	unsigned short int		bfReserverd2; 	// λͼ�ļ������֣�����Ϊ0
	unsigned int 			bfbfOffBits;  	// λͼ�ļ�ͷ�����ݵ�ƫ���������ֽ�Ϊ��λ
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	int 		biSize;					    // �ýṹ��С���ֽ�Ϊ��λ
	int 		biWidth;				    // ͼ�ο��������Ϊ��λ
	int 		biHeight;				    // ͼ�θ߶�������Ϊ��λ
	short int 	biPlanes;				    // Ŀ���豸�ļ��𣬱���Ϊ1
	short int 	biBitcount;				    // ��ɫ��ȣ�ÿ����������Ҫ��λ��
	int 		biCompression;			    // λͼ��ѹ������
	int 		biSizeImage;			    // λͼ�Ĵ�С�����ֽ�Ϊ��λ
	int 		biXPelsPermeter;		    // λͼˮƽ�ֱ��ʣ�ÿ��������
	int 		biYPelsPermeter;		    // λͼ��ֱ�ֱ��ʣ�ÿ��������
	int 		biClrUsed;				    // λͼʵ��ʹ�õ���ɫ���е���ɫ��
	int 		biClrImportant;			    // λͼ��ʾ��������Ҫ����ɫ��
}BITMAPINFOHEADER;

typedef struct
{
	BITMAPFILEHEADER file;                  // �ļ���Ϣ��
	BITMAPINFOHEADER info;                  // ͼ����Ϣ��
	RGBQUAD *pColorTable;                   // ��ɫ��
	unsigned char *imgBuf;					// λͼ����
}bmp;
/************************************BMP ����**************************************/
/****************************************************************************/
/*                                                                          */
/*              ȫ�ֱ���                                                    */
/*                                                                          */
/****************************************************************************/
unsigned char *newBmpBuf;
unsigned char newpColorTable[256 * 4];

/****************************************************************************/
/*                                                                          */
/*              ��������                                                    */
/*                                                                          */
/****************************************************************************/
bmp ReadBmp(char *bmpName);				// ��ȡ bmp ͼƬ
int SaveBmp(char *bmpName, bmp *m);		// ���� bmp ͼƬ
bmp Canny(bmp *m, char *mask_ptr);		//��Ե���
char* SerialNumberRecognition(bmp *m); 	// ����ҹ��ֺ�ʶ��
bmp ImagePreProcess(bmp srcImage);//ͼ��Ԥ����
bmp ImageBinarization(bmp srcImage, int threshold);//��ֵ��ͼ��
bmp ImageNormalization(bmp srcImage, bmp Template);//�ַ���һ��
float ImageMatch(bmp srcImage, bmp Template);//ģ��ƥ���ʼ���
int MaxMatchValue(float matchvalue[], int n);//Ѱ������ƥ��ֵ
void ImageShowValue(bmp srcImage);//��ʾͼ�������ֵ


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
	//���浽һ����ά������



	//���й��ֺ������ȷ��
	//���й��ֺŵ�ƽ������
	//���й��ֺŵ��ַ��ָ���ֱ𱣴�
	//Ȼ�������֮ǰд�Ľ���ʶ��Ľ׶�

	//���洦����ͼƬ

	//printf("cout:%d,  %d,  %d", p[testImage.imgBuf[0]].rgbBlue, p[testImage.imgBuf[0]].rgbGreen, p[testImage.imgBuf[0]].rgbRed);

	SaveBmp("../Image/processImage/Out10.bmp", &testImage);

	printf("Successfully save the BMP image!");
	
	system("PAUSE");

	return 0;
}

/****************************************************************************/
/*                                                                          */
/*              ��ȡ bmp ͼƬ                                               */
/*                                                                          */
/****************************************************************************/
bmp ReadBmp(char *bmpName)
{
	bmp m;			// ����һ��λͼ�ṹ
	FILE *fp;
	unsigned char *BmpBuf;
	unsigned char *a;
	int i;
	int bmpWidth;	// ͼ��Ŀ�
	int bmpHeight;	// ͼ��ĸ�
	int biBitCount;	// ͼ�����ͣ�ÿ����λ��
	int lineByte;

	if ((fp = fopen(bmpName, "rb")) == NULL)
	{
		printf("can't open the bmp imgae.\n ");
		exit(0);
	}
	else
	{
		fread(&m.file.bfType, sizeof(char), 2, fp);				// ����
		fread(&m.file.bfSize, sizeof(int), 1, fp);				    // �ļ�����
		fread(&m.file.bfReserverd1, sizeof(short int), 1, fp);	// ������ 1
		fread(&m.file.bfReserverd2, sizeof(short int), 1, fp);	// ������ 2
		fread(&m.file.bfbfOffBits, sizeof(int), 1, fp);			// ƫ����
		fread(&m.info.biSize, sizeof(int), 1, fp);				    // �˽ṹ��С
		fread(&m.info.biWidth, sizeof(int), 1, fp);				// λͼ�Ŀ��
		fread(&m.info.biHeight, sizeof(int), 1, fp);			    // λͼ�ĸ߶�
		fread(&m.info.biPlanes, sizeof(short), 1, fp);			    // Ŀ���豸λͼ��
		fread(&m.info.biBitcount, sizeof(short), 1, fp);		    // ��ɫ���
		fread(&m.info.biCompression, sizeof(int), 1, fp);		    // λͼѹ������
		fread(&m.info.biSizeImage, sizeof(int), 1, fp);			// λͼ��С
		fread(&m.info.biXPelsPermeter, sizeof(int), 1, fp);		// λͼˮƽ�ֱ���
		fread(&m.info.biYPelsPermeter, sizeof(int), 1, fp);		// λͼ��ֱ�ֱ���
		fread(&m.info.biClrUsed, sizeof(int), 1, fp);			    // λͼʵ��ʹ����ɫ��
		fread(&m.info.biClrImportant, sizeof(int), 1, fp);		    // λͼ��ʾ�бȽ���Ҫ��ɫ��
	}

	// ��ȡͼ����ߡ�ÿ������ռλ������Ϣ
	bmpWidth = m.info.biWidth;
	bmpHeight = m.info.biHeight;
	biBitCount = m.info.biBitcount;

	printf("%d,%d,%d",bmpHeight,bmpWidth,biBitCount);

	// �������������ͼ��ÿ��������ռ���ֽ�����������4�ı�����
	lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;

	// �Ҷ�ͼ������ɫ������ɫ�����Ϊ256
	if (biBitCount == 8)
	{
		// �����ڴ�ռ�
		m.pColorTable = (RGBQUAD *)malloc(256 * 4);
		// ������ɫ������Ҫ�Ŀռ䣬����ɫ����ڴ�
		fread(m.pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	// �����ڴ�ռ�
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


	// ��λͼ����
	fread(BmpBuf, lineByte * bmpHeight, 1, fp);

	// �����������ݴ�ֱ����ת
	for (i = 0; i < bmpHeight; i++)
		memcpy((void *)(m.imgBuf + lineByte * i),
		(void *)(BmpBuf + lineByte * (bmpHeight - i - 1)), lineByte);

	
	
	// �ر��ļ�
	fclose(fp);

	return m;
}

/****************************************************************************/
/*                                                                          */
/*              ���� bmp ͼƬ                                               */
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

	// ���λͼ����ָ��Ϊ0����û�����ݴ��룬��������
	if (!m->imgBuf)
		return 0;

	// ���洢ͼ������ÿ���ֽ���Ϊ4�ı���
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;
	//�Զ�����д�ķ�ʽ���ļ�
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

	// ����Ҷ�ͼ������ɫ��д���ļ�
	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD)* 256, 1, fp);

	/*for (i = 0; i < lineByte * bmpHeight; m->imgBuf + 4, i++)
	{
		
			*(m->imgBuf) = 0;
			*(m->imgBuf + 1) = 0;
			*(m->imgBuf + 2) = 0;
		
	}*/

	// �������ݻ���
	BmpBuf = (unsigned char *)malloc(m->info.biSizeImage);
	// ��λͼ���ݴ�ֱ����ת��д��
	for (i = 0; i < bmpHeight; i++)
		memcpy((void *)(BmpBuf + lineByte * i),
		(void *)(m->imgBuf + lineByte * (bmpHeight - i - 1)), lineByte);

	// дλͼ���ݽ��ļ�
	fwrite(BmpBuf, bmpHeight*lineByte, 1, fp);

	// �ر��ļ�
	fclose(fp);

	return 1;
}

/****************************************************************************/
/*                                                                          */
/*               ���ֺ�ʶ��                                                 */
/*                                                                          */
/****************************************************************************/
char* SerialNumberRecognition(bmp *m)
{
	int i;
	bmp newm;
	int bmpWidth;		// ͼ��Ŀ�
	int bmpHeight;		// ͼ��ĸ�
	int biBitCount;		// ͼ�����ͣ�ÿ����λ��

	int newBmpWidth; 	// ��ͼ��Ŀ�
	int newBmpHeight; 	// ��ͼ��ĸ�
	int newLineByte;

	// ��ȡͼ����ߡ�ÿ������ռλ������Ϣ
	bmpWidth = m->info.biWidth;
	bmpHeight = m->info.biHeight;
	biBitCount = m->info.biBitcount;

	// ��ͼ��Ŀ��
	newBmpWidth = bmpWidth;
	newBmpHeight = bmpHeight;
	newLineByte = (newBmpWidth * biBitCount / 8 + 3) / 4 * 4;
	// ����λͼ������
	newBmpBuf = (unsigned char *)malloc(newLineByte * newBmpHeight);

	// ������
	int pixelNums;
	// ͼ��������
	unsigned char* imageData;
	// ������
	unsigned char* scratchData;
	// x�����ݶ�
	short* gradX;
	// y�����ݶ�
	short* gradY;
	// �ݶȷ�ֵ
	short* gradM;
	// �Ǽ���ֵ������
	unsigned char* nonMax;
	// �¼���ı���
	int* numItems;
	// ��Ե����
	unsigned int* listptr;
	// ����ֵ
	unsigned char hi = 20;
	// ����ֵ
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

	// ��˹�˲�
	pixelNums = (bmpHeight - 6)*bmpWidth - 6;
	

	// �� nonMax ��ĩ5������
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

	newm.file.bfType = 0x4d42;							// ����
	newm.file.bfSize = 54 + 246 * 4 +
		newLineByte * newBmpHeight;		// �ļ�����
	newm.file.bfReserverd1 = 0;							// ������ 1
	newm.file.bfReserverd2 = 0;							// ������ 2
	newm.file.bfbfOffBits = 54 + 256 * 4;				// ƫ����
	newm.info.biSize = 40;								// �˽ṹ��С
	newm.info.biWidth = newBmpWidth;					// λͼ�Ŀ��
	newm.info.biHeight = newBmpHeight;					// λͼ�ĸ߶�
	newm.info.biPlanes = 1;								// Ŀ���豸λͼ��
	newm.info.biBitcount = 8;							// ��ɫ���
	newm.info.biCompression = 0;						// λͼѹ������
	newm.info.biSizeImage = newLineByte * newBmpHeight;	// λͼ��С
	newm.info.biXPelsPermeter = newBmpWidth;			// λͼˮƽ�ֱ���
	newm.info.biYPelsPermeter = newBmpHeight;			// λͼ��ֱ�ֱ���
	newm.info.biClrUsed = 256;							// λͼʵ��ʹ����ɫ��
	newm.info.biClrImportant = 256;						// λͼ��ʾ�бȽ���Ҫ��ɫ��

	// д���ɫ��
	memcpy((void *)(newpColorTable), (void *)(m->pColorTable), 256 * 4);
	newm.pColorTable = (RGBQUAD*)newpColorTable;

	// д��λͼ����
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
	//�������
	IplImage *g_pGrayImage = NULL;
	IplImage *g_pBinaryImage = NULL;

	// ���ļ��м���ԭͼ  
	IplImage *pSrcImage = cvLoadImage("../image/junjian.jpg", CV_LOAD_IMAGE_UNCHANGED);

	// תΪ�Ҷ�ͼ  
	g_pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);

	// ������ֵͼ  
	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	cvThreshold(g_pGrayImage, g_pBinaryImage, 130, 255, CV_THRESH_BINARY);

	cvSaveImage("../image/junjianerzhi.jpg", g_pBinaryImage);
}