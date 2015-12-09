////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_CyberCurb_H
#define _Maps_CyberCurb_H


//����OpenCV����ؿ⺯�����ļ���
/*#pragma comment(lib,"cxcore200d")
#pragma comment(lib,"cv200d")
#pragma comment(lib,"ml200d")
#pragma comment(lib,"cvaux200d")
#pragma comment(lib,"highgui200d")
*/
// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSCyberCurb : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSCyberCurb)
private :
	// Place here your specific methods and attributes
	//MAPS I/O
	MAPSIOElt* Laser_Short_In_Elt;//��ǰհ�״������Ԫ��
	MAPSIOElt* Laser_Long_In_Elt;//��ǰհ�״������Ԫ��
	MAPSIOElt* MAP_Image_Out_Elt;//x

	IplImage* MAP_Image;//��ͼ�ı���ָ��

	//IplImage* Map_Photo;
	unsigned char* ib;
	unsigned char* ob;

	//Sensor State
	bool Sensor_State[2];

	//Road�Ƿ���·���ϵĵ�
	int Road_Short[48];
	int Road_Long[48];

	//Raw data
	MAPSInt64 Laser_Short_Raw[240];//ɨ��180�ȵķ�Χ������һ��360����
	MAPSInt64 Laser_Short_x[240];
	MAPSInt64 Laser_Short_y[240];
	MAPSInt64 Laser_Long_Raw[240];
	MAPSInt64 Laser_Long_x[240];
	MAPSInt64 Laser_Long_y[240];

	//ͳ��ѧ���ݶ���
	MAPSInt64 Laser_Short_x_mean[48];
	MAPSInt64 Laser_Short_y_mean[48];
	MAPSInt64 Laser_Short_x_dev[48];
	MAPSInt64 Laser_Short_y_dev[48];
	MAPSInt64 Laser_Short_xy_dev[48];

	MAPSInt64 Laser_Long_x_mean[48];
	MAPSInt64 Laser_Long_y_mean[48];
	MAPSInt64 Laser_Long_x_dev[48];
	MAPSInt64 Laser_Long_y_dev[48];
	MAPSInt64 Laser_Long_xy_dev[48];
	
	//�����··��
	int Laser_Short_Left_max;
	int Laser_Short_Left_position;
	int Laser_Short_Right_max;
	int Laser_Short_Right_position;
	int Laser_Long_Left_max;
	int Laser_Long_Left_position;
	int Laser_Long_Right_max;
	int Laser_Long_Right_position;
	//·��x�������ꡢy��������
	int Laser_Short_edge_right_x;
	int Laser_Short_edge_left_x;
	int Laser_Long_edge_right_x;
	int Laser_Long_edge_left_x;
	int Laser_Short_edge_right_y;
	int Laser_Short_edge_left_y;
	int Laser_Long_edge_right_y;
	int Laser_Long_edge_left_y;
	//ǰ��ֵ
	int Laser_Short_edge_right_x_former;
	int Laser_Short_edge_left_x_former;
	int Laser_Long_edge_right_x_former;
	int Laser_Long_edge_left_x_former;
	//�˲�ֵ
	int Laser_Short_edge_right_x_filter;
	int Laser_Long_edge_right_x_filter;
	int Laser_Short_edge_left_x_filter;
	int Laser_Long_edge_left_x_filter;
	//·�ر�־
	bool Laser_Short_Left_flag;
	bool Laser_Long_Left_flag;
	bool Laser_Short_Right_flag;
	bool Laser_Long_Right_flag;

	int Laser_Short_Pitch;
	int Laser_Long_Pitch;

	int Laser_Short_y_mean_th;
	int Laser_Short_y_dev_th;
	int Laser_Short_edge_th;

	int Laser_Long_y_mean_th;
	int Laser_Long_y_dev_th;
	int Laser_Long_edge_th;

	int Image_Scale;
	int x_movement;
	int x_min;
    int image_x_temp;//ͼ��x��y�������ʱ���ֵ
	int image_y_temp;
	double kg;
	bool Firsttime_image;
	//����MAPʱ��
	MAPSTimestamp m_nextTimestamp1;
	MAPSTimestamp m_nextTimestamp2;
	//��Ե��Ϣ��λ��
	MAPSIOElt* edge_info;
	MAPSIOElt* edge_position;
	//��������ͻ���
	CvFont font1;
	char C_buffer[100];
	//��ǰհ�����״��point
	struct LIDAR_EDGE_POINT	
	{
		int short_left;
		int short_right;
	}lidar_edge_pos_stru;
	int lidar_left[5];
	int lidar_right[5];
	int k;
};

#endif
