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

#include "GCLHack.h"
void ReadBuffer(PORT* port, char* buffer, sint16 len)
{
}
OSErr WriteBuffer(PORT*port, char* buffer, sint16 len)
{
	return noErr;
}
void ClearRXBuffer(PORT* port){
}
void ClearTXBuffer(PORT* port){
}
void PortClose(PORT* port){
}
void exchange_ramp(PORT* port, sint32 Id)
{
}

#if 0 // MAD
sint32 getch(void)
{
	return 0;
}
sint32 kbhit(void)
{
	return 0;
}
#endif

sint16 HMSetDialingMethod(PORT* port, sint16 method)
{
}
sint32 ReadChar(PORT* port)
{
	return 0;
}
void SetDtr(PORT* port, sint32 state)
{
}
sint16 HMDial(PORT* port, char* phonenum)
{
	return 0;
}
sint16 HMSetRegister(PORT* port, sint16 reg, sint16 value)
{
	return 0;
}
