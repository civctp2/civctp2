/*
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include <Types.h>
#include "types.h"

typedef struct {
	sint16	status;
	sint16	count;
} PORT;
#define PULSE	0
#define TONE	1
#define TOUCH_TONE TONE
#define DEFAULT_DIALING_METHOD TONE
#define OFF	false
#define ON	true
OSErr WriteBuffer(PORT*port, char* buffer, sint16 len);
void ReadBuffer(PORT* port, char* buffer, sint16 len);
void ClearRXBuffer(PORT* port);
void ClearTXBuffer(PORT* port);
sint32 SpaceUsedInTXBuffer(PORT* port);
void PortClose(PORT* port);
sint32 ReadChar(PORT* port);
sint32 getch(void);
void SetDtr(PORT* port, sint32 state);
OSErr FastGetPortHardware(sint16 portnum, sint32* irq, sint32* baseaddr);
OSErr FastSetPortHardware(sint16 portnum, sint32 irq, sint32 baseadr);
PORT* PortOpenGreenleafFast(sint16 portnum, sint32 baud, char parity, sint16 bits, sint16 stops);
sint32 kbhit(void);
void HMWaitForOK(sint32 wait, char* what);
sint16 HMSetEchoMode(PORT* port, sint16 mode);
sint16 HMSetVerboseMode(PORT* port, sint16 mode);
sint16 HMSendString(PORT* port, char* init);
sint16 HMSetDialingMethod(PORT* port, sint16 method);
sint16 HMDial(PORT* port, char* phonenum);
sint16 HMSetRegister(PORT* port, sint16 reg, sint16 value);
void exchange_ramp(PORT* port, sint32 Id);
