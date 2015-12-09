////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////
//包含OpenCV头文件
//#include "cv.h"
//#include "highgui.h"
#include"opencv2\opencv.hpp"

#include "maps_CyberCurb.h"	// Includes the header of this component
#include "cmath"

#define PI 3.1415926535897932384626433832795
#define image_width 600
#define image_length 600
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSCyberCurb)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("Laser_Short",MAPS::FilterInteger,MAPS::LastOrNextReader)//Long Laser
    MAPS_INPUT("Laser_Long",MAPS::FilterInteger,MAPS::LastOrNextReader)//Short Laser
 //   MAPS_INPUT("",MAPS::FilterInteger32,MAPS::LastOrNextReader)
   // MAPS_INPUT("",MAPS::FilterInteger32,MAPS::LastOrNextReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSCyberCurb)
    //MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
	MAPS_OUTPUT("Map_Image_Out",MAPS::IplImage,NULL,NULL,1)//Image out 
	MAPS_OUTPUT("Road_Curb",MAPS::Integer,NULL,NULL,8)//Curb out
	MAPS_OUTPUT("Road_Edge_Position",MAPS::Integer,NULL,NULL,2)//Position out
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSCyberCurb)
    //MAPS_PROPERTY("pName",128,false,false)
	//一下很多是经验值
	MAPS_PROPERTY("Image_Peroid",20000,false,true)//图像周期
	MAPS_PROPERTY("Map_Peroid",10000,false,true)//地图周期
	MAPS_PROPERTY("Laser_Short_Pitch",9,false,true)//短前瞻雷达俯仰角
	MAPS_PROPERTY("Laser_Long_Pitch",7,false,true)//长前瞻雷达俯仰角
	MAPS_PROPERTY("Image_Scale",40,false,true)//图像标尺
	MAPS_PROPERTY("Laser_Short_y_mean_th",3800,false,true)//短前瞻雷达均值阈值、方差阈值、边缘阈值
    MAPS_PROPERTY("Laser_Short_y_dev_th",1500,false,true)
	MAPS_PROPERTY("Laser_Short_edge_th",90,false,true)
	MAPS_PROPERTY("Laser_Long_y_mean_th",6000,false,true)//长前瞻雷达均值阈值、方差阈值、边缘阈值
	MAPS_PROPERTY("Laser_Long_y_dev_th",12000,false,true)
	MAPS_PROPERTY("Laser_Long_edge_th",90,false,true)
	MAPS_PROPERTY("x_movement",-200,false,true)
	MAPS_PROPERTY("x_min",100,false,true)
	MAPS_PROPERTY("kg",0.05,false,true)

MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSCyberCurb)
    //MAPS_ACTION("aName",MAPSCyberCurb::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (CyberCurb) behaviour
MAPS_COMPONENT_DEFINITION(MAPSCyberCurb,"CyberCurb","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  2, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  3, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  14, // Nb of properties. Leave -1 to use the number of declared property definitions
			  0) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSCyberCurb::Birth()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    //ReportInfo("Passing through Birth() method");
	Image_Scale=GetIntegerProperty("Image_Scale");
	Laser_Short_Pitch=GetIntegerProperty("Laser_Short_Pitch");
	Laser_Long_Pitch=GetIntegerProperty("Laser_Long_Pitch");
	Laser_Short_y_mean_th=GetIntegerProperty("Laser_Short_y_mean_th");
	Laser_Short_y_dev_th=GetIntegerProperty("Laser_Short_y_dev_th");
	Laser_Short_edge_th=GetIntegerProperty("Laser_Short_edge_th");
	Laser_Long_y_mean_th=GetIntegerProperty("Laser_Long_y_mean_th");
	Laser_Long_y_dev_th=GetIntegerProperty("Laser_Long_y_dev_th");
	Laser_Long_edge_th=GetIntegerProperty("Laser_Long_edge_th");

	x_movement=GetIntegerProperty("x_movement");
	kg=GetFloatProperty("kg");
	x_min=GetIntegerProperty("x_min");

	Laser_Short_edge_right_x_filter = 0;
	Laser_Long_edge_right_x_filter = 0;
	Laser_Short_edge_left_x_filter = 0;
	Laser_Long_edge_left_x_filter = 0;

	m_nextTimestamp1 = MAPS::CurrentTime();
	Firsttime_image = true;
	m_nextTimestamp2 = MAPS::CurrentTime();
	k = 0;
}

