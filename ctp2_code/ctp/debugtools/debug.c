#ifdef _DEBUG


#include <stdio.h>


#include "DebugAssert.h"
#include "DebugCallStack.h"
#include "DebugException.h"
#include "DebugMemory.h"
#include "log.h"


void Debug_SystemCleanup (void)
{
}

void Debug_SystemRestore (void)
{
}

//
// open all debugging systems.
//

void Debug_Open (void)
{
	char name[30];

	DebugCallStack_Open();

	sprintf(name, "%s%#.3d.txt", "CivCTP_debug", 0);
	
	Log_Open ("CTP_debug.cfg", name);

	DebugAssert_Open (Debug_SystemCleanup, Debug_SystemRestore);
	DebugException_Open (Debug_SystemCleanup);
	DebugMemory_Open();
}


//
// Only close memory system ... logs asserts etc may be used in static desctructors
//
void Debug_Close (void)
{
	DebugMemory_Close();
	Log_Close();
}

#endif // def _DEBUG

