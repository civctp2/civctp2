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

typedef struct {
	short	status;
	short	count;
} PORT;
#define PULSE	0
#define TONE	1
#define TOUCH_TONE TONE
#define DEFAULT_DIALING_METHOD TONE
#define OFF	false
#define ON	true
OSErr WriteBuffer(PORT*port, char* buffer, short len);
void ReadBuffer(PORT* port, char* buffer, short len);
void ClearRXBuffer(PORT* port);
void ClearTXBuffer(PORT* port);
long SpaceUsedInTXBuffer(PORT* port);
void PortClose(PORT* port);
int ReadChar(PORT* port);
int getch(void);
void SetDtr(PORT* port, int state);
OSErr FastGetPortHardware(short portnum, int* irq, int* baseaddr);
OSErr FastSetPortHardware(short portnum, int irq, int baseadr);
PORT* PortOpenGreenleafFast(short portnum, int baud, char parity, short bits, short stops);
int kbhit(void);
void HMWaitForOK(long wait, char* what);
short HMSetEchoMode(PORT* port, short mode);
short HMSetVerboseMode(PORT* port, short mode);
short HMSendString(PORT* port, char* init);
short HMSetDialingMethod(PORT* port, short method);
short HMDial(PORT* port, char* phonenum);
short HMSetRegister(PORT* port, short reg, short value);
void exchange_ramp(PORT* port, long Id);
