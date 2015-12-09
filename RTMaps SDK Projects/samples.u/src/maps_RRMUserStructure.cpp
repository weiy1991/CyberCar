////////////////////////////////
// RTMaps SDK Sample Record/Replay method.
////////////////////////////////

/* Chapter 9 - Record/Replay methods
 * This section demonstrates how to write a new record/replay method.
 * When the package registers into RTMaps, the new record/replay method will
 * be available to record data in a certain format with the Recorder component,
 * then the Player component can use it to replay the data.
*/

/*! \ingroup Chapter9
 * \file
 * \brief RRMUserStructure
 *	This a very simple record/replay method recording user defined structure (see Chapter 5).
 *	Components can exchange data of any type, as soon as these do not contain 
 *	any code or pointer. User may experience a lot of problems if they try to exchange 
 *	objects instead of structured data.
 */

#include "maps_RRMUserStructure.h"
#include "UserDefinitions.h"
#include <cstdlib>

// Properties definition
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSRRMUserStructure)
MAPS_END_PROPERTIES_DEFINITION

// Actions definition
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSRRMUserStructure)
MAPS_END_ACTIONS_DEFINITION

// RRM definition
MAPS_RECORD_REPLAY_METHOD_DEFINITION(MAPSRRMUserStructure,"RRMUserStructure","1.0",
				     MAPSFilterMyNewStructure,false,false,0,0)

////////////////////////////////////////////////////////////////////////////////////
// Record specific implementation
/////////////////////////////////

void MAPSRRMUserStructure::BirthRecord(void)
{
}

void MAPSRRMUserStructure::DeathRecord(void)
{
}

// Hint() method : is called each time a data has to be recorded (IOElt) in the .rec
// file. Put the data to record in the returned string
const char* MAPSRRMUserStructure::Hint(MAPSIOElt& IOElt)
{
	// Get a reference to the data to be written in the .rec file
	MyNewStructure &myStruct=*static_cast<MyNewStructure *>(IOElt.Data());
	// Create a temp string
	MAPSStreamedString str;
	// Store the value, then a blank space, and at last the message field.
	str<<myStruct.m_value<<" "<<myStruct.m_message;
	// Use hint to store the string, because hint is not a local variable (destroyed
	// when returning from this method).
	m_hint=str;
	return m_hint; // This string will be stored in the .rec file
}

// Store method : can be used to store data in a programmer-defined file format when 
// recorded an output data 
void MAPSRRMUserStructure::Store(MAPSIOElt& IOElt)
{
	// Unlock the I/O element (compulsory and very important !)
	Unlock(IOElt);
}

////////////////////////////////////////////////////////////////////////////////////
// Replay specific implementation
/////////////////////////////////

void MAPSRRMUserStructure::BirthReplay(void)
{
}

void MAPSRRMUserStructure::DeathReplay(void)
{
}

// Replay() method : is called in replay mode when the user want to replay previously 
// recorded data by this RRM. Put in IOElt the data to replay, hint contains the string 
// recorded by Hint() method in the .rec file
bool MAPSRRMUserStructure::Replay(MAPSIOElt& IOElt,int rec,const char *hint,MAPSTypeInfo& type)
{
	// Get the reference the MyNewStructure where to write the replayed data
	MyNewStructure &myStruct=*static_cast<MyNewStructure *>(IOElt.Data());
	// Find the first blank space in the hint string (' '). 
	const char *strMessage=MAPS::Strchr(hint,' ');
	// We should always have a space in replayed string (hint).
	if (strMessage==NULL)
		return(false);
	// fill the myStruct fields from the content of hint
	myStruct.m_value=MAPS::Atoi(hint);
	MAPS::Strcpy(myStruct.m_message,strMessage+1);;
	// And returns OK !
	return true;
}
