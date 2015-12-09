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
	int laser_short_y_th_max;//y的值范围
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
	int road_th_s_e_x;//路缘和路面之间的x差值阈值
	float road_th_surface_coef;//道路表面斜率阈值
	int road_th_edge_coef;//路沿斜率阈值
	int road_th_surface_edge_x;//路沿线和路面线的x距离
	//左右路沿标志
	bool short_right;
	bool short_left;
	bool long_right;
	bool long_left;
	//道路模型
	int road_type_short;
	int road_type_long;
	//路缘坐标值
	int short_edge_right[2];
	int short_edge_left[2];
	int long_edge_right[2];
	int long_edge_left[2];

	//最小二乘法所需要的一些系数数组
	int m_x[100];
	int m_y[100];
	double m_dCoefFit[6];
	int m_nCheckPoints;
	int m_nFitNum;
	double dError[4];
	//存储直线的两个系数
	double short_fit_coef[100][2];//两个系数值
	double long_fit_coef[100][2];
	double short_fit_next_coef[100][2];//两个后期处理系数值
	double long_fit_next_coef[100][2];
	int short_fit_type[100];//拟合曲线的类型
	int long_fit_type[100];
	double short_fit_error[100];//保存误差值
	double long_fit_error[100];
	int line_num_short;//保存直线数
	int line_num_long;
	int line_num_short_next;//保存聚类区直线数
	int line_num_long_next;
	int line_num_short_combine;//保存合并的直线数
	int line_num_long_combine;
	int line_short_se[100][2];//保存各条直线的起始点号和结束点号
	int line_long_se[100][2];
	int line_short_next_se[100][2];//保存各条后处理直线的起始点号和结束点号
	int line_long_next_se[100][2];
	int line_short_combine[100][2];//合并的直线号
	int line_long_combine[100][2];
	//创建图像参数
	bool firsttime_image;
	MAPSTimestamp m_nextTimestamp;
	IplImage* Map_Image;
	MAPSIOElt* Map_Image_Out;
	int temp_x;
	int temp_y;
	int image_scale;
	unsigned char* ib;
	unsigned char* ob;
	//报警境界参数(为1时可用，为0时点数太少，报警，当两者均报警时，表示警告无人驾驶状态不安全)
	int run_count;//core函数的运行计数次数
	int short_available;
	int short_available_count;
	int long_available;
	int long_available_count;


public:
	//通用函数
	void FitCurveCompute(double *x,double *y,double *a,int num,int n,double *err);
	int Average(int* x,int n);
    //专有函数
	double Slope(int* start,int* stop);
	// Place here your specific methods and attributes
};

#endif
