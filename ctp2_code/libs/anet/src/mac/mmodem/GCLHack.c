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
void ReadBuffer(PORT* port, char* buffer, short len)
{
}
OSErr WriteBuffer(PORT*port, char* buffer, short len)
{
	return noErr;
}
void ClearRXBuffer(PORT* port){
}
void ClearTXBuffer(PORT* port){
}
void PortClose(PORT* port){
}
void exchange_ramp(PORT* port, long Id)
{
}

#if 0 // MAD
int getch(void)
{
	return 0;
}
int kbhit(void)
{
	return 0;
}
#endif

short HMSetDialingMethod(PORT* port, short method)
{
}
int ReadChar(PORT* port)
{
	return 0;
}
void SetDtr(PORT* port, int state)
{
}
short HMDial(PORT* port, char* phonenum)
{
	return 0;
}
short HMSetRegister(PORT* port, short reg, short value)
{
	return 0;
}