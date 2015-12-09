////////////////////////////////
// RTMaps SDK Component
////////////////////////////////
/*
Copyright YuanWei 2014
*/
////////////////////////////////
// Purpose of this module :
// To detect the edge of the Road
////////////////////////////////

#include "maps_road_edge.h"	// Includes the header of this component
#include "cmath"
#include "cstdlib"
#include "opencv2\opencv.hpp"

#define PI 3.1415926535
#define image_with 600
#define image_height 600
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSroad_edge)
	MAPS_INPUT("Laser_Short",MAPS::FilterIntegers,MAPS::LastOrNextReader)
	MAPS_INPUT("Laser_Long",MAPS::FilterIntegers,MAPS::LastOrNextReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSroad_edge)
	MAPS_OUTPUT("Road_Edge",MAPS::Integer32,NULL,NULL,8)
	MAPS_OUTPUT("Road_Edge_Image",MAPS::IplImage,NULL,NULL,1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSroad_edge)
	//Parameters of lidars
    MAPS_PROPERTY("laser_short_pitch",10,false,true)
	MAPS_PROPERTY("laser_long_pitch",7,false,true)
	MAPS_PROPERTY("laser_short_x_movement",0,false,true)
	MAPS_PROPERTY("laser_long_x_movement",0,false,true)
	MAPS_PROPERTY("laser_short_y_movement",2360,false,true)
	MAPS_PROPERTY("laser_long_y_movement",-250,false,true)
	MAPS_PROPERTY("laser_short_h",128,false,true)
	MAPS_PROPERTY("laser_long_h",128,false,true)
	//Parameters of threshold value
	MAPS_PROPERTY("laser_short_y_th_max",12000,false,true)
	MAPS_PROPERTY("laser_short_y_th_min",6000,false,true)
	MAPS_PROPERTY("laser_short_raw_th_ang_max",128,false,true)
	MAPS_PROPERTY("laser_short_raw_th_ang_min",100,false,true)
	MAPS_PROPERTY("laser_long_y_th_max",16000,false,true)
	MAPS_PROPERTY("laser_long_y_th_min",8000,false,true)
	MAPS_PROPERTY("laser_long_raw_th_ang_max",128,false,true)
	MAPS_PROPERTY("laser_long_raw_th_ang_min",128,false,true)
	//The threshold value of the Clustering
	MAPS_PROPERTY("short_dor_c0",70,false,true)
	MAPS_PROPERTY("short_dor_a",0.5,false,true)
	MAPS_PROPERTY("short_dor_b",45,false,true)
	MAPS_PROPERTY("long_dor_c0",100,false,true)
	MAPS_PROPERTY("long_dor_a",0.5,false,true)
	MAPS_PROPERTY("long_dor_b",45,false,true)
	MAPS_PROPERTY("short_th_rate_a",0.001,false,true)
	MAPS_PROPERTY("long_th_rate_a",0.001,false,true)
	MAPS_PROPERTY("short_th_rate_y",20,false,true)
	MAPS_PROPERTY("long_th_rate_y",20,false,true)
	MAPS_PROPERTY("short_th_fit_num",10,false,true)
	MAPS_PROPERTY("long_th_fit_num",10,false,true)
	//The threshold value of the Road
	MAPS_PROPERTY("road_surface_slope",5,false,true)
	MAPS_PROPERTY("road_edge_slope",100,false,true)
	MAPS_PROPERTY("road_th_s_e_x",100,false,true)
	MAPS_PROPERTY("road_th_surface_coef",0.001,false,true)
	MAPS_PROPERTY("road_th_edge_coef",100,false,true)
	MAPS_PROPERTY("road_th_surface_edge_x",10,false,true)
	//画图的属性
	MAPS_PROPERTY("image_scale",70,false,true)
	MAPS_PROPERTY("Image_Period_us",20000,false,true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSroad_edge)
    //MAPS_ACTION("aName",MAPSroad_edge::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (road_edge) behaviour
