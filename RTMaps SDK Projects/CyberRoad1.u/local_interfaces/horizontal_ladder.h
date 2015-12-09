////////////////////////////////
// SDK Programmer samples
////////////////////////////////

#ifndef _horizontal_H
#define _horizontal_H

// Includes maps sdk library header
#include "maps.hpp"
#define MINRADIUS 6000

// Declares a new MAPSComponent child class
class horizontal_ladder : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(horizontal_ladder)
private :
	// Place here your specific methods and attributes
	MAPSInput*	m_inputs[4];
	int inputThatAnswered;
	int j;
	int rd;
	int ki;

	int exRangel;
	int exRanger;

	double DesignedRadius;
	int DesignedKi;
	int RestictedKi;
	double RestictedRadius;
	int minDistance;
	int position;

	int dectrange;

	int ArcRoadL;
	int ArcRoadR;
	int kiL;
	int kiR;

	int SOEXN[401];
	int NS;
	int ThSX;
	int ThS;
	int ThNS;
	int ThL0;	//搜索区域的阀值最大值
	int ThL1;
	int CarWidthL;
	int CarWidthR;
	int RoadL;
	int RoadR;
	int check_flag;
	double last_angle;
	int ThL_search[4];	//设置搜索区域的阀值
	int ThL_search_index;
};

#endif
