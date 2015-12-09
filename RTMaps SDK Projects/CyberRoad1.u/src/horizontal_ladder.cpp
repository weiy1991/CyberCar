////////////////////////////////
// RTMaps SDK Sample Component
////////////////////////////////

/*! \ingroup Chapter1 
 * \file
 * \brief multiplier_2: This module extends the multiplier1 component also provided in this section.
 * It reads integer vectors data on its input, multiplies the incoming integers
 *  by a factor specified in a property, and outputs the result as a vector of integers.
 * This component demonstrates how to :
 * - declare an output able to handle vectors of limited size (see the MAPS_OUTPUT macro)
 * - use of the MAPSIOElt::VectorSize() method
 * This component will be set to handle vectors of integers with a limited 
 * maximum size (set to 32 in this example).
 * Component multiplier3 also provided in this section
 * will demonstrate how to extend this component to handle vectors of data
 * of any size to be more generic.
 */
#define PI 3.1415926535897932384626433832795
#define MINRADIUS 6000
#include <cmath>
#include "horizontal_ladder.h"	// Includes the header of this component

// Declaration of an input called "input", which accepts connections with
// outputs of type Integer, and which has a FifoReader behaviour by default.
MAPS_BEGIN_INPUTS_DEFINITION(horizontal_ladder)
	MAPS_INPUT("RawData",MAPS::FilterInteger,MAPS::LastOrNextReader)
	MAPS_INPUT("DesignedRadius",MAPS::FilterFloat,MAPS::LastOrNextReader)
	MAPS_INPUT("isCross",MAPS::FilterFloat,MAPS::LastOrNextReader)
	MAPS_INPUT("Point10",MAPS::FilterInteger,MAPS::LastOrNextReader)
MAPS_END_INPUTS_DEFINITION

// Declaration of an output called "output" of type Integer, which will
// handle vectors of size 32 max.
MAPS_BEGIN_OUTPUTS_DEFINITION(horizontal_ladder)
	MAPS_OUTPUT("isSafe",MAPS::Integer,NULL,NULL,2)
	MAPS_OUTPUT("RestictedRadius",MAPS::Float,NULL,NULL,1)
MAPS_END_OUTPUTS_DEFINITION

// Declaration of a property called "factor" of type integer (due to the type
// of the default value), with default value = 2, and which is allowed to be 
// modified by the user during the diagram execution.
MAPS_BEGIN_PROPERTIES_DEFINITION(horizontal_ladder)
	MAPS_PROPERTY("ThSX",6000,false,true)
	MAPS_PROPERTY("ThS",8000,false,true)
	MAPS_PROPERTY("ThNS",2,false,true)
	MAPS_PROPERTY("CarWidthL",2000,false,true)
	MAPS_PROPERTY("CarWidthR",2300,false,true)
	MAPS_PROPERTY("CarWidthL_forturn",4000,false,true)
	MAPS_PROPERTY("CarWidthR_forturn",4500,false,true)
	MAPS_PROPERTY("ThL_max",20000,false,true)
	MAPS_PROPERTY("ThL_turn",8000,false,true)
	MAPS_PROPERTY("RoadL",3000,false,true)
	MAPS_PROPERTY("RoadR",2000,false,true)
MAPS_END_PROPERTIES_DEFINITION

// No action here yet.
MAPS_BEGIN_ACTIONS_DEFINITION(horizontal_ladder)
    //MAPS_ACTION("aName",MAPSmultiplier2::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Declaration of the component attributes: named "multiplier2", with
// version 1.0, default priority of 128 (medium) if the component is threaded,
// the component accepts threaded or sequential behaviours, is threaded by default
// and has 1 input, 1 output and 1 property (taken from the above declarations).
MAPS_COMPONENT_DEFINITION(horizontal_ladder,"horizontal_ladder","1.0",128,
			MAPS::Threaded|MAPS::Sequential,MAPS::Threaded,
			  4, // Nb of inputs
			  2, // Nb of outputs
			  11, // Nb of properties
			  0) // Nb of actions

void horizontal_ladder::Birth()
{
	//Initialize the _inputs member array.
	m_inputs[0] = &Input(0);
	m_inputs[1] = &Input(1);
	m_inputs[2] = &Input(2);
	m_inputs[3] = &Input(3);
	//Get the current value of the "factor" property (whose type is integer).
	ThSX = (int)GetIntegerProperty("ThSX");
	ThS = (int)GetIntegerProperty("ThS");
	ThNS = (int)GetIntegerProperty("ThNS");
	ThL0 = (int)GetIntegerProperty("ThL_max");
	ThL_search_index = 0;
	ThL_search[0] = ThL0;				//20m
	ThL_search[1] = ThL_search[0] * 4/5;//16m
	ThL_search[2] = ThL_search[0] * 3/5;//12m
	ThL_search[3] = ThL_search[0] * 2/5;//8m
	ThL1 = (int)GetIntegerProperty("ThL_turn");
	CarWidthL = (int)GetIntegerProperty("CarWidthL");
	CarWidthR = (int)GetIntegerProperty("CarWidthR");
	RoadL = (int)GetIntegerProperty("RoadL");
	RoadR = (int)GetIntegerProperty("RoadR");
	ThSX = ThSX / 2;
	DesignedRadius = 0;
	DesignedKi = 200;
	dectrange = 0;
	ArcRoadL = 0;
	ArcRoadR = 260;
}