MAPS_COMPONENT_DEFINITION(MAPSroad_edge,"road_edge","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  2, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  2, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  36, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSroad_edge::Birth()
{
	laser_short_pitch = GetIntegerProperty("laser_short_pitch");
	laser_long_pitch = GetIntegerProperty("laser_long_pitch");

	laser_short_x_movement = GetIntegerProperty("laser_short_x_movement");
	laser_long_x_movement = GetIntegerProperty("laser_long_x_movement");
	laser_short_y_movement = GetIntegerProperty("laser_short_y_movement");
	laser_long_y_movement = GetIntegerProperty("laser_long_y_movement");
	laser_short_h = GetIntegerProperty("laser_short_h");
	laser_long_h = GetIntegerProperty("laser_long_h");

	laser_short_y_th_max = GetIntegerProperty("laser_short_y_th_max");
	laser_short_y_th_min = GetIntegerProperty("laser_short_y_th_min");
	laser_short_raw_th_ang_max = GetIntegerProperty("laser_short_raw_th_ang_max");
	laser_short_raw_th_ang_min = GetIntegerProperty("laser_short_raw_th_ang_min");
	laser_long_y_th_max = GetIntegerProperty("laser_long_y_th_max");
	laser_long_y_th_min = GetIntegerProperty("laser_long_y_th_min");
	laser_long_raw_th_ang_max = GetIntegerProperty("laser_long_raw_th_ang_max");
	laser_long_raw_th_ang_min = GetIntegerProperty("laser_long_raw_th_ang_min");

	short_dor_c0 = GetIntegerProperty("short_dor_c0");
	short_dor_a = GetFloatProperty("short_dor_a");
	short_dor_b = GetIntegerProperty("short_dor_b");
	long_dor_c0 = GetIntegerProperty("long_dor_c0");
	long_dor_a = GetFloatProperty("long_dor_a");
	long_dor_b = GetIntegerProperty("long_dor_b");
	short_th_rate_a = GetFloatProperty("short_th_rate_a");
	long_th_rate_a = GetFloatProperty("long_th_rate_a");	
	short_th_rate_y = GetIntegerProperty("short_th_rate_y");
	long_th_rate_y = GetIntegerProperty("long_th_rate_y");
	short_th_fit_num = GetIntegerProperty("short_th_fit_num");
	long_th_fit_num = GetIntegerProperty("long_th_fit_num");

	road_surface_slope = GetIntegerProperty("road_surface_slope");
	road_edge_slope = GetIntegerProperty("road_edge_slope");
	road_th_s_e_x = GetIntegerProperty("road_th_s_e_x");

	road_th_surface_coef = GetFloatProperty("road_th_surface_coef");
	road_th_edge_coef = GetIntegerProperty("road_th_edge_coef");
	road_th_surface_edge_x = GetIntegerProperty("road_th_surface_edge_x");

	image_scale = GetIntegerProperty("image_scale");
	
	//Initial the variances
	for(int i=0;i<240;i++)
	{
		laser_short_raw[i] = 0;
		laser_short_raw_break[i] = 0;
		laser_short_ang[i] = 0.0;
		
	}
	for(int i=0;i<140;i++)
	{
		laser_long_raw[i] = 0;
		laser_long_raw_break[i] = 0;
		laser_long_ang[i] = 0.0;
	}
	for(int i=0;i<100;i++)
	{
		for(int j=0;j<2;j++)
		{
			short_fit_coef[i][j] = 0.0;
			long_fit_coef[i][j] = 0.0;
			line_short_se[i][j] = 0;
			line_long_se[i][j] = 0;
			short_fit_next_coef[i][j] = 0.0;
			long_fit_next_coef[i][j] = 0.0;
			line_short_next_se[i][j] = 0;
			line_long_next_se[i][j] = 0;
			line_short_combine[i][j] = 0;
			line_long_combine[i][j] = 0;
		}
	}
	for(int i=0;i<4;i++)
		dError[i] = 0.0;

	for(int i=0;i<100;i++)
	{
		short_fit_type[i] = 0;
		long_fit_type[i] = 0;
	}
	for(int i=0;i<2;i++)
	{
		short_edge_right[i] = 0;
		short_edge_left[i] = 0;
		long_edge_right[i] = 0;
		short_edge_left[i] = 0;
	}
	line_num_short = 0;
	line_num_long = 0;
	line_num_short_next = 0;
	line_num_long_next = 0;
	line_num_short_combine = 0;
	line_num_long_combine = 0;
	short_right = false;
	short_left = false;
	long_right = false;
	long_left = false;
	road_type_short = 0;
	road_type_long = 0;

	//报警参数初始化
	short_available = 0;
	short_available_count = 0;
	long_available = 0;
	long_available_count = 0;
	run_count = 0;
	
	//创建图像参数
	firsttime_image = true;
	m_nextTimestamp = MAPS::CurrentTime();
	temp_x = 0;
	temp_y = 0;
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
}

void MAPSroad_edge::Core() 
{
	Map_Image = cvCreateImage(cvSize(image_with,image_height),8,3);
    /**************************读取激光雷达原始数据*************************/
	if(true == DataAvailableInFIFO(Input("Laser_Short")))
	{
		Laser_Short_In = StartReading(Input("Laser_Short"));
		if(Laser_Short_In!=NULL)
		{
			for(int i=0;i<240;i++)
				laser_short_raw[i] = Laser_Short_In->Integer(i+150);
		}	
	}
    if(true == DataAvailableInFIFO(Input("Laser_Long")))
	{
		Laser_Long_In = StartReading(Input("Laser_Long"));
		if(Laser_Long_In!=NULL)
		{
			for(int i=0;i<140;i++)
				laser_long_raw[i] = Laser_Long_In->Integer(i+200);
		}	
	}
	short_available++;
	if(short_available>100)
		//return  ;
	//转化到X-Y平面坐标系
	for(int i=0;i<240;i++)
	{
		laser_short_x[i]=laser_short_raw[i]*sin((120-i)*PI/360)+laser_short_x_movement;
	    laser_short_y[i]=(laser_short_raw[i]*cos((120-i)*PI/360)*(cos(laser_short_pitch*PI/180)))+laser_short_y_movement;

	}

	for(int i=0;i<140;i++)
	{
		laser_long_x[i]=laser_long_raw[i]*sin((70-i)*PI/360)+laser_long_x_movement;
	    laser_long_y[i]=(laser_long_raw[i]*cos((70-i)*PI/360)*(cos(laser_long_pitch*PI/180)))+laser_long_y_movement;
	}
	
	/***********************************************************************/

	/*******************************数据预处理*****************************/
	/********************数据预处理需要用其他的算法进行重新分类*****************************/
	/*改变分类的方法，将现有的分类公示进行改进，研究一下这样的单线激光雷达需要的分类策略，只要把分类做好，后面的工作就好做**/
	//Set-Of-Interest（兴趣集）的选取,非兴趣集的数据全部置为0
	/*for(int i=0;i<240;i++)
	{
		if((laser_short_raw[i]<laser_short_y_th_min)&&(laser_short_raw[i]>laser_short_y_th_max))
			laser_short_raw[i] = 0;
		if((laser_long_raw[i]<laser_long_y_th_min)&&(laser_long_raw[i]>laser_long_y_th_max))
			laser_long_raw[i] = 0;
	}*/
	//根据y值范围去掉太原或者太近的点，对于这些点全部赋值为0,不让其参与断点判断
	for(int i=0;i<240;i++)
	{
		if((laser_short_y[i]<laser_short_y_th_min)&&(laser_short_y[i]>laser_short_y_th_max))
		{
			laser_short_raw[i] = 0;
		    laser_short_x[i] = 0;
			laser_short_y[i] = 0;
		}

	}
	for(int i=0;i<140;i++)
	{
		if((laser_long_y[i]<laser_long_y_th_min)&&(laser_long_y[i]>laser_long_y_th_max))
		{
			laser_long_raw[i] = 0;
		    laser_long_x[i] = 0;
			laser_long_y[i] = 0;
		}
	}
	//数据安全预警判断
	/*for(int i=0;i<240;i++)
	{
		if(laser_short_raw[i]==0)
		{
			short_available_count++;
			if(short_available_count>100)
			{
				short_available++;
			}
		}
	}*/
	//寻找断点，进行聚类(Clustering)处理（Data Segmentation）
	for(int i=0;i<239;i++)
	{
		int Short_DOR = 0;	
		if((laser_short_raw[i]>10)&&(laser_short_raw[i+1]>10))
		{
			int temp_p1,temp_p2;
			temp_p1 = sqrt(pow(double(laser_short_x[i]),2)+pow(double(laser_short_y[i]),2));
			temp_p2 = sqrt(pow(double(laser_short_x[i+1]),2)+pow(double(laser_short_y[i+1]),2));
			Short_DOR = short_dor_c0+min(temp_p1,temp_p2)*(tan(float(short_dor_b)*PI/180))*(sin(float(short_dor_a)*PI/(2*180)))/((cos(float(short_dor_a)*PI/(2*180)))-(sin(float(short_dor_a)*PI/(2*180))));
			if((abs(laser_short_raw[i]-laser_short_raw[i+1]))>Short_DOR)
				laser_short_raw_break[i] = 1;
		}
	}
	for(int i=0;i<139;i++)
	{		
		int Long_DOR = 0;
		if((laser_long_raw[i]>10)&&(laser_long_raw[i+1]>10))
		{
			int temp_p1,temp_p2;
			temp_p1 = sqrt(pow(double(laser_long_x[i]),2)+pow(double(laser_long_y[i]),2));
			temp_p2 = sqrt(pow(double(laser_long_x[i+1]),2)+pow(double(laser_long_y[i+1]),2));
			Long_DOR = long_dor_c0+min(temp_p1,temp_p2)*(tan(float(long_dor_b)*PI/180))*(sin(float(long_dor_a)*PI/(2*180)))/((cos(float(long_dor_a)*PI/(2*180)))-(sin(float(long_dor_a)*PI/(2*180))));
			if((abs(laser_long_raw[i]-laser_long_raw[i+1]))>Long_DOR)
				laser_long_raw_break[i] = 1;
		}
	}
	//-------------------------------------滤波处理-----------------------------------
	/*for(int i=1;i<239;i++)
	{
		if((laser_short_raw_break[i-1]==1)&&(laser_short_raw_break[i]==0)&&(laser_short_raw_break[i+1]==1))
			laser_short_raw_break[i] = 1;
	}
	for(int i=1;i<239;i++)
	{
		if((laser_long_raw_break[i-1]==1)&&(laser_long_raw_break[i]==0)&&(laser_long_raw_break[i+1]==1))
			laser_long_raw_break[i] = 1;
	}*/
	/***********************************************************************/

	/********************************线段拟合*******************************/
	/*
	部分重要变量说明：
	short_fit_type、long_fit_type:线段类型0——最原始的拟合线段
	                              线段类型1——可以合并的路面线段，合并后就是路面线段
								  线段类型2——路缘石线段候选段
								  线段类型3——路缘石线段右
								  线段类型4——路缘石线段左
	*/
	
	//---------------------------聚类区直线拟合--------------------------------
	for(int i=0;i<240;i++)
	{
		for(int j=i+1;j<240;j++)
		{
			if((laser_short_raw_break[i]==1)&&(laser_short_raw_break[j]==1))
			{
				if((j-i)>short_th_fit_num)
				{
					double *fit_temp_x = new double[j-i+1];//动态分配数组空间
					double *fit_temp_y = new double[j-i+1];
					for(int k=i,l=0;k<j+1;k++,l++)
					{
						fit_temp_x[l] =  double(laser_short_x[k]);
						fit_temp_y[l] =  double(laser_short_y[k]);
					}
					FitCurveCompute(fit_temp_x,fit_temp_y,m_dCoefFit,j-i+1,1,dError);//直线拟合
					short_fit_coef[line_num_short][0] = m_dCoefFit[0];
					short_fit_coef[line_num_short][1] = m_dCoefFit[1];
					short_fit_type[line_num_short] = 0;
					short_fit_error[line_num_short] = dError[3];
					line_short_se[line_num_short][0] = i;
					line_short_se[line_num_short][1] = j;
					line_num_short++;
					i=j;				
				}
				else
				{		
					i=j;
				}
			}
		}
	}
	for(int i=0;i<140;i++)
	{
		for(int j=i+1;j<140;j++)
		{
			if((laser_long_raw_break[i]==1)&&(laser_long_raw_break[j]==1))
			{
				if((j-i)>long_th_fit_num)
				{
					double *fit_temp_x = new double[j-i+1];//动态分配数组空间
					double *fit_temp_y = new double[j-i+1];
					for(int k=i,l=0;k<j;k++,l++)
					{
						fit_temp_x[l] =  double(laser_long_x[k]);
						fit_temp_y[l] =  double(laser_long_y[k]);
					}
					FitCurveCompute(fit_temp_x,fit_temp_y,m_dCoefFit,j-i,1,dError);//直线拟合
					long_fit_coef[line_num_long][0] = m_dCoefFit[0];
					long_fit_coef[line_num_long][1] = m_dCoefFit[1];
					long_fit_type[line_num_long] = 0;
					long_fit_error[line_num_long] = dError[3];
					line_long_se[line_num_long][0] = i;
					line_long_se[line_num_long][1] = j;
					line_num_long++;
					i=j;				
				}
				else
				{		
					i=j;
				}
			}
		}
	}
	//--------------------------聚类区直线拟合再处理(断点拟合)，找到路沿候选线段-----------
	for(int i=0;i<240;i++)
	{	
		if(laser_short_raw_break[i]==1)
		{
			int j = i;
			while(laser_short_raw_break[j+1]==1)
			{
				j++;
			}
			if((j-i)>=1)
			{
				double *fit_temp_x = new double[j-i+1];//动态分配数组空间
				double *fit_temp_y = new double[j-i+1];
				for(int k=i,l=0;k<j+1;k++,l++)
				{
					fit_temp_x[l] =  double(laser_short_x[k]);
					fit_temp_y[l] =  double(laser_short_y[k]);
				}
				FitCurveCompute(fit_temp_x,fit_temp_y,m_dCoefFit,j-i+1,1,dError);//直线拟合
				short_fit_next_coef[line_num_short_next][0] = m_dCoefFit[0];
				short_fit_next_coef[line_num_short_next][1] = m_dCoefFit[1];
				line_short_next_se[line_num_short_next][0] = i;
				line_short_next_se[line_num_short_next][1] = j;
				line_num_short_next++;
				i=j;				
			}
		}
	}
	for(int i=0;i<140;i++)
	{	
		if(laser_long_raw_break[i]==1)
		{
			int j = i;
			while(laser_long_raw_break[j+1]==1)
			{
				j++;
			}
			if((j-i)>=1)
			{
				double *fit_temp_x = new double[j-i+1];//动态分配数组空间
				double *fit_temp_y = new double[j-i+1];
				for(int k=i,l=0;k<j+1;k++,l++)
				{
					fit_temp_x[l] =  double(laser_long_x[k]);
					fit_temp_y[l] =  double(laser_long_y[k]);
				}
				FitCurveCompute(fit_temp_x,fit_temp_y,m_dCoefFit,j-i+1,1,dError);//直线拟合
				long_fit_next_coef[line_num_long_next][0] = m_dCoefFit[0];
				long_fit_next_coef[line_num_long_next][1] = m_dCoefFit[1];
				line_long_next_se[line_num_long_next][0] = i;
				line_long_next_se[line_num_long_next][1] = j;
				line_num_long_next++;
				i=j;				
			}
		}
	}

	//--------------------------------------对直线进行合并操作--------------------------------
	for(int i=0,k=0;i<line_num_short;i++)
		for(int j=i+1;j<line_num_short;j++)
			if((abs(short_fit_coef[i][1]-short_fit_coef[j][1]))<short_th_rate_a)
			{
				int temp_y_i[2],temp_y_j[2];
				temp_y_i[0] = (short_fit_coef[i][1])*(laser_short_x[(line_short_se[i][0])])+short_fit_coef[i][0];
				temp_y_i[1] = (short_fit_coef[i][1])*(laser_short_x[(line_short_se[i][1])])+short_fit_coef[i][0];
				temp_y_j[0] = (short_fit_coef[j][1])*(laser_short_x[(line_short_se[j][0])])+short_fit_coef[j][0];
				temp_y_j[1] = (short_fit_coef[j][1])*(laser_short_x[(line_short_se[j][1])])+short_fit_coef[j][0];
				if((abs((Average(temp_y_i,2))-(Average(temp_y_j,2))))<short_th_rate_y)
				{
					line_short_combine[k][0] = i;
					line_short_combine[k][1] = j;
					short_fit_type[i] = 1;
					short_fit_type[j] = 1;
					k++;
					line_num_short_combine = k;
				}
			}

	for(int i=0,k=0;i<line_num_long;i++)
		for(int j=i+1;j<line_num_long;j++)
			if((abs(long_fit_coef[i][1]-long_fit_coef[j][1]))<long_th_rate_a)
			{
				int temp_y_i[2],temp_y_j[2];
				temp_y_i[0] = (long_fit_coef[i][1])*(laser_long_x[(line_long_se[i][0])])+long_fit_coef[i][0];
				temp_y_i[1] = (long_fit_coef[i][1])*(laser_long_x[(line_long_se[i][1])])+long_fit_coef[i][0];
				temp_y_j[0] = (long_fit_coef[j][1])*(laser_long_x[(line_long_se[j][0])])+long_fit_coef[j][0];
				temp_y_j[1] = (long_fit_coef[j][1])*(laser_long_x[(line_long_se[j][1])])+long_fit_coef[j][0];
				if((abs((Average(temp_y_i,2))-(Average(temp_y_j,2))))<long_th_rate_y)
				{
					line_long_combine[k][0] = i;
					line_long_combine[k][1] = j;
					long_fit_type[i] = 1;
					long_fit_type[j] = 1;
					k++;
					line_num_long_combine = k;
				}
			}
	/******************************道路模型匹配*****************************/
	//判断是否是路沿线段,从中间开始遍历，
	//先右边
    for(int i=(line_num_short/2);i>-1;i--)
		if(abs(short_fit_coef[i][1])>road_edge_slope)
		{
			short_fit_type[i] = 2;
			if(abs((laser_short_x[line_short_se[i][1]])-(laser_short_x[line_short_se[line_short_combine[0][0]][0]]))<road_th_s_e_x)
			{
				short_fit_type[i] = 3;
				short_right = true;
			}
		}
	for(int i=(line_num_long/2);i>-1;i--)
		if(abs(long_fit_coef[i][1])>road_edge_slope)
		{
			long_fit_type[i] = 2;
			if(abs((laser_long_x[line_long_se[i][1]])-(laser_long_x[line_long_se[line_long_combine[0][0]][0]]))<road_th_s_e_x)
			{
				long_fit_type[i] = 3;
				long_right = true;
			}
		}
    //再左边
	for(int i=(line_num_short/2);i<line_num_short;i++)
		if(abs(short_fit_coef[i][1])>road_edge_slope)
		{
			short_fit_type[i] = 2;
			if(abs((laser_short_x[line_short_se[i][0]])-(laser_short_x[line_short_se[line_short_combine[0][1]][1]]))<road_th_s_e_x)
			{
				short_fit_type[i] = 3;
				short_left = true;
			}
		}
	for(int i=(line_num_long/2);i<line_num_short;i++)
		if(abs(long_fit_coef[i][1])>road_edge_slope)
		{
			long_fit_type[i] = 2;
			if(abs((laser_long_x[line_long_se[i][0]])-(laser_long_x[line_long_se[line_long_combine[0][1]][1]]))<road_th_s_e_x)
			{
				long_fit_type[i] = 3;
				long_left = true;
			}
		}
    //道路模型匹配
	if(short_right&&short_left)
		road_type_short = 1;
	else if((!short_right)&&(!short_left))
		road_type_short = 2;
	else if((short_right==true)&&(short_left==false))
		road_type_short = 3;
	else
		road_type_short = 4;

	if(long_right&&long_left)
		road_type_long = 1;
	else if((!long_right)&&(!long_left))
		road_type_long = 2;
	else if((long_right==true)&&(long_left==false))
		road_type_long = 3;
	else
		road_type_long = 4;

	//获取当前找到的道路边缘的初始值坐标
	//短前瞻雷达
	if(road_type_short==1)
	{
		short_edge_right[0] = laser_short_x[line_short_se[line_short_combine[0][0]][0]];
		short_edge_right[1] = laser_short_y[line_short_se[line_short_combine[0][0]][0]];
		short_edge_left[0] = laser_short_x[line_short_se[line_short_combine[0][1]][1]];
		short_edge_left[1] = laser_short_y[line_short_se[line_short_combine[0][1]][1]];
	}
	else if(road_type_short==2)
	{
		short_edge_right[0] = 1000;
		short_edge_right[1] = 1000;
		short_edge_left[0] = 1000;
		short_edge_left[1] = 1000;
	}
	else if(road_type_short==3)
	{
		short_edge_right[0] = laser_short_x[line_short_se[line_short_combine[0][0]][0]];
		short_edge_right[1] = laser_short_y[line_short_se[line_short_combine[0][0]][0]];
		short_edge_left[0] = 1000;
		short_edge_left[1] = 1000;
	}
	else
	{
		short_edge_right[0] = 1000;
		short_edge_right[1] = 1000;
		short_edge_left[0] = laser_short_x[line_short_se[line_short_combine[0][1]][1]];
		short_edge_left[1] = laser_short_y[line_short_se[line_short_combine[0][1]][1]];
	}
	//长前瞻雷达
	if(road_type_long==1)
	{
		long_edge_right[0] = laser_long_x[line_long_se[line_long_combine[0][0]][0]];
		long_edge_right[1] = laser_long_y[line_long_se[line_long_combine[0][0]][0]];
		long_edge_left[0] = laser_long_x[line_long_se[line_long_combine[0][1]][1]];
		long_edge_left[1] = laser_long_y[line_long_se[line_long_combine[0][1]][1]];
	}
	else if(road_type_long==2)
	{
		long_edge_right[0] = 1000;
		long_edge_right[1] = 1000;
		long_edge_left[0] = 1000;
		long_edge_left[1] = 1000;
	}
	else if(road_type_long==3)
	{
		long_edge_right[0] = laser_long_x[line_long_se[line_long_combine[0][0]][0]];
		long_edge_right[1] = laser_long_y[line_long_se[line_long_combine[0][0]][0]];
		long_edge_left[0] = 1000;
		long_edge_left[1] = 1000;
	}
	else
	{
		long_edge_right[0] = 1000;
		long_edge_right[1] = 1000;
		long_edge_left[0] = laser_long_x[line_long_se[line_long_combine[0][1]][1]];
		long_edge_left[1] = laser_long_y[line_long_se[line_long_combine[0][1]][1]];
	}
	/********************************找路沿点*******************************/
	int short_edge_right_x_candidate = 1000;
	int short_edge_right_y_candidate = 1000;
	int short_edge_left_x_candidate = 1000;
	int short_edge_left_y_candidate = 1000;
	int long_edge_right_x_candidate = 1000;
	int long_edge_right_y_candidate = 1000;
	int long_edge_left_x_candidate = 1000;
	int long_edge_left_y_candidate = 1000;

	bool short_edge_right_candidate = false;
	bool short_edge_left_candidate = false;
	bool long_edge_right_candidate = false;
	bool long_edge_left_candidate = false;

	int road_short_surface_line_num = 0;
	int road_long_surface_line_num = 0;

	//先找符合要求的路面线
	for(int i=0;i<line_num_short;i++)
		if(abs(short_fit_coef[i][1])<road_th_surface_coef)
		{
			road_short_surface_line_num = i;
			short_edge_right_x_candidate = laser_short_x[line_short_se[i][0]];
			short_edge_right_y_candidate = laser_short_y[line_short_se[i][0]];
			short_edge_left_x_candidate = laser_short_x[line_short_se[i][1]];
			short_edge_left_y_candidate = laser_short_y[line_short_se[i][1]];
		}
	for(int i=0;i<line_num_long;i++)
		if(abs(long_fit_coef[i][1])<road_th_surface_coef)
		{
			road_long_surface_line_num = i;
			long_edge_right_x_candidate = laser_long_x[line_short_se[i][0]];
			long_edge_right_y_candidate = laser_long_y[line_short_se[i][0]];
			long_edge_left_x_candidate = laser_long_x[line_short_se[i][1]];
			long_edge_left_y_candidate = laser_long_y[line_short_se[i][1]];
		}
	//再找符合规则的路沿线
	for(int i=0;i<line_num_short_next;i++)
	{
		if(abs(short_fit_next_coef[i][1])>road_th_edge_coef)//先判断斜率是否符合要求
		{
			if((line_short_se[road_short_surface_line_num][0]-line_short_next_se[i][1])>=0)//再判断点号是否符合要求,右边
				if(abs(short_edge_right_x_candidate-laser_short_x[line_short_next_se[i][1]])<road_th_surface_edge_x)//判断距离是否符合要求，右边
				{
					short_edge_right_candidate = true;
					//short_edge_right_x_candidate = laser_short_x[line_short_next_se[i][1]];
				}
			else if((line_short_se[road_short_surface_line_num][1]-line_short_next_se[i][0])<=0)//再判断点号是否符合要求，左边
				if(abs(short_edge_left_x_candidate-laser_short_x[line_short_next_se[i][1]])<road_th_surface_edge_x)//判断距离是否符合要求，左边
				{
					short_edge_left_candidate = true;
					//short_edge_left_x_candidate = laser_short_x[line_short_next_se[i][1]];
				}
		}
	}
	for(int i=0;i<line_num_long_next;i++)
	{
		if(abs(long_fit_next_coef[i][1])>road_th_edge_coef)//先判断斜率是否符合要求
		{
			if((line_long_se[road_long_surface_line_num][0]-line_long_next_se[i][1])>=0)//再判断点号是否符合要求,右边
				if(abs(long_edge_right_x_candidate-laser_long_x[line_long_next_se[i][1]])<road_th_surface_edge_x)//判断距离是否符合要求，右边
				{
					long_edge_right_candidate = true;
					//short_edge_right_x_candidate = laser_short_x[line_short_next_se[i][1]];
				}
			else if((line_long_se[road_long_surface_line_num][1]-line_long_next_se[i][0])<=0)//再判断点号是否符合要求，左边
				if(abs(long_edge_left_x_candidate-laser_long_x[line_long_next_se[i][1]])<road_th_surface_edge_x)//判断距离是否符合要求，左边
				{
					long_edge_left_candidate = true;
					//short_edge_left_x_candidate = laser_short_x[line_short_next_se[i][1]];
				}
		}
	}
	if(short_edge_right_candidate)
	{
		short_edge_right[0] = short_edge_right_x_candidate;
		short_edge_right[1] = short_edge_right_y_candidate;
	}
	else
	{
		short_edge_right[0] = 1000;
		short_edge_right[1] = 1000;
	}
	if(short_edge_left_candidate)
	{
		short_edge_left[0] = short_edge_left_x_candidate;
		short_edge_left[1] = short_edge_left_y_candidate;
	}
	else
	{
		short_edge_left[0] = 1000;
		short_edge_left[1] = 1000;
	}
	/*short_edge_right[0] = short_edge_right_x_candidate;
	short_edge_right[1] = short_edge_right_y_candidate;
	short_edge_left[0] = short_edge_left_x_candidate;
	short_edge_left[1] = short_edge_left_y_candidate;*/
	/*****************利用最近领域滤波器对道路边界点进行跟踪****************/

	/******************************创建实时图像*****************************/
	if(firsttime_image==true)
	{
		IplImage modle;
		modle = MAPS::IplImageModel(image_with,image_height,"BGR");
		Output("Road_Edge_Image").AllocOutputBufferIplImage(modle);
		firsttime_image = false;
	}
	Wait(m_nextTimestamp);
	//------------------------------开始画图--------------------------------
	for(int i=0;i<240;i++)
	{
		temp_x = image_with/2+laser_short_x[i]/image_scale;
		temp_y = image_height/2-laser_short_y[i]/image_scale;
		if(laser_short_raw_break[i]==1)
			cvCircle(Map_Image,cvPoint(temp_x,temp_y),4,CV_RGB(255,0,0),1,8,0);
		else
			cvCircle(Map_Image,cvPoint(temp_x,temp_y),2,CV_RGB(0,255,0),1,8,0);
	}
	for(int i=0;i<140;i++)
	{
		temp_x = image_with/2+laser_long_x[i]/image_scale;
		temp_y = image_height/2-laser_long_y[i]/image_scale;
		if(laser_long_raw_break[i]==1)
			cvCircle(Map_Image,cvPoint(temp_x,temp_y),4,CV_RGB(255,0,0),1,8,0);
		else
			cvCircle(Map_Image,cvPoint(temp_x,temp_y),2,CV_RGB(0,255,0),1,8,0);
	}
	//--------------------------创建原始拟合直线-----------------------------
	for(int i=0;i<line_num_short;i++)
	{
		int line_temp_y[2],image_temp_x[2],image_temp_y[2];
		line_temp_y[0] = (short_fit_coef[i][1])*(laser_short_x[(line_short_se[i][0])])+short_fit_coef[i][0];
		line_temp_y[1] = (short_fit_coef[i][1])*(laser_short_x[(line_short_se[i][1])])+short_fit_coef[i][0];
		image_temp_x[0] = image_with/2+laser_short_x[(line_short_se[i][0])]/image_scale;
		image_temp_x[1] = image_with/2+laser_short_x[(line_short_se[i][1])]/image_scale;
		image_temp_y[0] = image_height/2-line_temp_y[0]/image_scale;
		image_temp_y[1] = image_height/2-line_temp_y[1]/image_scale;
		cvLine(Map_Image,cvPoint(image_temp_x[0],image_temp_y[0]),cvPoint(image_temp_x[1],image_temp_y[1]),CV_RGB(0,0,255),2,8,0);
	}

	for(int i=0;i<line_num_long;i++)
	{
		int line_temp_y[2],image_temp_x[2],image_temp_y[2];
		line_temp_y[0] = (long_fit_coef[i][1])*(laser_long_x[(line_short_se[i][0])])+long_fit_coef[i][0];
		line_temp_y[1] = (long_fit_coef[i][1])*(laser_long_x[(line_short_se[i][1])])+long_fit_coef[i][0];
		image_temp_x[0] = image_with/2+laser_long_x[(line_long_se[i][0])]/image_scale;
		image_temp_x[1] = image_with/2+laser_long_x[(line_long_se[i][1])]/image_scale;
		image_temp_y[0] = image_height/2-line_temp_y[0]/image_scale;
		image_temp_y[1] = image_height/2-line_temp_y[1]/image_scale;
		cvLine(Map_Image,cvPoint(image_temp_x[0],image_temp_y[0]),cvPoint(image_temp_x[1],image_temp_y[1]),CV_RGB(0,0,255),2,8,0);
	}
	//--------------------------创建后处理的直线-----------------------------
	for(int i=0;i<line_num_short_next;i++)
	{
		int line_temp_y[2],image_temp_x[2],image_temp_y[2];
		line_temp_y[0] = (short_fit_next_coef[i][1])*(laser_short_x[(line_short_next_se[i][0])])+short_fit_next_coef[i][0];
		line_temp_y[1] = (short_fit_next_coef[i][1])*(laser_short_x[(line_short_next_se[i][1])])+short_fit_next_coef[i][0];
		image_temp_x[0] = image_with/2+laser_short_x[(line_short_next_se[i][0])]/image_scale;
		image_temp_x[1] = image_with/2+laser_short_x[(line_short_next_se[i][1])]/image_scale;
		image_temp_y[0] = image_height/2-line_temp_y[0]/image_scale;
		image_temp_y[1] = image_height/2-line_temp_y[1]/image_scale;
		cvLine(Map_Image,cvPoint(image_temp_x[0],image_temp_y[0]),cvPoint(image_temp_x[1],image_temp_y[1]),CV_RGB(139,101,8),2,8,0);
	}
	for(int i=0;i<line_num_long_next;i++)
	{
		int line_temp_y[2],image_temp_x[2],image_temp_y[2];
		line_temp_y[0] = (long_fit_next_coef[i][1])*(laser_long_x[(line_long_next_se[i][0])])+long_fit_next_coef[i][0];
		line_temp_y[1] = (long_fit_next_coef[i][1])*(laser_long_x[(line_long_next_se[i][1])])+long_fit_next_coef[i][0];
		image_temp_x[0] = image_with/2+laser_long_x[(line_long_next_se[i][0])]/image_scale;
		image_temp_x[1] = image_with/2+laser_long_x[(line_long_next_se[i][1])]/image_scale;
		image_temp_y[0] = image_height/2-line_temp_y[0]/image_scale;
		image_temp_y[1] = image_height/2-line_temp_y[1]/image_scale;
		cvLine(Map_Image,cvPoint(image_temp_x[0],image_temp_y[0]),cvPoint(image_temp_x[1],image_temp_y[1]),CV_RGB(139,101,8),2,8,0);
	}

	//--------------------------创建合并的直线-----------------------------
	for(int i=0;i<line_num_short_combine;i++)
	{
		int line_temp_y[2],image_temp_x[2],image_temp_y[2];
		line_temp_y[0] = (short_fit_coef[i][1])*(laser_short_x[(line_short_se[line_short_combine[i][0]][0])])+short_fit_coef[line_short_combine[i][0]][0];
		line_temp_y[1] = (short_fit_coef[i][1])*(laser_short_x[(line_short_se[line_short_combine[i][1]][1])])+short_fit_coef[line_short_combine[i][1]][0];
		image_temp_x[0] = image_with/2+laser_short_x[(line_short_se[i][0])]/image_scale;
		image_temp_x[1] = image_with/2+laser_short_x[(line_short_se[i][1])]/image_scale;
		image_temp_y[0] = image_height/2-line_temp_y[0]/image_scale;
		image_temp_y[1] = image_height/2-line_temp_y[1]/image_scale;
		cvLine(Map_Image,cvPoint(image_temp_x[0],image_temp_y[0]),cvPoint(image_temp_x[1],image_temp_y[1]),CV_RGB(255,100,50),4,8,0);
	}
	for(int i=0;i<line_num_long_combine;i++)
	{
		int line_temp_y[2],image_temp_x[2],image_temp_y[2];
		line_temp_y[0] = (long_fit_coef[i][1])*(laser_long_x[(line_long_se[line_long_combine[i][0]][0])])+long_fit_coef[line_long_combine[i][0]][0];
		line_temp_y[1] = (long_fit_coef[i][1])*(laser_long_x[(line_long_se[line_long_combine[i][1]][1])])+long_fit_coef[line_long_combine[i][1]][0];
		image_temp_x[0] = image_with/2+laser_long_x[(line_long_se[i][0])]/image_scale;
		image_temp_x[1] = image_with/2+laser_long_x[(line_long_se[i][1])]/image_scale;
		image_temp_y[0] = image_height/2-line_temp_y[0]/image_scale;
		image_temp_y[1] = image_height/2-line_temp_y[1]/image_scale;
		cvLine(Map_Image,cvPoint(image_temp_x[0],image_temp_y[0]),cvPoint(image_temp_x[1],image_temp_y[1]),CV_RGB(255,100,50),4,8,0);
	}
	//----------------------------创建路沿点(纯黄色)-----------------------------
	temp_x = image_with/2+short_edge_right[0]/image_scale;
	temp_y = image_height/2-short_edge_right[1]/image_scale;
	cvCircle(Map_Image,cvPoint(temp_x,temp_y),6,CV_RGB(255,255,0),1,8,0);
	                                
	temp_x = image_with/2+short_edge_left[0]/image_scale;
	temp_y = image_height/2-short_edge_left[1]/image_scale;
	cvCircle(Map_Image,cvPoint(temp_x,temp_y),6,CV_RGB(255,255,0),1,8,0);

	temp_x = image_with/2+long_edge_right[0]/image_scale;
	temp_y = image_height/2-long_edge_right[1]/image_scale;
	cvCircle(Map_Image,cvPoint(temp_x,temp_y),6,CV_RGB(255,255,0),1,8,0);

	temp_x = image_with/2+long_edge_left[0]/image_scale;
	temp_y = image_height/2-long_edge_left[1]/image_scale;
	cvCircle(Map_Image,cvPoint(temp_x,temp_y),6,CV_RGB(255,255,0),1,8,0);

	//-------------------------------释放图像-------------------------------
	if(MAPS::CurrentTime()>m_nextTimestamp)
	{
		Map_Image_Out = StartWriting(Output("Road_Edge_Image"));
		IplImage &oIPLImage=Map_Image_Out->IplImage();
	    Map_Image_Out->Timestamp()= MAPS::CurrentTime();
		ib=(unsigned char*)(*Map_Image).imageData;
		ob=(unsigned char*)oIPLImage.imageData;
		MAPS::Memcpy(ob,ib,((*Map_Image).height)*((*Map_Image).widthStep));
		StopWriting(Map_Image_Out);
		m_nextTimestamp = MAPS::CurrentTime()+GetIntegerProperty("Image_Period_us");
	}
	cvReleaseImage(&Map_Image);

	/******************************输出可用数据*****************************/
	Road_Egde_Info = StartWriting(Output("Road_Edge"));
	Road_Egde_Info->Integer(0) = short_edge_right[0];
	Road_Egde_Info->Integer(1) = short_edge_right[1];
	Road_Egde_Info->Integer(2) = short_edge_left[0];
    Road_Egde_Info->Integer(3) = short_edge_left[1];
    Road_Egde_Info->Integer(4) = long_edge_right[0];
    Road_Egde_Info->Integer(5) = long_edge_right[1];
    Road_Egde_Info->Integer(6) = long_edge_left[0];
	Road_Egde_Info->Integer(7) = long_edge_left[1];
	StopWriting(Road_Egde_Info);
    
	/*****************************对重要的数据进行置位*****************************/
	for(int i=0;i<240;i++)
	{
		laser_short_raw_break[i] = 0;
	}
	for(int i=0;i<140;i++)
	{
		laser_long_raw_break[i] = 0;
	}
	for(int i=0;i<100;i++)
	{
		for(int j=0;j<2;j++)
		{
			short_fit_coef[i][j] = 0.0;
			long_fit_coef[i][j] = 0.0;
			line_short_se[i][j] = 0;
			line_long_se[i][j] = 0;
			short_fit_next_coef[i][j] = 0.0;
			long_fit_next_coef[i][j] = 0.0;
			line_short_next_se[i][j] = 0;
			line_long_next_se[i][j] = 0;
			line_short_combine[i][j] = 0;
			line_long_combine[i][j] = 0;
		}
	}
	
	for(int i=0;i<4;i++)
	{
		dError[i] = 0.0;
	}
	line_num_short = 0;
	line_num_long = 0;
	line_num_short_next = 0;
	line_num_long_next = 0;
	line_num_short_combine = 0;
	line_num_long_combine = 0;
	short_right = false;
	short_left = false;
	long_right = false;
	long_left = false;
	road_type_short = 0;
	road_type_long = 0;
	
	temp_x = 0;
	temp_y = 0;

	//报警参数再次初始化
	//short_available = 1;
	long_available = 1;

	
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSroad_edge::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}

/********************************最小二乘法函数*****************************/
void MAPSroad_edge::FitCurveCompute(double *x,double *y,double *a,int num,int n,double *err)
{
	//曲线拟合最小二乘算法，这个就直接用黄老师的代码了
	double fangchengxishux[10][10];//构造正规方程组,最大n取9次
	double fangchengxishuy[10];

	double xishu[31];//个数为3*n+1,最大取n=10
	for(int i=0;i<31;i++)
		xishu[i]=0;
    //计算内积（1，x^k）其中k=0,1,2...2n共2n+1个即xishu[0]、xishu[1]...xishu[2n]
	for(int k=0;k<2*n+1;k++)
	{
		for(int i=0;i<num;i++)
		{
			xishu[k] += pow(*(x+i),k);
		}
	}
	//计算内积（y，x^k）其中k=0,1,2...n共n+1个即xishu[(2n+1)+0]、xishu[(2n+1)+1]...xishu[(2n+1)+n]
	for(int k=0;k<n+1;k++)
	{
		for(int i=0;i<num;i++)
		{
			xishu[(2*n+1)+k] += pow(*(x+i),k)*(*(y+i));
		}
	}
	//赋方程系数给正规方程组
	for(int k=0;k<n+1;k++)//行
	{
		for(int i=0;i<n+1;i++)//列
		{
			fangchengxishux[k][i]=xishu[k+i];
		}
		fangchengxishuy[k]=xishu[2*n+1+k];
	}
	//解正规方程组,高斯主元消去法
	int line;
	int col;
	double max;
	int maxline;
	double temp[10];
	double t;
	double total;
	//按列选主元逐行消去
	for(col=0;col<n+1;col++)
	{
		max=fangchengxishux[col][col];
		maxline=col;
		for(line=col+1;line<n+1;line++)
		{
			if(fabs(fangchengxishux[line][col])>fabs(max))
			{
				max=fangchengxishux[line][col];
				maxline=line;
			}
		}
		if(maxline!=col)//不等,交换行系数
		{
			int k;
			for(k=col;k<n+1;k++)
			{
				temp[k]=fangchengxishux[col][k];
				fangchengxishux[col][k]=fangchengxishux[maxline][k];
				fangchengxishux[maxline][k]=temp[k];
			}
			temp[k]=fangchengxishuy[col];
			fangchengxishuy[col]=fangchengxishuy[maxline];
			fangchengxishuy[maxline]=temp[k];
		}
	//消去
		for(int k=col+1;k<n+1;k++)
		{
			t=fangchengxishux[k][col]/fangchengxishux[col][col];
			for(int i=col;i<n+1;i++)
				fangchengxishux[k][i]+=fangchengxishux[col][i]*(-t);
			fangchengxishuy[k]+=fangchengxishuy[col]*(-t);
		}
	}
	//回代求解
	*(a+n)=fangchengxishuy[n]/fangchengxishux[n][n];
	for(int i=n-1;i>-1;i--)
	{
		total=0;
		for(int k=i+1;k<n+1;k++)
			total+=*(a+k)*fangchengxishux[i][k];
		*(a+i)=(fangchengxishuy[i]-total)/fangchengxishux[i][i];
	}
	//误差
	//求最大值
	double calcuy=0;
	double maxy=fabs(*y);//最大工程值
	for(int i=1;i<num;i++)
	{
		if(maxy<fabs(*(y+i)))
			maxy=fabs(*(y+i));
	}
	*(err+3)=0;
	for(int i=0;i<num;i++)
	{
		calcuy=0;
		for(int k=0;k<n+1;k++)
			calcuy += *(a+k)*pow(*(x+i),k);
	//最小二乘误差
		*err += pow((*(y+i)-calcuy),2);
	//绝对误差和
		*(err+1) += fabs(*(y+i)-calcuy);
	//最大绝对误差
		if(fabs(*(y+i)-calcuy) > *(err+2))
			*(err+2) = fabs(*(y+i)-calcuy);
	//最大相对误差
		if(maxy!=0)
		{
			if(fabs(*(y+i)-calcuy)/maxy > *(err+3))
				*(err+3) = fabs(*(y+i)-calcuy)/maxy;
		}
	}
	*(err+3) *= 100;
}
/***************************************************************************/

/********************************求平均值函数*******************************/
int MAPSroad_edge::Average(int* x,int n)
{
	int sum = 0;
	int average = 0;
	for(int i=0;i<n;i++)
		sum +=x[i];
	average = sum/n;
	return average;
}
/***************************************************************************/

/********************************求平均值函数*******************************/
double MAPSroad_edge::Slope(int* start,int* stop)
{
	double slope;
	slope = (stop[1]-start[1])/(stop[0]-start[0]);
	return slope;
}
/***************************************************************************/
