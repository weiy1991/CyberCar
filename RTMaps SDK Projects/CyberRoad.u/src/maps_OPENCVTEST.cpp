////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////
#include"opencv2\opencv.hpp"
#include "maps_OPENCVTEST.h"	// Includes the header of this component
#include "cmath"

#define img_width 600
#define img_length 600//图像尺寸

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSOPENCVTEST)
MAPS_INPUT("Laser_Short",MAPS::FilterInteger,MAPS::LastOrNextReader)
MAPS_INPUT("Laser_Long",MAPS::FilterInteger,MAPS::LastOrNextReader)
MAPS_INPUT("Laser_Front",MAPS::FilterInteger,MAPS::LastOrNextReader)
MAPS_INPUT("VELOCITY",MAPS::FilterFloat,MAPS::LastOrNextReader)
MAPS_INPUT("road",MAPS::FilterInteger,MAPS::LastOrNextReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSOPENCVTEST)
MAPS_OUTPUT("Map_Image_Out",MAPS::IplImage,NULL,NULL,1)//输出地图显示图像
MAPS_OUTPUT("road_curb",MAPS::Integer,NULL,NULL,8)//输出路沿
MAPS_OUTPUT("road_edge_position",MAPS::Integer,NULL,NULL,2)//输出道路边缘位置
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSOPENCVTEST)
  MAPS_PROPERTY("Map_Period_us",10000,false,true)
  MAPS_PROPERTY("Image_Period_us",20000,false,true)
  MAPS_PROPERTY("Laser_Short_Pitch",10,false,true)//雷达倾斜角赋值
  MAPS_PROPERTY("Laser_Long_Pitch",7,false,true)
  MAPS_PROPERTY("Image_Scale",40,false,true)
  MAPS_PROPERTY("laser_short_y_mean_th",3800,false,true)//xiugai3800
  MAPS_PROPERTY("laser_short_y_dev_th",1500,false,true)//xiugai1500
  MAPS_PROPERTY("laser_short_edge_th",90,false,true)//xiugai90
  MAPS_PROPERTY("laser_long_y_mean_th",3800,false,true)//xiugai6000
  MAPS_PROPERTY("laser_long_y_dev_th",4000,false,true)//xiugai12000
  MAPS_PROPERTY("laser_long_edge_th",90,false,true)//xiugai50
  //设定每一个激光雷达坐标系转化参数
  MAPS_PROPERTY("short_x_movement",0,false,true)
  MAPS_PROPERTY("short_y_movement",2360,false,true)
  MAPS_PROPERTY("long_x_movement",0,false,true)
  MAPS_PROPERTY("long_y_movement",-250,false,true)
  MAPS_PROPERTY("front_x_movement",130,false,true)
  MAPS_PROPERTY("front_y_movement",2400,false,true)
 // MAPS_PROPERTY("x_movement",0,false,true)
  MAPS_PROPERTY("Laser_Front_Display",1,false,true)
  MAPS_PROPERTY("y_min",100,false,true)
  MAPS_PROPERTY("warning_x",1200,false,true)
  MAPS_PROPERTY("warning_y",7000,false,true)
  MAPS_PROPERTY("kg",0.05,false,true)
  MAPS_PROPERTY("warning_factor",500,false,true)
  MAPS_PROPERTY("SPEEDLIMITDISPLAY",1,false,true)
  
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSOPENCVTEST)
    //MAPS_ACTION("aName",MAPSOPENCVTEST::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (OPENCVTEST) behaviour
MAPS_COMPONENT_DEFINITION(MAPSOPENCVTEST,"OPENCVTEST","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  5, // Nb of inputs min
			  3, // Nb of outputs min 
			  24, // Nb of properties min
			  0) // Nb of actions min

void MAPSOPENCVTEST::Birth()
{
   img_scale=GetIntegerProperty("Image_Scale");
   laser_short_pitch=GetIntegerProperty("Laser_Short_Pitch");
   laser_long_pitch=GetIntegerProperty("Laser_Long_Pitch");

   laser_short_y_mean_th=GetIntegerProperty("laser_short_y_mean_th");
   laser_short_y_dev_th=GetIntegerProperty("laser_short_y_dev_th");
   laser_short_edge_th=GetIntegerProperty("laser_short_edge_th");

   laser_long_y_mean_th=GetIntegerProperty("laser_long_y_mean_th");
   laser_long_y_dev_th=GetIntegerProperty("laser_long_y_dev_th");
   laser_long_edge_th=GetIntegerProperty("laser_long_edge_th");

   short_x_movement=GetIntegerProperty("short_x_movement");
   short_y_movement=GetIntegerProperty("short_y_movement");
   long_x_movement=GetIntegerProperty("long_x_movement");
   long_y_movement=GetIntegerProperty("long_y_movement");
   front_x_movement=GetIntegerProperty("front_x_movement");
   front_y_movement=GetIntegerProperty("front_y_movement");

   laser_front_display=GetIntegerProperty("Laser_Front_Display");
   SPEEDLIMITDISPLAY=GetIntegerProperty("SPEEDLIMITDISPLAY");
   y_min=GetIntegerProperty("y_min");
   warning_x=GetIntegerProperty("warning_x");
   warning_y=GetIntegerProperty("warning_y");

   kg=GetFloatProperty("kg");

   laser_short_pitch_cos=cos(laser_short_pitch*3.1415/180);//激光雷达数据的cos值，为了求出对应的水平距离做好准备
   laser_long_pitch_cos=cos(laser_long_pitch*3.1415/180);
   m_nextTimestamp=MAPS::CurrentTime();
   m_firsttime_image=true;
   m_nextTimestamp1=MAPS::CurrentTime();

   laser_short_edge_right_filter=0;
   laser_short_edge_left_filter=0;
   laser_long_edge_right_filter=0;
   laser_long_edge_left_filter=0;

   velocity[0]=0;
	current_time = MAPS::CurrentTime();
	last_time = current_time;
	for(int i=0;i<140;i++)
		laser_long_raw[i]=0.0;
}