void horizontal_ladder::Core() 
{
	MAPSIOElt* ioEltIn = StartReading(4,m_inputs, &inputThatAnswered);
	if (ioEltIn == NULL)
		return;
	ThL1 = (int)GetIntegerProperty("ThL_turn");
//	ThL0 = (int)GetIntegerProperty("ThL0");
	ThL0 = ThL_search[ThL_search_index];	//获取搜索阀值

	
	if(dectrange > 0)//转弯时离右侧要宽一些
	{
		CarWidthR = (int)GetIntegerProperty("CarWidthR_forturn");
		CarWidthL = (int)GetIntegerProperty("CarWidthL");
	}
	else if(dectrange < 0)
	{
		CarWidthR = (int)GetIntegerProperty("CarWidthR");
		CarWidthL = (int)GetIntegerProperty("CarWidthL_forturn");
	}
	else
	{
		CarWidthR = (int)GetIntegerProperty("CarWidthR");
		CarWidthL = (int)GetIntegerProperty("CarWidthL");
	}
	//First get a MAPSIOElt object in which we can write the result to be output.
	//The StartWriting function is non-blocking.


	if (inputThatAnswered == 0)
	{
		NS = 0;
		for (int i = 0; i <= 400; i++)
		{
			SOEXN[i]=0;	//细分的角度
		}
		if(dectrange>0)	//右转时，限制左边搜索
		{
			for(int i=250; i<=400; i++)
				SOEXN[i] = 1;
		}
		else if(dectrange<0)//左转时
		{
			for(int i=0; i<=150;i++)
			{
				SOEXN[i] = 1;
			}
		}
		for (int i = 0; i <= 360; i++)
		{
			rd = ioEltIn->Integer(i);
			//if (rd < ThS)
			//	if(rd * sin(abs(double(i-180))/2*PI/180) < ThSX)
			//		NS++;
			/*转弯时缩小搜索范围*/
			check_flag = 0;
			if ((dectrange==0) && (rd<ThL0))	//直行
				check_flag = 1;
			else if(dectrange<0)//左转
			{
				if((i>180 && rd<ThL1) || (i<=180 && rd<(ThL0*2/3)))
				{
					check_flag = 1;
				}
			}
			else if(dectrange>0)
			{
				if((i<180 && rd<ThL1) || (i>=180 && rd<ThL0*2/3))//右转
				{
					check_flag = 1;
				}
			}

			if(check_flag == 1)
			{

				ki = (int) ( 2400000.0 / rd * sin( double(i-180)/2*PI/180 ) );	//
				exRangel = ceil(2400000.0/rd/rd*CarWidthL);
				exRanger = ceil(2400000.0/rd/rd*CarWidthR);
				for (j = max(0,ki+200-exRangel); j <= min(400,ki+200+exRanger);j++)	//膨胀
					SOEXN[j] = 1;
			}

			//else if (rd < ThL1)
			//{
			//	ki = (int) ( 2400000.0 / rd * sin( double(i-180)/2*PI/180 ) );
			//	if (ki < 500 && ki > -500)
			/*	{
					exRangel = ceil(2400000.0/rd/rd*CarWidthL);
					exRanger = ceil(2400000.0/rd/rd*CarWidthR);
					for (j = max(0,ki+200-exRangel); j <= min(400,ki+200+exRanger);j++)
					{
						SOEXN[j] = 2;
					}
				}
			}*/
		}

		if (dectrange == 0)
		{	
			kiL = (int) ( 2400000.0 / rd * sin( double(ArcRoadL-130)/2*PI/180 ) ) + 200 - ceil(2400000.0/4000/4000*RoadL);
			kiR = (int) ( 2400000.0 / rd * sin( double(ArcRoadR-130)/2*PI/180 ) ) + 200 + ceil(2400000.0/4000/4000*RoadR);
			if (kiL < 0)
				kiL = 0;
			if (kiL > 400)
				kiL = 400;
			if (kiR < 0)
				kiR = 0;
			if (kiR > 400)
				kiR = 400;
			for (int i = 0; i < kiL;i ++)
				SOEXN[i]=3;
			for (int i = kiR; i < 400; i ++)
				SOEXN[i]=3;
		}

	}

	else if (inputThatAnswered == 1)
	{
		DesignedRadius = (double)(ioEltIn->Float(0) * 1000);
		if (DesignedRadius == 0)
		{	
			DesignedKi = 0;
		}
		else
		{
			DesignedKi = (int)(1200000 / DesignedRadius);	//半径求曲率
		}
		if (DesignedKi > 200)
			DesignedKi = 200;
		if (DesignedKi < -200)
			DesignedKi = -200;
		DesignedKi += 200;

		return;
	}
	else if (inputThatAnswered == 2)
	{
		if((int(ioEltIn->Float(0))) == 1)//转向模式
		{
			if(ioEltIn->Float(1)>0)	//左转
			{
				dectrange = -1;
			}
			else if(ioEltIn->Float(1)<0)	//右转
			{
				dectrange = 1;
			}		
			else
			{
				dectrange = 0;
			}	
		}

		else if((int(ioEltIn->Float(0))) == 2)//直行保持航向模式
		{
			if(ioEltIn->Float(1)<0)//左转
			{
				dectrange = -1;
			}
			else if(int(ioEltIn->Float(1))>0)//右转
			{
				dectrange = 1;
			}
			else
				dectrange = 0;
			
		}
		else
			dectrange = 0;

		return;
	}
	else
	{
		ArcRoadL = ioEltIn->Integer(0);
		ArcRoadR = ioEltIn->Integer(1);
		return;
	}
	MAPSIOElt* ioEltisSafe = StartWriting(Output("isSafe"));
	MAPSIOElt* ioEltRadius = StartWriting(Output("RestictedRadius"));	

	// 计算

	if (SOEXN[DesignedKi]==0)	//如果目标曲率没有障碍
	{
		ioEltisSafe->Integer(0) = 3;
		ThL_search_index = (ThL_search_index-1)<0 ? 0 : (ThL_search_index-1);//搜索范围增加
		if (DesignedKi == 200)
		{
			ioEltRadius->Float(0) = 0;
			last_angle = 0;
		}
		else
		{
			last_angle = (double)(1200 / (DesignedKi-200));	//求转向角
			ioEltRadius->Float(0) = last_angle;
		}
	}
	else
	{
		minDistance = 100000;
		for (int i = 0; i <= 400; i++)		//寻找离目标曲率最近的曲率
		{
			if(SOEXN[i] == 0 && abs(i-DesignedKi) < minDistance)
			{
				minDistance = abs(i-DesignedKi);
				position = i;
			}
		}
		if (minDistance < 400)
		{
			ioEltisSafe->Integer(0) = 2;
			ThL_search_index = (ThL_search_index-1)<0 ? 0 : (ThL_search_index-1);//搜索范围增加
			if (position == 200)
			{
				ioEltRadius->Float(0) = 0;
				last_angle = 0;
			}
			else
			{
				last_angle = (double)(1200 / (position-200));
				ioEltRadius->Float(0) = last_angle;
			}
		}
		else	
		{
			ioEltisSafe->Integer(0) = -1;	//输出错误信息表示无路可走
			ThL_search_index++;				//搜索范围变小

			ioEltRadius->Float(0) = last_angle;
		}
		/*if (dectrange == 1 || minDistance > 400)
		{
			minDistance = 100000;
			for (int i = 0; i <= 400; i++)
			{
				if(SOEXN[i] <= 1 && abs(i-DesignedKi) < minDistance)
				{
					minDistance = abs(i-DesignedKi);
					position = i;
				}
			}
			if (minDistance < 400)
			{
				ioEltisSafe->Integer(0) = 1;
				if (position == 200)
					ioEltRadius->Float(0) = 0;
				else
					ioEltRadius->Float(0) = (double)(1200 / (position-200));
			}
			else
			{
				ioEltRadius->Float(0) = 0;
				ioEltisSafe->Integer(0) = -1;
			}
		}*/
	}
//	if (NS > ThNS)
//		ioEltisSafe->Integer(1) = 0;
//	else
//		ioEltisSafe->Integer(1) = 1;
	
	//Specify the number of valid elements in the output vector (less or equal than 32).
	ioEltisSafe->VectorSize() = 2;
	ioEltRadius->VectorSize() = 1;
	/*根据可通行区域的情况，给定适当的速度，更新isSafe，前面的都不用了*/
	switch(ThL_search_index)
	{
	case 0:
		ioEltisSafe->Integer(0) = 0;	//20m左右正常最高速度 30km/s
		break;
	case 1:
		ioEltisSafe->Integer(0) = -1;	//16m左右 30*0.8 = 24
		break;
	case 2:
		ioEltisSafe->Integer(0) = -2;	//12m左右 30*0.6 = 18
		break;
	case 3:
		ioEltisSafe->Integer(0) = -3;	//8m以上 30*0.4 = 12
		break;
	default :
		ioEltisSafe->Integer(0) = -55;	//搜索范围最小也不能搜索到，你懂的:给4码的速度，怠速吧
		ThL_search_index = 3;		//置为3
		break;
	}
	ioEltisSafe->Integer(1) = ThL_search_index;	

	//Transfer the timestamp (don't forget).
	ioEltisSafe->Timestamp() = ioEltIn->Timestamp();
	ioEltRadius->Timestamp() = ioEltIn->Timestamp();

	//Writing in the ioEltOut object is complete.
	//The only thing left is to release it and make it available to downstream components :
	StopWriting(ioEltisSafe);
	StopWriting(ioEltRadius);
}

//This function is called once by the RTMaps engine when the diagram execution stops.
//Nothing to do here for this simple component.
void horizontal_ladder::Death()
{
}