//ATTENTION: 
//	Make sure there is ONE and ONLY ONE blocking function inside this Core method.
//	Consider that Core() will be called inside an infinite loop while the diagram is executing.
//	Something similar to: 
//		while (componentIsRunning) {Core();}
//
//	Usually, the one and only blocking function is one of the following:
//		* StartReading(MAPSInput& input); //Data request on a single BLOCKING input. A "blocking input" is an input declared as FifoReader, LastOrNextReader, Wait4NextReader or NeverskippingReader (declaration happens in MAPS_INPUT: see the beginning of this file). A SamplingReader input is non-blocking: StartReading will not block with a SamplingReader input.
//		* StartReading(int nCount, MAPSInput* inputs[], int* inputThatAnswered, int nCountEvents = 0, MAPSEvent* events[] = NULL); //Data request on several BLOCKING inputs.
//		* SynchroStartReading(int nb, MAPSInput** inputs, MAPSIOElt** IOElts, MAPSInt64 synchroTolerance = 0, MAPSEvent* abortEvent = NULL); // Synchronized reading - waiting for samples with same or nearly same timestamps on several BLOCKING inputs.
//		* Wait(MAPSTimestamp t); or Rest(MAPSDelay d); or MAPS::Sleep(MAPSDelay d); //Pauses the current thread for some time. Can be used for instance in conjunction with StartReading on a SamplingReader input (in which case StartReading is not blocking).
//		* Any blocking grabbing function or other data reception function from another API (device driver,etc.). In such case, make sure this function cannot block forever otherwise it could freeze RTMaps when shutting down diagram.
//**************************************************************************/
//	In case of no blocking function inside the Core, your component will consume 100% of a CPU.
//  Remember that the StartReading function used with an input declared as a SamplingReader is not blocking.
//	In case of two or more blocking functions inside the Core, this is likely to induce synchronization issues and data loss. (Ex: don't call two successive StartReading on FifoReader inputs.)
/***************************************************************************/
void MAPSCyberCurb::Core() 
{
    // Reposrts thi information to the RTMaps console. You can remove this line if you know when Core() is called in the component lifecycle.
    //ReportInfo("Passing through Core() method");
	MAP_Image=cvCreateImage(cvSize(image_width,image_length),8,3);//存放地图图像
//--------------------------------读取激光雷达数据------------------------------
	if(DataAvailableInFIFO(Input("Laser_Short")))//若短前瞻雷达读取到数据
	{
		Laser_Short_In_Elt = StartReading(Input("Laser_Short"));//开始读取数据
		if(Laser_Short_In_Elt!=NULL)//判断读取的数据是否为空
		{
			Sensor_State[0] = true;//短前瞻雷达的状态为真，正在运行中
			for(int i=0;i<240;i++)//读取360点，即扫描180度
			{
				Laser_Short_Raw[i] = Laser_Short_In_Elt->Integer(i+150);//i+90为读取点的索引号，这个索引号的求法（270/0.5-360）/2得出
			}
		}
	}

	if(DataAvailableInFIFO(Input("Laser_Long")))
	{
		Laser_Short_In_Elt = StartReading(Input("Laser_Long"));
		if(Laser_Short_In_Elt!=NULL)
		{
			Sensor_State[1] = true;
			for(int i=0;i<240;i++)
			{
//				Laser_Long_Raw[i] = Laser_Long_In_Elt->Integer(i+60);
			}
		}
	}

//--------------------------------将平面投影到X-Y平面------------------------------
	for(int i=0;i<240;i++)
	{
		Laser_Short_x[i] = Laser_Short_Raw[i]*sin((120-i)*PI/360)+x_movement;
		Laser_Short_y[i] = Laser_Short_Raw[i]*sin((120-i)*PI/360)*cos(Laser_Short_Pitch*PI/180);

		Laser_Long_x[i] = Laser_Long_Raw[i]*sin((120-i)*PI/360)-x_movement;
		Laser_Long_y[i] = Laser_Long_Raw[i]*sin((120-i)*PI/360)*cos(Laser_Long_Pitch*PI/180);
	}
	
//--------------------------------计算统计学数据------------------------------
	for(int i=0;i<48;i++)
	{
		Laser_Short_x_mean[i] = (Laser_Short_x[i*5]+Laser_Short_x[i*5+1]+Laser_Short_x[i*5+2]+Laser_Short_x[i*5+3]+Laser_Short_x[i*5+4])/5;
		Laser_Short_y_mean[i] = (Laser_Short_y[i*5]+Laser_Short_y[i*5+1]+Laser_Short_y[i*5+2]+Laser_Short_y[i*5+3]+Laser_Short_y[i*5+4])/5;
		Laser_Short_x_dev[i] = ((Laser_Short_x[i*5]-Laser_Short_x_mean[i])*(Laser_Short_x[i*5]-Laser_Short_x_mean[i])+(Laser_Short_x[i*5+1]-Laser_Short_x_mean[i])*(Laser_Short_x[i*5+1]-Laser_Short_x_mean[i])+(Laser_Short_x[i*5+2]-Laser_Short_x_mean[i])*(Laser_Short_x[i*5+2]-Laser_Short_x_mean[i])+(Laser_Short_x[i*5+3]-Laser_Short_x_mean[i])*(Laser_Short_x[i*5+3]-Laser_Short_x_mean[i])+(Laser_Short_x[i*5+4]-Laser_Short_x_mean[i])*(Laser_Short_x[i*5+4]-Laser_Short_x_mean[i]))/4;
		Laser_Short_y_dev[i] = ((Laser_Short_y[i*5]-Laser_Short_y_mean[i])*(Laser_Short_y[i*5]-Laser_Short_y_mean[i])+(Laser_Short_y[i*5+1]-Laser_Short_y_mean[i])*(Laser_Short_y[i*5+1]-Laser_Short_y_mean[i])+(Laser_Short_y[i*5+2]-Laser_Short_y_mean[i])*(Laser_Short_y[i*5+2]-Laser_Short_y_mean[i])+(Laser_Short_y[i*5+3]-Laser_Short_y_mean[i])*(Laser_Short_y[i*5+3]-Laser_Short_y_mean[i])+(Laser_Short_y[i*5+4]-Laser_Short_y_mean[i])*(Laser_Short_y[i*5+4]-Laser_Short_y_mean[i]))/4;
	    Laser_Short_xy_dev[i] = Laser_Short_y_dev[i]/(Laser_Short_x_dev[i]+1);

		Laser_Long_x_mean[i] = (Laser_Long_x[i*5]+Laser_Long_x[i*5+1]+Laser_Long_x[i*5+2]+Laser_Long_x[i*5+3]+Laser_Long_x[i*5+4])/5;
		Laser_Long_y_mean[i] = (Laser_Long_y[i*5]+Laser_Long_y[i*5+1]+Laser_Long_y[i*5+2]+Laser_Long_y[i*5+3]+Laser_Long_y[i*5+4])/5;
		Laser_Long_x_dev[i] = ((Laser_Long_x[i*5]-Laser_Long_x_mean[i])*(Laser_Long_x[i*5]-Laser_Long_x_mean[i])+(Laser_Long_x[i*5+1]-Laser_Long_x_mean[i])*(Laser_Long_x[i*5+1]-Laser_Long_x_mean[i])+(Laser_Long_x[i*5+2]-Laser_Long_x_mean[i])*(Laser_Long_x[i*5+2]-Laser_Long_x_mean[i])+(Laser_Long_x[i*5+3]-Laser_Long_x_mean[i])*(Laser_Long_x[i*5+3]-Laser_Long_x_mean[i])+(Laser_Long_x[i*5+4]-Laser_Long_x_mean[i])*(Laser_Long_x[i*5+4]-Laser_Long_x_mean[i]))/4;
		Laser_Long_y_dev[i] = ((Laser_Long_y[i*5]-Laser_Long_y_mean[i])*(Laser_Long_y[i*5]-Laser_Long_y_mean[i])+(Laser_Long_y[i*5+1]-Laser_Long_y_mean[i])*(Laser_Long_y[i*5+1]-Laser_Long_y_mean[i])+(Laser_Long_y[i*5+2]-Laser_Long_y_mean[i])*(Laser_Long_y[i*5+2]-Laser_Long_y_mean[i])+(Laser_Long_y[i*5+3]-Laser_Long_y_mean[i])*(Laser_Long_y[i*5+3]-Laser_Long_y_mean[i])+(Laser_Long_y[i*5+4]-Laser_Long_y_mean[i])*(Laser_Long_y[i*5+4]-Laser_Long_y_mean[i]))/4;
	    Laser_Long_xy_dev[i] = Laser_Long_y_dev[i]/(Laser_Long_x_dev[i]+1);
	}

//--------------------------------判断路面上的点------------------------------
	for(int i=0;i<48;i++)
	{
		if((Laser_Short_y_mean[i]>=Laser_Short_y_mean_th)&&(Laser_Short_y_dev[i]<=Laser_Short_y_dev_th))
		{
			Road_Short[i] = 1;//1表示为路面上的点
		}
		else
		{
			Road_Short[i] = 0;//0表示非路面上的点
		}
		if((Laser_Long_y_mean[i]>=Laser_Long_y_mean_th)&&(Laser_Long_y_dev[i]<=Laser_Long_y_dev_th))
		{
			Road_Long[i] = 1;
		}
		else
		{
			Road_Long[i] = 0;
		}
	}

//--------------------------------滤波------------------------------
	for(int i=1;i<47;++i)
	{
		if((Road_Short[i-1]==1)&&(Road_Short[i]==0)&&(Road_Short[i+1]==1))
		{
			Road_Short[i] = 1;
		}
		if((Road_Long[i-1==1])&&(Road_Long[i]==0)&&(Road_Long[i+1]==1))
		{
			Road_Long[i] = 1;
		}
	}

//--------------------------------寻找道路左边缘------------------------------
	Laser_Short_Left_max = Laser_Short_xy_dev[0];
	Laser_Long_Left_max = Laser_Short_xy_dev[0];
	for(int i=24;i<48;++i)
	{
		if(Laser_Short_xy_dev[i]>=Laser_Short_Left_max)
		{
			Laser_Short_Left_max = Laser_Short_xy_dev[i];
			Laser_Short_Left_position = i;
		}
		if(Laser_Long_xy_dev[i]>=Laser_Long_Left_max)
		{
			Laser_Long_Left_max = Laser_Long_xy_dev[i];
			Laser_Long_Left_position = i;
		}
	}
	if(Laser_Short_Left_max>=Laser_Short_edge_th)
		Laser_Short_Left_flag = true;
	else
		Laser_Short_Left_flag = false;
	if(Laser_Long_Left_max>=Laser_Long_edge_th)
		Laser_Long_Left_flag = true;
	else
		Laser_Long_Left_flag = false;

//--------------------------------寻找道路右边缘------------------------------
	Laser_Short_Right_max = Laser_Short_xy_dev[0];
	Laser_Long_Right_max = Laser_Short_xy_dev[0];
	for(int i=0;i<24;++i)
	{
		if(Laser_Short_xy_dev[i]>=Laser_Short_Right_max)
		{
			Laser_Short_Right_max = Laser_Short_xy_dev[i];
			Laser_Short_Right_position = i;
		}
		if(Laser_Long_xy_dev[i]>=Laser_Long_Right_max)
		{
			Laser_Long_Right_max = Laser_Long_xy_dev[i];
			Laser_Long_Right_position = i;
		}
	}
	if(Laser_Short_Right_max>=Laser_Short_edge_th)
		Laser_Short_Right_flag = true;
	else
		Laser_Short_Right_flag = false;
	if(Laser_Long_Right_max>=Laser_Long_edge_th)
		Laser_Long_Right_flag = true;
	else
		Laser_Long_Right_flag = false;

//--------------------------------处理检测后的结果------------------------------
	Laser_Short_edge_right_x = -1;
	Laser_Short_edge_left_x = -1;
	Laser_Long_edge_left_x = -1;
	Laser_Long_edge_right_x = -1;

	//处理短前瞻雷达右侧
	if(Laser_Short_Right_flag)
	{
		Laser_Short_edge_right_x = Laser_Short_x_mean[Laser_Short_Right_position];//保存x坐标值
		Laser_Short_edge_right_y = Laser_Short_y_mean[Laser_Short_Right_position];//保存y坐标值
		Road_Short[Laser_Short_Right_position] = 2;//此时将Road_[]标记为2
		lidar_edge_pos_stru.short_right = Laser_Short_Right_position;
	}
	else
	{
		int j;
		for(j=24;j>=0;j--)
		{
			if(Road_Short[j]!=1)
			{
				Laser_Short_edge_right_x = Laser_Short_x_mean[j];
				Laser_Short_edge_right_y = Laser_Short_y_mean[j];
				lidar_edge_pos_stru.short_right = j;
				break;
			}
		}
		if(j==0)//若都不是，保存最边上的点
		{
			Laser_Short_edge_right_x = Laser_Short_x_mean[0];
			Laser_Short_edge_right_y = Laser_Short_y_mean[0];
			lidar_edge_pos_stru.short_right = 0;
		}
	}
	//处理长前瞻雷达右侧
	if(Laser_Long_Right_flag)
	{
		Laser_Long_edge_right_x = Laser_Long_x_mean[Laser_Long_Right_position];//保存x坐标值
		Laser_Long_edge_right_y = Laser_Long_y_mean[Laser_Long_Right_position];//保存y坐标值
		Road_Long[Laser_Long_Right_position] = 2;
	}
	else
	{
		int j;
		for(j=24;j>=0;j--)
		{
			if(Road_Long[j]!=1)
			{
				Laser_Long_edge_right_x = Laser_Long_x_mean[j];
				Laser_Long_edge_right_y = Laser_Long_y_mean[j];
				break;
			}
		}
		if(j==0)//若都不是，保存最边上的点
		{
			Laser_Long_edge_right_x = Laser_Long_x_mean[0];
			Laser_Long_edge_right_y = Laser_Long_y_mean[0];
		}
	}
	//处理短前瞻雷达左侧
	if(Laser_Short_Left_flag)
	{
		Laser_Short_edge_left_x = Laser_Short_x_mean[Laser_Short_Left_position];//保存x坐标值
		Laser_Short_edge_left_y = Laser_Short_y_mean[Laser_Short_Left_position];//保存y坐标值
		Road_Short[Laser_Short_Left_position] = 3;//左侧的Road_[]都标记为3
		lidar_edge_pos_stru.short_left = Laser_Short_Left_position;
	}
	else
	{
		int j;
		for(j=25;j<48;j++)
		{
			if(Road_Short[j]!=1)
			{
				Laser_Short_edge_left_x = Laser_Short_x_mean[j];
				Laser_Short_edge_left_y = Laser_Short_y_mean[j];
				lidar_edge_pos_stru.short_left = j;
				break;
			}
		}
		if(j==48)//若都不是，保存最边上的点
		{
			Laser_Short_edge_left_x = Laser_Short_x_mean[47];
			Laser_Short_edge_left_y = Laser_Short_y_mean[47];
			lidar_edge_pos_stru.short_left = 47;
		}
	}
	//处理长前瞻雷达左侧
	if(Laser_Long_Left_flag)
	{
		Laser_Long_edge_left_x = Laser_Long_x_mean[Laser_Long_Left_position];//保存x坐标值
		Laser_Long_edge_left_y = Laser_Long_y_mean[Laser_Long_Left_position];//保存y坐标值
		Road_Long[Laser_Long_Left_position] = 3;
	}
	else
	{
		int j;
		for(j=25;j<48;j++)
		{
			if(Road_Long[j]!=1)
			{
				Laser_Long_edge_left_x = Laser_Long_x_mean[j];
				Laser_Long_edge_left_y = Laser_Long_y_mean[j];
				break;
			}
		}
		if(j==48)//若都不是，保存最边上的点
		{
			Laser_Long_edge_left_x = Laser_Long_x_mean[47];
			Laser_Long_edge_left_y = Laser_Long_y_mean[47];
		}
	}

	if(Laser_Short_edge_right_x<x_min)
		Laser_Short_edge_right_x = Laser_Short_edge_right_x_former;
	Laser_Short_edge_right_x_former = Laser_Short_edge_right_x;

	if(Laser_Long_edge_right_x<x_min)
		Laser_Long_edge_right_x = Laser_Long_edge_right_x_former;
	Laser_Long_edge_right_x_former = Laser_Long_edge_right_x;

	if(Laser_Short_edge_left_x>-1*x_min)
		Laser_Short_edge_left_x = Laser_Short_edge_left_x_former;
	Laser_Short_edge_left_x_former = Laser_Short_edge_left_x;

	if(Laser_Long_edge_left_x>-1*x_min)
		Laser_Long_edge_left_x = Laser_Long_edge_left_x_former;
	Laser_Long_edge_left_x_former = Laser_Long_edge_left_x;

//--------------------------------一阶滤波------------------------------

	Laser_Short_edge_right_x_filter = Laser_Short_edge_right_x_filter;
	Laser_Short_edge_right_x_filter = Laser_Short_edge_right_x_filter+kg*(Laser_Short_edge_right_x-Laser_Short_edge_right_x_filter);

	Laser_Long_edge_right_x_filter = Laser_Long_edge_right_x_filter;
	Laser_Long_edge_right_x_filter = Laser_Long_edge_right_x_filter+kg*(Laser_Long_edge_right_x-Laser_Long_edge_right_x_filter);

	Laser_Short_edge_left_x_filter = Laser_Short_edge_left_x_filter;
	Laser_Short_edge_left_x_filter = Laser_Short_edge_left_x_filter+kg*(Laser_Short_edge_left_x-Laser_Short_edge_left_x_filter);

	Laser_Long_edge_left_x_filter = Laser_Long_edge_left_x_filter;
	Laser_Long_edge_left_x_filter = Laser_Long_edge_left_x_filter+kg*(Laser_Long_edge_left_x-Laser_Long_edge_left_x_filter);

//--------------------------------创建图像------------------------------
	if(Firsttime_image)
	{
		IplImage model;
		model=MAPS::IplImageModel(image_width,image_length,"BGR");
		Output("Map_Image_Out").AllocOutputBufferIplImage(model);
		Firsttime_image = false;
	}
	Wait(m_nextTimestamp1);

//--------------------------------开始作图------------------------------
	for(int i=0;i<240;i++)
	{
		image_x_temp = image_width/2+Laser_Short_x[i]/Image_Scale;
		image_y_temp = image_length/2-Laser_Short_y[i]/Image_Scale;

		if(Road_Short[i/5]==1)
			cvCircle(MAP_Image,cvPoint(image_x_temp,image_y_temp),2,CV_RGB(0,255,0),1,8,0);//绿色
		else if(Road_Short[i/5]==0)
			cvCircle(MAP_Image,cvPoint(image_x_temp,image_y_temp),2,CV_RGB(0,0,0),1,8,0);//黑色
		else if(Road_Short[i/5]==2)
			cvCircle(MAP_Image,cvPoint(image_x_temp,image_y_temp),2,CV_RGB(255,0,0),1,8,0);//红色
		else if(Road_Short[i/5]==3)
			cvCircle(MAP_Image,cvPoint(image_x_temp,image_y_temp),2,CV_RGB(0,0,255),1,8,0);//蓝色
	}
	for(int i=0;i<240;i++)
	{
		image_x_temp = image_width/2+Laser_Long_x[i]/Image_Scale;
		image_y_temp = image_length/2-Laser_Long_y[i]/Image_Scale;

		if(Road_Long[i/5]==1)
			cvCircle(MAP_Image,cvPoint(image_x_temp,image_y_temp),2,CV_RGB(0,255,0),1,8,0);
		else if(Road_Long[i/5]==0)
			cvCircle(MAP_Image,cvPoint(image_x_temp,image_y_temp),2,CV_RGB(0,0,0),1,8,0);
		else if(Road_Long[i/5]==2)
			cvCircle(MAP_Image,cvPoint(image_x_temp,image_y_temp),2,CV_RGB(255,0,0),1,8,0);
		else if(Road_Long[i/5]==3)
			cvCircle(MAP_Image,cvPoint(image_x_temp,image_y_temp),2,CV_RGB(0,0,255),1,8,0);
	}

	cvCircle(MAP_Image,cvPoint(image_width/2+Laser_Short_edge_right_x_filter/Image_Scale,image_length-Laser_Short_edge_right_y/Image_Scale),10,CV_RGB(128,128,255),1,8,0);
	cvCircle(MAP_Image,cvPoint(image_width/2+Laser_Long_edge_right_x_filter/Image_Scale,image_length-Laser_Long_edge_right_y/Image_Scale),10,CV_RGB(128,128,255),1,8,0);
	cvCircle(MAP_Image,cvPoint(image_width/2+Laser_Short_edge_left_x_filter/Image_Scale,image_length-Laser_Short_edge_left_y/Image_Scale),10,CV_RGB(128,128,255),1,8,0);
	cvCircle(MAP_Image,cvPoint(image_width/2+Laser_Long_edge_left_x_filter/Image_Scale,image_length-Laser_Long_edge_left_y/Image_Scale),10,CV_RGB(128,128,255),1,8,0);

	//测试
	for(int i=0;i<600;i=i++)
	{
		cvLine(MAP_Image,cvPoint(0,0),cvPoint(0,i),CV_RGB(255,0,0),1,8,0);
		cvCircle(MAP_Image,cvPoint(200,i),4,CV_RGB(255,0,0),1,8,0);
	}
	cvCircle(MAP_Image,cvPoint(k,k),100,CV_RGB(255,0,0),1,8,0);
	k = k+1;
//--------------------------------输出文本流------------------------------
	cvInitFont(&font1,CV_FONT_HERSHEY_COMPLEX_SMALL,0.5,0.5,0.5,1,8);
	sprintf(C_buffer,"Laser_Short_right:%d",Laser_Short_edge_right_x_filter);
	cvPutText(MAP_Image,C_buffer,cvPoint(310,90),&font1,CV_RGB(0,0,0));
	sprintf(C_buffer,"Laser_Long_right:%d",Laser_Long_edge_right_x_filter);
	cvPutText(MAP_Image,C_buffer,cvPoint(310,60),&font1,CV_RGB(0,0,0));
	sprintf(C_buffer,"Laser_Short_left:%d",Laser_Short_edge_left_x_filter);
	cvPutText(MAP_Image,C_buffer,cvPoint(10,90),&font1,CV_RGB(0,0,0));
	sprintf(C_buffer,"Laser_Long_left:%d",Laser_Long_edge_left_x_filter);
	cvPutText(MAP_Image,C_buffer,cvPoint(10,60),&font1,CV_RGB(0,0,0));

//--------------------------------可画出两条方向直线------------------------------

//--------------------------------输出图像的缓冲------------------------------
	if(MAPS::CurrentTime()>m_nextTimestamp1)
	{
		MAP_Image_Out_Elt = StartWriting(Output("Map_Image_Out"));
		IplImage &oIplImage=MAP_Image_Out_Elt->IplImage();
		MAP_Image_Out_Elt->Timestamp() = MAPS::CurrentTime();
		ib = (unsigned char*)(*MAP_Image).imageData;
		ob = (unsigned char*)oIplImage.imageData;
		MAPS::Memcpy(ob,ib,((*MAP_Image).height)*((*MAP_Image).widthStep));
		//停止写入
		StopWriting(MAP_Image_Out_Elt);
		m_nextTimestamp1 = MAPS::CurrentTime()+GetIntegerProperty("Image_Peroid");
	}
	//释放图像
	cvReleaseImage(&MAP_Image);
	//输出数据
	if(MAPS::CurrentTime()>m_nextTimestamp1)
	{
		//写入路沿信息
		edge_info = StartWriting(Output("Road_Curb"));
		edge_info->Integer(0) = Laser_Short_edge_right_x_filter;//输出滤波后的值
		edge_info->Integer(1) = Laser_Short_edge_left_x_filter;
		edge_info->Integer(2) = Laser_Long_edge_right_x_filter;
		edge_info->Integer(3) = Laser_Long_edge_left_x_filter;
		//停止写入
		StopWriting(edge_info);

		edge_position = StartWriting(Output("Road_Edge_Position"));
		lidar_right[0] = (lidar_edge_pos_stru.short_right*5+3)*13/12;
		lidar_left[0] = (lidar_edge_pos_stru.short_left*5+3)*13/12;
		lidar_right[0] = (lidar_right[4]+lidar_right[3]+lidar_right[2]+lidar_right[1]+lidar_right[0])/5;
		lidar_left[0] = (lidar_left[4]+lidar_left[3]+lidar_left[2]+lidar_left[1]+lidar_left[0])/5;
		lidar_right[4]=lidar_right[3];lidar_right[3]=lidar_right[2];lidar_right[2]=lidar_right[1];lidar_right[1]=lidar_right[0];
		lidar_left[4]=lidar_left[3];lidar_left[3]=lidar_left[2];lidar_left[2]=lidar_left[1];lidar_left[1]=lidar_left[0];
		edge_position->Integer(0) =  lidar_right[0];//0;//lidar_r[0];
		edge_position->Integer(1) =  lidar_left[0];//1000;lidar_l[0];
		StopWriting(edge_position);
	}

    m_nextTimestamp1=MAPS::CurrentTime()+GetIntegerProperty("Map_Peroid");
}

void MAPSCyberCurb::Death()
{
    ReportInfo("Passing through Death() method");
}