void MAPSOPENCVTEST::Core() //main function ,executing when the new datas arrvive.
{
		Map_Image=cvCreateImage(cvSize(img_width,img_length),8,3);//存放地图图像
	//----------------读取激光雷达原始数据------------------------------------------
	if(true == DataAvailableInFIFO(Input("Laser_Short"))){
		Laser_Short_In_Elt = StartReading(Input("Laser_Short"));
		if(Laser_Short_In_Elt!=NULL){	

			sensor_state[0] = 1;
			for (int i=0;i<240;i++)
	         {
				 laser_short_raw[i]=Laser_Short_In_Elt->Integer(i+150);//Laser_Short_In_Elt的定义是什么

	         }
		}
	}
	
	if(true == DataAvailableInFIFO(Input("Laser_Long"))){
		Laser_Long_In_Elt = StartReading(Input("Laser_Long")); 
		if(Laser_Long_In_Elt!=NULL){	

			sensor_state[1] = 1;
			for (int i=0;i<140;i++)
	         {
				 laser_long_raw[i]=Laser_Long_In_Elt->Integer(i+200);//Laser_Long_In_Elt的定义是什么
	         }
		}
	}

	if(true == DataAvailableInFIFO(Input("Laser_Front"))){
		Laser_Front_In_Elt = StartReading(Input("Laser_Front"));
		if(Laser_Front_In_Elt!=NULL){	

			sensor_state[2] = 1;
			for (int i=0;i<240;i++)
	         {
				 laser_front_raw[i]=Laser_Front_In_Elt->Integer(i+70);
	         }
		}
	}
	if(true == DataAvailableInFIFO(Input("VELOCITY"))){
		VELOCITY_In_Elt = StartReading(Input("VELOCITY"));
		if(VELOCITY_In_Elt!=NULL){	
			velocity[0] = VELOCITY_In_Elt->Float(0);
		}
	}
	if(true == DataAvailableInFIFO(Input("road"))){
		ROAD_CURB_In_Elt = StartReading(Input("road"));
		
		if(ROAD_CURB_In_Elt!=NULL){	
			road_curb_right = ROAD_CURB_In_Elt->Integer(0);
			road_curb_left = ROAD_CURB_In_Elt->Integer(1);
		}

	}
	
	//---------------------转换为X-Y平面-----------------------
	for (int i=0;i<240;i++)
	{
     laser_short_x[i]=laser_short_raw[i]*sin((120-i)*3.1415/360)+short_x_movement;//映射到X，x_movement是安装位置和车的中心线的偏差
	 laser_short_y[i]=(laser_short_raw[i]*cos((120-i)*3.1415/360)*laser_short_pitch_cos)+short_y_movement;//映射到Y，感觉这个公式有问题，有个角度应该是变化的
	 
	 laser_front_x[i]=laser_front_raw[i]*sin((120-i)*3.1415/360)+front_x_movement;//映射到X
	 laser_front_y[i]=laser_front_raw[i]*cos((120-i)*3.1415/360)+front_y_movement;//映射到Y
	// sprintf(buffer,"x:%d,y:%d", laser_long_x[i], laser_long_y[i]);
	//	ReportInfo(buffer);
	}
	for (int i=0;i<140;i++)
	{
	 laser_long_x[i]=laser_long_raw[i]*sin((70-i)*3.1415/360)+long_x_movement;//映射到X
	 laser_long_y[i]=(laser_long_raw[i]*cos((70-i)*3.1415/360)*laser_long_pitch_cos)+long_y_movement;//映射到Y
	}
    //------------------------开始计算统计学参数-----------------------------------------
	for (int i=0; i<48;i++)
	{
		laser_short_x_mean[i]=(laser_short_x[i*5]+laser_short_x[i*5+1]+laser_short_x[i*5+2]+laser_short_x[i*5+3]+laser_short_x[i*5+4])/5;//计算X均值
		laser_short_y_mean[i]=(laser_short_y[i*5]+laser_short_y[i*5+1]+laser_short_y[i*5+2]+laser_short_y[i*5+3]+laser_short_y[i*5+4])/5;//计算Y均值
		laser_short_x_dev[i]=((laser_short_x[i*5]-laser_short_x_mean[i])*(laser_short_x[i*5]-laser_short_x_mean[i])+(laser_short_x[i*5+1]-laser_short_x_mean[i])*(laser_short_x[i*5+1]-laser_short_x_mean[i])+(laser_short_x[i*5+2]-laser_short_x_mean[i])*(laser_short_x[i*5+2]-laser_short_x_mean[i])+(laser_short_x[i*5+3]-laser_short_x_mean[i])*(laser_short_x[i*5+3]-laser_short_x_mean[i])+(laser_short_x[i*5+4]-laser_short_x_mean[i])*(laser_short_x[i*5+4]-laser_short_x_mean[i]))/4;//计算X方差
		laser_short_y_dev[i]=((laser_short_y[i*5]-laser_short_y_mean[i])*(laser_short_y[i*5]-laser_short_y_mean[i])+(laser_short_y[i*5+1]-laser_short_y_mean[i])*(laser_short_y[i*5+1]-laser_short_y_mean[i])+(laser_short_y[i*5+2]-laser_short_y_mean[i])*(laser_short_y[i*5+2]-laser_short_y_mean[i])+(laser_short_y[i*5+3]-laser_short_y_mean[i])*(laser_short_y[i*5+3]-laser_short_y_mean[i])+(laser_short_y[i*5+4]-laser_short_y_mean[i])*(laser_short_y[i*5+4]-laser_short_y_mean[i]))/4;//计算Y方差
		laser_short_xy_dev[i]=laser_short_y_dev[i]/(laser_short_x_dev[i]+1);

		laser_front_raw_mean[i]=(laser_front_raw[i*5]+laser_front_raw[i*5+1]+laser_front_raw[i*5+2]+laser_front_raw[i*5+3]+laser_front_raw[i*5+4])/5;//计算X均值
		laser_front_x_mean[i]=(laser_front_x[i*5]+laser_front_x[i*5+1]+laser_front_x[i*5+2]+laser_front_x[i*5+3]+laser_front_x[i*5+4])/5;//计算front均值
		laser_front_y_mean[i]=(laser_front_y[i*5]+laser_front_y[i*5+1]+laser_front_y[i*5+2]+laser_front_y[i*5+3]+laser_front_y[i*5+4])/5;//计算Y均值
	}
	for (int i=0; i<28;i++)
	{
		laser_long_x_mean[i]=(laser_long_x[i*5]+laser_long_x[i*5+1]+laser_long_x[i*5+2]+laser_long_x[i*5+3]+laser_long_x[i*5+4])/5;//计算X均值
		laser_long_y_mean[i]=(laser_long_y[i*5]+laser_long_y[i*5+1]+laser_long_y[i*5+2]+laser_long_y[i*5+3]+laser_long_y[i*5+4])/5;//计算Y均值
		laser_long_x_dev[i]=((laser_long_x[i*5]-laser_long_x_mean[i])*(laser_long_x[i*5]-laser_long_x_mean[i])+(laser_long_x[i*5+1]-laser_long_x_mean[i])*(laser_long_x[i*5+1]-laser_long_x_mean[i])+(laser_long_x[i*5+2]-laser_long_x_mean[i])*(laser_long_x[i*5+2]-laser_long_x_mean[i])+(laser_long_x[i*5+3]-laser_long_x_mean[i])*(laser_long_x[i*5+3]-laser_long_x_mean[i])+(laser_long_x[i*5+4]-laser_long_x_mean[i])*(laser_long_x[i*5+4]-laser_long_x_mean[i]))/4;//计算X方差
		laser_long_y_dev[i]=((laser_long_y[i*5]-laser_long_y_mean[i])*(laser_long_y[i*5]-laser_long_y_mean[i])+(laser_long_y[i*5+1]-laser_long_y_mean[i])*(laser_long_y[i*5+1]-laser_long_y_mean[i])+(laser_long_y[i*5+2]-laser_long_y_mean[i])*(laser_long_y[i*5+2]-laser_long_y_mean[i])+(laser_long_y[i*5+3]-laser_long_y_mean[i])*(laser_long_y[i*5+3]-laser_long_y_mean[i])+(laser_long_y[i*5+4]-laser_long_y_mean[i])*(laser_long_y[i*5+4]-laser_long_y_mean[i]))/4;//计算Y方差
		laser_long_xy_dev[i]=laser_long_y_dev[i]/(laser_long_x_dev[i]+1);
	}

//---------------根据统计学信息判断点是不是在路面上-----------------------------------

	for (int i=0;i<48;i++)
	{
		/**判断y方向上的均值是否大于阀值和y方向上的方差小于阀值*/
		if ((laser_short_y_mean[i]>=laser_short_y_mean_th)&&(laser_short_y_dev[i]<=laser_short_y_dev_th))//这个阈值是多少，在哪里计算得出的
		{
			road_short[i]=1;	//*路面点
		}
		else
		{
			road_short[i]=0;	//*非路面点
		}

	}
	for (int i=0;i<28;i++)
	{
		/**判断y方向上的均值是否大于阀值和y方向上的方差小于阀值*/
		if ((laser_long_y_mean[i]>=laser_long_y_mean_th)&&(laser_long_y_dev[i]<=laser_long_y_dev_th))
		{
			road_long[i]=1;
		}
		else
		{
			road_long[i]=0;
		}
	}
//---------------对路面信息进行滤波，去除某些噪点---------------------------------------

	for (int i=1;i<47;++i)//这种滤波的原理是什么,这个滤波应该是很有道理的
	{
		if ((road_short[i-1]==1)&&(road_short[i]==0)&&(road_short[i+1]==1))
		{
			road_short[i]=1;
		}
	}
	for (int i=1;i<27;++i)//这种滤波的原理是什么,这个滤波应该是很有道理的
	{
		if ((road_long[i-1]==1)&&(road_long[i]==0)&&(road_long[i+1]==1))
		{
			road_long[i]=1;
		}
	}

	//---------------根据统计学信息寻找道路边缘------------------------

	laser_short_right_max=laser_short_xy_dev[0];
	laser_long_right_max=laser_long_xy_dev[0];
	/**寻找最大方差比*/
	for (int i=0;i<24;++i)
	{	
      if (laser_short_xy_dev[i]>=laser_short_right_max)
	  {
		  laser_short_right_max=laser_short_xy_dev[i];
		  laser_short_right_position=i;//保存最大方差点
	  }
	}
	for (int i=0;i<14;++i)
	{	
	  if (laser_long_xy_dev[i]>=laser_long_right_max)
	  {
		  laser_long_right_max=laser_long_xy_dev[i];
		  laser_long_right_position=i;
	  }
	}
	/**最大方差比大于阀值则存在路边*/
	if (laser_short_right_max>=laser_short_edge_th)
	{
		laser_short_right_flag=true;
	}
	else
	{
		laser_short_right_flag=false;
	}
	if (laser_long_right_max>=laser_long_edge_th)
	{
		laser_long_right_flag=true;
	}
	else
	{
		laser_long_right_flag=false;
	}

   //------------------以上为寻找道路右边缘，以下为寻找左边缘
	laser_short_left_max=laser_short_xy_dev[0];
	laser_long_left_max=laser_long_xy_dev[0];
	for (int i=24;i<48;++i)
	{
      if (laser_short_xy_dev[i]>=laser_short_left_max)
	  {
		  laser_short_left_max=laser_short_xy_dev[i];
		  laser_short_left_position=i;
	  }
	}
	for (int i=14;i<28;++i)
	{
	  if (laser_long_xy_dev[i]>=laser_long_left_max)
	  {
		  laser_long_left_max=laser_long_xy_dev[i];
		  laser_long_left_position=i;
	  }
	}
	if (laser_short_left_max>=laser_short_edge_th)
	{
		laser_short_left_flag=true;
	}
	else
	{
		laser_short_left_flag=false;
	}
	if (laser_long_left_max>=laser_long_edge_th)
	{
		laser_long_left_flag=true;
	}
	else
	{
		laser_long_left_flag=false;
	}
	//-----------边缘寻找结束，以下程序为处理检测后的数据--------------------------

laser_short_edge_right=-1;
laser_short_edge_left=-1;
laser_long_edge_right=-1;
laser_long_edge_left=-1;

	/**存在路边时保存路边的x坐标值，y坐标值，road_short[i]标记为4*/
	if (laser_short_right_flag)
	{
		laser_short_edge_right=laser_short_x_mean[laser_short_right_position];
		short_right_y=laser_short_y_mean[laser_short_right_position];
		road_short[laser_short_right_position]=4;
		lidar_edge_pos_stru.short_right = laser_short_right_position;	//保存短lidar右边位置
	}
	
	else
	{
	 for (j=24;j>=0;j--)
       {
	     if (road_short[j]!=1)	/**y方向距离小于阀值，或方差大于阀值即road_short=0,记为路边，再即最靠里的非路面点即为路边*/
	     {
		   laser_short_edge_right=laser_short_x_mean[j];	
		   short_right_y=laser_short_y_mean[j];
		   lidar_edge_pos_stru.short_right = j;	//保存短lidar右边位置

		   break;
	     }
       }
	 if (j==0)	/**若都不满足把最边上的点记为路边*/
	 {
        laser_short_edge_right=laser_short_x_mean[0];
		short_right_y=laser_short_y_mean[0];
		lidar_edge_pos_stru.short_right = 0;	//保存短lidar右边位置
	 }
	}


	if (laser_long_right_flag)
	{
		laser_long_edge_right=laser_long_x_mean[laser_long_right_position];
		long_right_y=laser_long_y_mean[laser_long_right_position];
		road_long[laser_long_right_position]=4;
	}
	else
	{
	 for (j=14;j>=0;j--)
       {
	     if (road_long[j]!=1)
	     {
		   laser_long_edge_right=laser_long_x_mean[j];
		   long_right_y=laser_long_y_mean[j];
		   break;
	     }
       }	
	 if (j==0)
	 {
      laser_long_edge_right=laser_long_x_mean[0];
      long_right_y=laser_long_y_mean[0];
	 }
	}



	if (laser_short_left_flag)
	{
		laser_short_edge_left=laser_short_x_mean[laser_short_left_position];
		short_left_y=laser_short_y_mean[laser_short_left_position];
		road_short[laser_short_left_position]=5;
		lidar_edge_pos_stru.short_left = laser_short_left_position;	//保存短lidar右边位置

	}
	else
	{
	 for (j=25;j<48;j++)
       {
	     if (road_short[j]!=1)
	     {
		   laser_short_edge_left=laser_short_x_mean[j];
		   short_left_y=laser_short_y_mean[j];
		   lidar_edge_pos_stru.short_left = j;	//保存短lidar右边位置
		   break;
	     }
       }
	 if (j==48)
	 {
       laser_short_edge_left=laser_short_x_mean[47];
	   short_left_y=laser_short_y_mean[47];
	   lidar_edge_pos_stru.short_left = 47;
	 }
	}


	if (laser_long_left_flag)
	{
		laser_long_edge_left=laser_long_x_mean[laser_long_left_position];
		long_left_y=laser_long_y_mean[laser_long_left_position];
		road_long[laser_long_left_position]=5;
	}
	else
	{
	 for (j=15;j<28;j++)
       {
	     if (road_long[j]!=1)
	     {
		   laser_long_edge_left=laser_long_x_mean[j];
		   long_left_y=laser_long_y_mean[j];
		   break;
	     }
       }
	 if (j==28)
	 {
       laser_long_edge_left=laser_long_x_mean[27];
	   long_left_y=laser_long_y_mean[27];
	 }
	}


/*if (laser_short_right_flag)
{
	road_short[laser_short_right_position]=4;
	for (int i=laser_short_right_position;i<24;i++)
{
	if (road_short[i]==1)
		break;
	if (road_short[i]==0)
		road_short[i]=2;
}
}

if (laser_short_left_flag)
{
	road_short[laser_short_left_position]=5;
	for (int i=laser_short_left_position;i>25;i--)
{
	if (road_short[i]==1)
		break;
	if (road_short[i]==0)
		road_short[i]=3;
}
}

if (laser_long_right_flag)
{
	road_long[laser_long_right_position]=4;
	for (int i=laser_long_right_position;i<24;i++)
{
	if (road_long[i]==1)
		break;
	if (road_long[i]==0)
		road_long[i]=2;
}
}

if (laser_long_left_flag)
{
	road_long[laser_long_left_position]=5;
	for (int i=laser_long_left_position;i>25;i--)
{
	if (road_long[i]==1)
		break;
	if (road_long[i]==0)
		road_long[i]=3;
}
}
//-------------------开始寻找路边----------------------------
laser_short_edge_right=-1;
laser_short_edge_left=-1;
laser_long_edge_right=-1;
laser_long_edge_left=-1;

for (int i=24;i>0;i--)
{
	if (road_short[i]!=1)
	{
		laser_short_edge_right=laser_short_x_mean[i];
		break;
	}
}
for (int i=25;i<48;i++)
{
	if (road_short[i]!=1)
	{
		laser_short_edge_left=laser_short_x_mean[i];
		break;
	}
}

for (int i=24;i>0;i--)
{
	if (road_long[i]!=1)
	{
		laser_long_edge_right=laser_long_x_mean[i];
		break;
	}
}
for (int i=25;i<48;i++)
{
	if (road_long[i]!=1)
	{
		laser_long_edge_left=laser_long_x_mean[i];
		break;
	}
}
*/

if (laser_short_edge_right<y_min)	/**如果路边小于最小路边阀值则使用上一次路边*/
{
	laser_short_edge_right=laser_short_edge_right_former;
}
laser_short_edge_right_former=laser_short_edge_right;

if (laser_long_edge_right<y_min)
{
	laser_long_edge_right=laser_long_edge_right_former;
}
laser_long_edge_right_former=laser_long_edge_right;

if (laser_short_edge_left>-1*y_min)
{
	laser_short_edge_left=laser_short_edge_left_former;
}
laser_short_edge_left_former=laser_short_edge_left;

if (laser_long_edge_left>-1*y_min)
{
	laser_long_edge_left=laser_long_edge_left_former;
}
laser_long_edge_left_former=laser_long_edge_left;

/**貌似滤波是一阶滤波，*/
laser_short_edge_right_filter=laser_short_edge_right_filter;
laser_short_edge_right_filter=laser_short_edge_right_filter+kg*(laser_short_edge_right-laser_short_edge_right_filter);
laser_short_edge_left_filter=laser_short_edge_left_filter;
laser_short_edge_left_filter=laser_short_edge_left_filter+kg*(laser_short_edge_left-laser_short_edge_left_filter);
laser_long_edge_right_filter=laser_long_edge_right_filter;
laser_long_edge_right_filter=laser_long_edge_right_filter+kg*(laser_long_edge_right-laser_long_edge_right_filter);
laser_long_edge_left_filter=laser_long_edge_left_filter;
laser_long_edge_left_filter=laser_long_edge_left_filter+kg*(laser_long_edge_left-laser_long_edge_left_filter);


//-----------壁障激光雷达----------------
warning_y_distance=GetIntegerProperty("warning_factor");
velocity[0] = 0.3*velocity[4]+0.3*velocity[3]+0.2*velocity[2]+0.1*velocity[1]+0.1*velocity[0];	//抖动太大了，平滑一下,以免误检测
velocity[4] = velocity[3];
velocity[3] = velocity[2];
velocity[2] = velocity[1];
velocity[1] = velocity[0];

warning_y_distance = velocity[0]*warning_y_distance;

obstacle=0;

current_time = MAPS::CurrentTime();
if(current_time - last_time > 500000)
{
	last_time = current_time;
	if((!sensor_state[0]) || (!sensor_state[1]) || (!sensor_state[2]))
	{
		emergency_sensor = 1;	//某一个激光雷达没有信号
	}
	else
	{
		emergency_sensor = 0;
	}
	sensor_state[0] = 0;
	sensor_state[1] = 0;
	sensor_state[2] = 0;
}

for (int i=0;i<240;i++)
{
 if ((laser_front_x[i]<=warning_x)&&(laser_front_x[i]>=-warning_x)&&(laser_front_y[i]<=warning_y_distance || laser_front_y[i]<=1500))
 {
	 obstacle++;
 }
}

if (obstacle>=2 || emergency_sensor)
{
	emergency=1;
}
else
{
	emergency = 0;
}

//输出四个路号,判断是否有道路边缘
	if(laser_short_right_flag)
		short_r_num = 5*laser_short_right_position+153;
	else
		short_r_num = 1000;
	if(laser_short_left_flag)
		short_l_num = 5*laser_short_left_position+152;
	else
		short_l_num = 1000;
	if(laser_long_right_flag)
		long_r_num = 5*laser_long_right_position+203;
	else
		long_r_num = 1000;
	if(laser_long_left_flag)
		long_l_num = 5*laser_long_left_position+202;
	else
		long_l_num = 1000;

//第二套方案，输出点的坐标
	//短前瞻右边路沿
	if(laser_short_edge_right==laser_short_x_mean[0])
	{
		shortrx = 0;
		shortry = 0;
	}
	else
	{
		shortrx = laser_short_edge_right_filter;
		shortry = short_right_y;
	}
	//长前瞻右边路沿
	if(laser_long_edge_right==laser_long_x_mean[0])
	{
		longrx = 0;
		longry = 0;
	}
	else
	{
		longrx = laser_long_edge_right_filter;
		longry = long_right_y;
	}
	//短前瞻左边路沿
	if(laser_short_edge_left==laser_short_x_mean[47])
	{
		shortlx = 0;
		shortly = 0;
	}
	else
	{
		shortlx = laser_short_edge_left_filter;
		shortly = short_left_y;
	}
	//长前瞻左边路沿
	if(laser_long_edge_right==laser_long_x_mean[27])
	{
		longlx = 0;
		longly = 0;
	}
	else
	{
		longlx = laser_long_edge_left_filter;
		longly = long_left_y;
	}



	//---------------------创建图像---------------------------------------


  	if(m_firsttime_image==true){
	
		IplImage model;
		model=MAPS::IplImageModel(img_width,img_length,"BGR");
		Output("Map_Image_Out").AllocOutputBufferIplImage(model);
		m_firsttime_image=false;
	}

	Wait(m_nextTimestamp);	
    //---------------------------------开始画图----------------------------------------

	for (int i=0; i<240;++i)
	{
	  img_x_temp=img_width/2+laser_short_x[i]/img_scale;
	  img_y_temp=img_length-laser_short_y[i]/img_scale;

	  if (road_short[i/5]==1)//**y距离大于阀值，且方差小于阀值，路面点->绿色
	  {
         cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),2,CV_RGB(0,255,0),1,8,0);
	  }
	   if (road_short[i/5]==0)//**y距离小于阀值，或方差大于阀值，非路面点->黑色
	  {
         cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),2,CV_RGB(0,0,0),1,8,0);
	  }
	   if ((road_short[i/5]==2)||(road_short[i/5]==4))	//**y/x方差明显时的右路边，红色
	  {
         cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),2,CV_RGB(0,0,255),1,8,0);
	  }
	     if ((road_short[i/5]==3)||(road_short[i/5]==5))//**方差明显时的左路边，蓝色
	  {
         cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),2,CV_RGB(255,0,0),1,8,0);
	  }
	}
	for (int i=0; i<140;++i)
	{
	  img_x_temp=img_width/2+laser_long_x[i]/img_scale;
	  img_y_temp=img_length-laser_long_y[i]/img_scale;

	 if (road_long[i/5]==1)
	  {
         cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),2,CV_RGB(0,255,0),1,8,0);
	  }
	   if (road_long[i/5]==0)
	  {
         cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),2,CV_RGB(0,0,0),1,8,0);
	  }
	   if ((road_long[i/5]==2)||(road_long[i/5]==4))
	  {
         cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),2,CV_RGB(0,0,255),1,8,0);
	  }
	     if ((road_long[i/5]==3)||(road_long[i/5]==5))
	  {
         cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),2,CV_RGB(255,0,0),1,8,0);
	  }

	}

      cvCircle(Map_Image,cvPoint(img_width/2+laser_short_edge_right_filter/img_scale,img_length-short_right_y/img_scale),10,CV_RGB(128,128,255),1,8,0);
	  cvCircle(Map_Image,cvPoint(img_width/2+laser_long_edge_right_filter/img_scale,img_length-long_right_y/img_scale),10,CV_RGB(128,128,255),1,8,0);
	  cvCircle(Map_Image,cvPoint(img_width/2+laser_short_edge_left_filter/img_scale,img_length-short_left_y/img_scale),10,CV_RGB(128,128,255),1,8,0);
	  cvCircle(Map_Image,cvPoint(img_width/2+laser_long_edge_left_filter/img_scale,img_length-long_left_y/img_scale),10,CV_RGB(128,128,255),1,8,0);
	
	  //画第二套方案的图形查看效果
	  cvCircle(Map_Image,cvPoint(img_width/2+shortrx/img_scale,img_length-shortry/img_scale),10,CV_RGB(0,0,255),1,8,0);
	  cvCircle(Map_Image,cvPoint(img_width/2+longrx/img_scale,img_length-longry/img_scale),10,CV_RGB(0,0,255),1,8,0);
	  cvCircle(Map_Image,cvPoint(img_width/2+shortlx/img_scale,img_length-shortly/img_scale),10,CV_RGB(0,0,255),1,8,0);
	  cvCircle(Map_Image,cvPoint(img_width/2+longlx/img_scale,img_length-longly/img_scale),10,CV_RGB(0,0,255),1,8,0);
	  //画得出的路沿点号，查看点号和计算距离的误差
	cvCircle(Map_Image,cvPoint(img_width/2+laser_short_x[short_r_num]/img_scale,img_length-(laser_short_y[short_r_num]/img_scale)),20,CV_RGB(255,0,0),1,8,0);
	cvCircle(Map_Image,cvPoint(img_width/2+laser_short_x[short_l_num]/img_scale,img_length-(laser_short_y[short_l_num]/img_scale)),15,CV_RGB(0,255,0),1,8,0);
	cvCircle(Map_Image,cvPoint(img_width/2+laser_long_x[long_r_num]/img_scale,img_length-(laser_long_y[long_r_num]/img_scale)),20,CV_RGB(255,0,0),1,8,0);
	cvCircle(Map_Image,cvPoint(img_width/2+laser_long_x[long_l_num]/img_scale,img_length-(laser_long_y[long_l_num]/img_scale)),15,CV_RGB(0,255,0),1,8,0);
	
	if (laser_front_display==1)
	{
		for (int i=0;i<240;i++)
		{
            img_x_temp=img_width/2+laser_front_x[i]/img_scale;
	        img_y_temp=img_length-laser_front_y[i]/img_scale;
			cvCircle(Map_Image,cvPoint(img_x_temp,img_y_temp),5,CV_RGB(255,0,255),1,8,0);
		}
	}

	cvInitFont(&font1,CV_FONT_HERSHEY_COMPLEX_SMALL,0.5,0.5,0.5,1,8); 
	sprintf(buffer1,"Laser_Short_Right:%d", laser_short_edge_right_filter);
	cvPutText(Map_Image, buffer1, cvPoint(310, 90), &font1, CV_RGB(0,0,0));
	sprintf(buffer1,"Laser_Short_Left:%d", laser_short_edge_left_filter);
	cvPutText(Map_Image, buffer1, cvPoint(10, 90), &font1, CV_RGB(0,0,0));
	sprintf(buffer1,"Laser_Long_Right:%d", laser_long_edge_right_filter);
	cvPutText(Map_Image, buffer1, cvPoint(310, 60), &font1, CV_RGB(0,0,0));
	sprintf(buffer1,"Laser_Long_Left:%d", laser_long_edge_left_filter);
	cvPutText(Map_Image, buffer1, cvPoint(10, 60), &font1, CV_RGB(0,0,0));


	/*画的紧急区域*/
	if (SPEEDLIMITDISPLAY==1)
	{
	cvLine(Map_Image,cvPoint(img_width/2+warning_x/img_scale,img_length-warning_y_distance/img_scale),cvPoint(img_width/2-warning_x/img_scale,img_length-warning_y_distance/img_scale),CV_RGB(0,0,0),3,8,0);
	cvLine(Map_Image,cvPoint(img_width/2+warning_x/img_scale,img_length-warning_y_distance/img_scale),cvPoint(img_width/2+warning_x/img_scale,img_length-1),CV_RGB(0,0,0),3,8,0);
 	cvLine(Map_Image,cvPoint(img_width/2-warning_x/img_scale,img_length-1),cvPoint(img_width/2-warning_x/img_scale,img_length-warning_y_distance/img_scale),CV_RGB(0,0,0),3,8,0);
	}
	/*小胖模块紧急区域*/
	cvLine(Map_Image,cvPoint(img_width/2+2300/img_scale,img_length-10000/img_scale),cvPoint(img_width/2-2000/img_scale,img_length-10000/img_scale),CV_RGB(0,0,0),3,8,0);
	cvLine(Map_Image,cvPoint(img_width/2+2300/img_scale,img_length-10000/img_scale),cvPoint(img_width/2+2300/img_scale,img_length-1),CV_RGB(0,0,0),3,8,0);
 	cvLine(Map_Image,cvPoint(img_width/2-2000/img_scale,img_length-1),cvPoint(img_width/2-2000/img_scale,img_length-10000/img_scale),CV_RGB(0,0,0),3,8,0);
	/*小胖路沿*/
	u_left = cos(((float)road_curb_left*65/130+25)*3.1415/180)*img_width/2;
	v_left = sin(((float)road_curb_left*65/130+25)*3.1415/180)*img_width/2;

	u_right = cos(((float)road_curb_right*65/130+25)*3.1415/180)*img_width/2;
	v_right = sin(((float)road_curb_right*65/130+25)*3.1415/180)*img_width/2;

