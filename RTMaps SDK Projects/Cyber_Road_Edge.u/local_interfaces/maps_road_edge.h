////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////
/*
Copyright YuanWei 2014
*/
#ifndef _Maps_road_edge_H
#define _Maps_road_edge_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSroad_edge : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSroad_edge)
private :
	//Variances of the lidars
	MAPSIOElt* Laser_Short_In;
	MAPSIOElt* Laser_Long_In;
	//Variances of the Outputs
	MAPSIOElt* Road_Egde_Info;
	//Variances of the lidars
	int laser_short_pitch;
	int laser_long_pitch;
	int laser_short_x_movement;
	int laser_long_x_movement;
	int laser_short_y_movement;
	int laser_long_y_movement;
	int laser_short_h;
	int laser_long_h;
	//Variances of the raw value of the lidar
	int laser_short_raw[240];
	int laser_short_raw_break[240];
	int laser_short_x[240];
	int laser_short_y[240];
	int laser_long_raw[140];
	int laser_long_raw_break[140];
	int laser_long_x[140];
	int laser_long_y[140];
	int laser_short_ang[240];
	int laser_long_ang[240];
	//Variances of the threshold value
	int laser_short_y_th_max;//y��ֵ��Χ
	int laser_short_y_th_min;
	int laser_short_raw_th_ang_max;
	int laser_short_raw_th_ang_min;
	int laser_long_y_th_max;
	int laser_long_y_th_min;
	int laser_long_raw_th_ang_max;
	int laser_long_raw_th_ang_min;
	//Variances of the threshold value of the Clustering
	int short_dor_c0;
	int short_dor_a;
	int short_dor_b;
	int long_dor_c0;
	int long_dor_a;
	int long_dor_b;
	float short_th_rate_a;
	float long_th_rate_a;
	int short_th_rate_y;
	int long_th_rate_y;
	int short_th_fit_num;
	int long_th_fit_num;
	//Variances of the threshold value of the Road
	int road_surface_slope;
	int road_edge_slope;
	int road_th_s_e_x;//·Ե��·��֮���x��ֵ��ֵ
	float road_th_surface_coef;//��·����б����ֵ
	int road_th_edge_coef;//·��б����ֵ
	int road_th_surface_edge_x;//·���ߺ�·���ߵ�x����
	//����·�ر�־
	bool short_right;
	bool short_left;
	bool long_right;
	bool long_left;
	//��·ģ��
	int road_type_short;
	int road_type_long;
	//·Ե����ֵ
	int short_edge_right[2];
	int short_edge_left[2];
	int long_edge_right[2];
	int long_edge_left[2];

	//��С���˷�����Ҫ��һЩϵ������
	int m_x[100];
	int m_y[100];
	double m_dCoefFit[6];
	int m_nCheckPoints;
	int m_nFitNum;
	double dError[4];
	//�洢ֱ�ߵ�����ϵ��
	double short_fit_coef[100][2];//����ϵ��ֵ
	double long_fit_coef[100][2];
	double short_fit_next_coef[100][2];//�������ڴ���ϵ��ֵ
	double long_fit_next_coef[100][2];
	int short_fit_type[100];//������ߵ�����
	int long_fit_type[100];
	double short_fit_error[100];//�������ֵ
	double long_fit_error[100];
	int line_num_short;//����ֱ����
	int line_num_long;
	int line_num_short_next;//���������ֱ����
	int line_num_long_next;
	int line_num_short_combine;//����ϲ���ֱ����
	int line_num_long_combine;
	int line_short_se[100][2];//�������ֱ�ߵ���ʼ��źͽ������
	int line_long_se[100][2];
	int line_short_next_se[100][2];//�����������ֱ�ߵ���ʼ��źͽ������
	int line_long_next_se[100][2];
	int line_short_combine[100][2];//�ϲ���ֱ�ߺ�
	int line_long_combine[100][2];
	//����ͼ�����
	bool firsttime_image;
	MAPSTimestamp m_nextTimestamp;
	IplImage* Map_Image;
	MAPSIOElt* Map_Image_Out;
	int temp_x;
	int temp_y;
	int image_scale;
	unsigned char* ib;
	unsigned char* ob;
	//�����������(Ϊ1ʱ���ã�Ϊ0ʱ����̫�٣������������߾�����ʱ����ʾ�������˼�ʻ״̬����ȫ)
	int run_count;//core���������м�������
	int short_available;
	int short_available_count;
	int long_available;
	int long_available_count;


public:
	//ͨ�ú���
	void FitCurveCompute(double *x,double *y,double *a,int num,int n,double *err);
	int Average(int* x,int n);
    //ר�к���
	double Slope(int* start,int* stop);
	// Place here your specific methods and attributes
};

#endif
