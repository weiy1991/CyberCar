////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_OPENCVTEST_H
#define _Maps_OPENCVTEST_H

#pragma comment(lib,"cxcore200d")
#pragma comment(lib,"cv200d")
#pragma comment(lib,"ml200d")
#pragma comment(lib,"cvaux200d")
#pragma comment(lib,"highgui200d")


// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSOPENCVTEST : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSOPENCVTEST)
private :
	// Place here your specific methods and attributes
	
	
	MAPSIOElt* Laser_Short_In_Elt;
	MAPSIOElt* Laser_Long_In_Elt;
    MAPSIOElt* Laser_Front_In_Elt;

	MAPSIOElt* VELOCITY_In_Elt;
	MAPSIOElt* ROAD_CURB_In_Elt;
	int v_left;
	int u_left;
	int v_right;
	int u_right;
    MAPSIOElt* edge_info;
	MAPSIOElt* edge_position;
	
	MAPSTimestamp m_nextTimestamp;
	MAPSTimestamp m_nextTimestamp1;
	bool m_firsttime_image;
	MAPSIOElt* Map_Image_Out_Elt;
	IplImage* Map_Image;
	//IplImage* Map_Photo;
	unsigned char* ib;
	unsigned char* ob;

	int short_x_movement;
	int short_y_movement;
	int long_x_movement;
	int long_y_movement;
	int front_x_movement;
	int front_y_movement;

	int laser_front_display;
	int SPEEDLIMITDISPLAY;

    MAPSInt64 laser_short_raw[240];
	MAPSInt64 laser_short_x[240];
	MAPSInt64 laser_short_y[240];
	MAPSInt64 laser_long_raw[140];
	MAPSInt64 laser_long_x[140];
	MAPSInt64 laser_long_y[140];
	MAPSInt64 laser_front_raw[240];
	MAPSInt64 laser_front_x[240];
	MAPSInt64 laser_front_y[240];

	MAPSInt64 laser_short_x_mean[48];
	MAPSInt64 laser_short_y_mean[48];
	MAPSInt64 laser_short_x_dev[48];
	MAPSInt64 laser_short_y_dev[48];
	MAPSInt64 laser_short_xy_dev[48];

	MAPSInt64 laser_long_x_mean[28];
	MAPSInt64 laser_long_y_mean[28];
	MAPSInt64 laser_long_x_dev[28];
	MAPSInt64 laser_long_y_dev[28];
	MAPSInt64 laser_long_xy_dev[28];

	MAPSInt64 laser_front_raw_mean[48];
	MAPSInt64 laser_front_x_mean[48];
	MAPSInt64 laser_front_y_mean[48];

	int road_short[48];
	int road_long[28];

	int laser_short_pitch;
	int laser_long_pitch;
	double laser_short_pitch_cos;
	double laser_long_pitch_cos;

	int laser_short_y_mean_th;
	int laser_short_y_dev_th;
	int laser_short_edge_th;
	int laser_long_y_mean_th;
	int laser_long_y_dev_th;
	int laser_long_edge_th;
	int laser_short_right_position;
	int laser_short_left_position;
	int laser_long_right_position;
	int laser_long_left_position;
	
	int img_scale;
	int img_x_temp;
	int img_y_temp;
	char buffer[100];

	bool laser_short_right_flag;
	bool laser_short_left_flag;
	bool laser_long_right_flag;
	bool laser_long_left_flag;
    int laser_short_right_max;
	int laser_short_left_max;
	int laser_long_right_max;
	int laser_long_left_max;

	int laser_short_edge_right;
	int laser_short_edge_left;
	int laser_long_edge_right;
	int laser_long_edge_left;

	
	int laser_short_edge_right_former;
	int laser_short_edge_left_former;
	int laser_long_edge_right_former;
	int laser_long_edge_left_former;

	int laser_short_edge_right_filter;
	int laser_short_edge_left_filter;
	int laser_long_edge_right_filter;
	int laser_long_edge_left_filter;

	int short_right_y;
	int short_left_y;
	int long_right_y;
	int long_left_y;

	int y_min;


	int j;

	int warning_x;
	int warning_y;
	int emergency;
	int obstacle;
	int road_curb_left;
	int road_curb_right;

	//路号定义
	int short_r_num;
	int short_l_num;
	int long_r_num;
	int long_l_num;
	//第二套方案的定义
	int shortrx;
	int shortry;
	int shortlx;
	int shortly;
	int longrx;
	int longry;
	int longlx;
	int longly;

	double velocity[5];

	double kg;
    
	CvFont font1;

	char buffer1[100];

	int warning_y_distance;
	struct LIDAR_EDGE_POINT	//仅短激光雷达的point
	{
		int short_left;
		int short_right;
	}lidar_edge_pos_stru;
	int lidar_l[5];
	int lidar_r[5];
	int sensor_state[3];
	int current_time;
	int last_time;
	int emergency_sensor;

};

#endif