//	cvLine(Map_Image,cvPoint(img_width/2,img_length-1),cvPoint(img_width/2+u_left,img_length-v_left),CV_RGB(255,0,0),3,8,0);
//	cvLine(Map_Image,cvPoint(img_width/2,img_length-1),cvPoint(img_width/2+u_right, img_length - v_right),CV_RGB(0,255,0),3,8,0);

	/*画的圆*/
	cvCircle(Map_Image,cvPoint(img_width/2,img_length),20000/img_scale,CV_RGB(0,0,255),1,8,0);

	

	
	if(MAPS::CurrentTime()>m_nextTimestamp1){
	
	Map_Image_Out_Elt=StartWriting(Output("Map_Image_Out"));	
	IplImage &oIPLImage=Map_Image_Out_Elt->IplImage();
	Map_Image_Out_Elt->Timestamp()= MAPS::CurrentTime();
	ib=(unsigned char*)(*Map_Image).imageData;
	ob=(unsigned char*)oIPLImage.imageData;
	MAPS::Memcpy(ob,ib,((*Map_Image).height)*((*Map_Image).widthStep));
	StopWriting(Map_Image_Out_Elt);
	
	m_nextTimestamp1=MAPS::CurrentTime()+GetIntegerProperty("Image_Period_us");
	}
	
	cvReleaseImage(&Map_Image);

	
	
	if(MAPS::CurrentTime()>m_nextTimestamp)
	{
	   edge_info = StartWriting(Output("road_curb"));
		edge_info->Integer(0) = short_r_num;
		edge_info->Integer(1) = short_l_num;
		edge_info->Integer(2) = long_r_num;
		edge_info->Integer(3) = long_l_num;
		edge_info->Integer(4) = emergency;
		edge_info->Integer(5) = obstacle;
		edge_info->Integer(6) = warning_y_distance;
		edge_info->Integer(7) = emergency_sensor;
		//第二套方案
		/* edge_info->Integer(0) = shortrx;
		edge_info->Integer(1) = shortry;
		edge_info->Integer(2) = shortlx;
		edge_info->Integer(3) = shortly;
		edge_info->Integer(4) = longrx;
		edge_info->Integer(5) = longry;
		edge_info->Integer(6) = longlx;
		edge_info->Integer(7) = longly;*/

	    edge_info->Timestamp() = edge_info->Timestamp();
		StopWriting(edge_info);

		edge_position = StartWriting(Output("road_edge_position"));
		lidar_r[0] = (lidar_edge_pos_stru.short_right*5+3)*13/12;
		lidar_l[0] = (lidar_edge_pos_stru.short_left*5+3)*13/12;
		lidar_r[0] = (lidar_r[4]+lidar_r[3]+lidar_r[2]+lidar_r[1]+lidar_r[0])/5;
		lidar_l[0] = (lidar_l[4]+lidar_l[3]+lidar_l[2]+lidar_l[1]+lidar_l[0])/5;
		lidar_r[4]=lidar_r[3];lidar_r[3]=lidar_r[2];lidar_r[2]=lidar_r[1];lidar_r[1]=lidar_r[0];
		lidar_l[4]=lidar_l[3];lidar_l[3]=lidar_l[2];lidar_l[2]=lidar_l[1];lidar_l[1]=lidar_l[0];
		edge_position->Integer(0) =  lidar_r[0];//0;//lidar_r[0];
		edge_position->Integer(1) =  lidar_l[0];//1000;lidar_l[0];
		StopWriting(edge_position);
	}

	m_nextTimestamp=MAPS::CurrentTime()+GetIntegerProperty("Map_Period_us");

}

void MAPSOPENCVTEST::Death()
{
    // Reports this information to the RTMaps console
    ReportInfo("OPENCVTEST: Passing through Death() method");
}
