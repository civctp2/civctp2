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

#ifndef _CTBStuff_
#define _CTBStuff_
#ifdef __cplusplus
extern "C" {
#endif
/* Prototypes for the stuff in CTBStuff.c */
Boolean InitializeCommToolBox(void);
OSErr	FindPrefs(FSSpec* where);
OSErr InitializeConnection(Boolean readPrefs, ConnHandle* connection);
int config_ctb(ConnHandle* connection);
OSErr CreateToolNameRes(StringPtr toolName);
OSErr CreateConfigRes(Ptr configPtr);
OSErr ConfigureConnection(ConnHandle* connection);
OSErr GetToolname(char* toolName);
OSErr GetConfig(char* toolName);
void CMTest(void);
void CMTest2(void);
OSErr GetData(unsigned long* value);
OSErr SetData(unsigned long value);
#define kConfigRes	5013
#define kToolNameRes	6013
#define kSerPtrRes	7013
#define kSerPtrResType	'mtSP'
#ifdef __cplusplus
}
#endif
#endif
