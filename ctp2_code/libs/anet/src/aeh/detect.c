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

/* Finds display, 3D system info; slightly modifed from code by Ben Siron */
#ifdef WIN32

#include <stdio.h>
#include <windows.h>
#include "detect.h"
#define MAXLEN 256

void GetPrimaryDisplayDesc(char *desc, unsigned int *len)
{
//Most reliable routine to find description of Video Card from Registry
//	Does NOT check for or check info from DirectX!
//Bonus: It's not recursive	
	HKEY hKey1, hKey2, hSubKey1, hSubKey2;
	int i, returnVal, found_it;
	DWORD valueSize, valueType, value;
   char keyName[128];

   //
   // We're gonna enumerate all keys under "HKEY_DYN_DATA\Config Manager\Enum"
   //
	if (!desc || !len) return;
	returnVal = RegOpenKeyEx(HKEY_DYN_DATA, "Config Manager\\Enum", 0, KEY_QUERY_VALUE, &hKey1);
	if ( returnVal != ERROR_SUCCESS )
		return;
   //
   // And for each, grab another key name under "HKEY_LOCAL_MACHINE\Enum"
   //
	returnVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Enum", 0, KEY_QUERY_VALUE, &hKey2);
	if ( returnVal != ERROR_SUCCESS )
		return;

   i = found_it = 0;
   returnVal = ERROR_SUCCESS;
   while ( !found_it && (returnVal != ERROR_NO_MORE_ITEMS) )
   {
      valueSize = sizeof(keyName);
      memset(keyName, 0, sizeof(keyName));
      returnVal   = RegEnumKeyEx(hKey1, i++, keyName, &valueSize, NULL, NULL, NULL, NULL);
      if ( returnVal == ERROR_SUCCESS )
      {
	      returnVal = RegOpenKeyEx(hKey1, keyName, 0, KEY_QUERY_VALUE, &hSubKey1);
         if ( returnVal != ERROR_SUCCESS )
            continue;

         //
         // Make sure the "Problem" field for this Config Magager device is 0
         //
         valueSize = sizeof(DWORD);
         valueType = REG_DWORD;
         returnVal = RegQueryValueEx(hSubKey1, "Problem", NULL, &valueType, (unsigned char *)&value, &valueSize); 
         if ( (returnVal == ERROR_SUCCESS) && (value == 0) )
         {
            //
            // Get the "HardwareKey" field and open that key under
            // "HKEY_LOCAL_MACHINE\Enum"  (hKey2)
            //
            valueSize = sizeof(keyName);
            valueType = REG_SZ;
            memset(keyName, 0, sizeof(keyName));
            returnVal = RegQueryValueEx(hSubKey1, "HardwareKey", NULL, &valueType, (unsigned char *)keyName, &valueSize); 
            if ( returnVal == ERROR_SUCCESS )
            {
               //
               // Now see if the value in the "Driver" field is for a display type
               //
	            returnVal = RegOpenKeyEx(hKey2, keyName, 0, KEY_QUERY_VALUE, &hSubKey2);
               if ( returnVal == ERROR_SUCCESS )
               {
                  valueSize = sizeof(keyName);
                  valueType = REG_SZ;
                  memset(keyName, 0, sizeof(keyName));
                  returnVal = RegQueryValueEx(hSubKey2, "Driver", NULL, &valueType, (unsigned char *)keyName, &valueSize); 
                  if ( (returnVal == ERROR_SUCCESS) && (strstr(strupr(keyName), "DISPLAY") != NULL) )
                  {
                     //
                     //  YAAAAY we found a display driver in the Config Manager with
                     //  no tagged problems, so we assume this is the active display.
                     //  Now get the description and let's blow this joint
                     //
                     valueSize = *len;
                     valueType = REG_SZ;
                     returnVal = RegQueryValueEx(hSubKey2, "DeviceDesc", NULL, &valueType, (unsigned char *)desc, &valueSize); 
                     if ( returnVal == ERROR_SUCCESS ) {
                        found_it = 1;
						*len = valueSize - 1;
					}
                  }
                  RegCloseKey(hSubKey2);
               }
            }
         }

         RegCloseKey(hSubKey2);
      }

   }  // end while

   if (!found_it) {
      *len = 0;
      *desc = '\0';
   }
   RegCloseKey(hKey2);
   RegCloseKey(hKey1);
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL FindDeviceInRegistry(LPSTR szStartKey, LPSTR szSearchStr)
{
	HKEY TopKey, CurKey; 
	DWORD dwTypeCode, dwSizeOfDataBuffer;
	char szValueBuffer[MAX_PATH];
	char szNewKeyName[MAX_PATH];
	long i;
	
	dwTypeCode = REG_SZ;
//	szDest[0] = '\0';
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szStartKey, 0, 
		KEY_ALL_ACCESS, &TopKey) == ERROR_SUCCESS)
	{
		for (i=0; 
			RegEnumKey(TopKey, i, szValueBuffer, MAX_PATH) == ERROR_SUCCESS;
			i++)
		{
			sprintf(szNewKeyName, "%s\\%s", szStartKey, szValueBuffer);
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szNewKeyName, 0, 
				KEY_ALL_ACCESS, &CurKey) == ERROR_SUCCESS)
			{
				dwSizeOfDataBuffer = MAX_PATH;
				memset(szValueBuffer, 0, MAX_PATH);
				RegQueryValueEx(CurKey, "DeviceDesc", NULL, &dwTypeCode, 
					(LPBYTE) szValueBuffer, &dwSizeOfDataBuffer);
				if (strstr(szValueBuffer, szSearchStr))
				{
					RegCloseKey(CurKey);					
					return TRUE;
				}
				else
				{
					if (FindDeviceInRegistry(szNewKeyName, szSearchStr))
					{
						RegCloseKey(CurKey);
						return TRUE;
					}
				}
				RegCloseKey(CurKey);
			}
		}
	}
	return FALSE;
}



//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL WINAPI FindRendition()
{
	char	szCardDescrip[MAXLEN];
	BOOL	result = FALSE;
	int len = MAXLEN;

	GetPrimaryDisplayDesc(szCardDescrip, &len);

	result = (strstr(szCardDescrip, "Rendition Verite 1000") != NULL);

	return result;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL WINAPI FindATI()
{
	char	szCardDescrip[MAXLEN];
	BOOL	result = FALSE;
	int len = MAXLEN;

	GetPrimaryDisplayDesc(szCardDescrip, &len);

	result = (strstr(szCardDescrip, "ATI 3D RAGE") != NULL);

	return result;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL WINAPI FindMystique()
{
	char	szCardDescrip[MAXLEN];
	BOOL	result = FALSE;
	int len = MAXLEN;

	GetPrimaryDisplayDesc(szCardDescrip, &len);

	result = (strstr(szCardDescrip, "Matrox MGA Mystique") != NULL);

	return result;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL WINAPI FindMonster3D()
{
	BOOL	result = FALSE;

	result = FindDeviceInRegistry("Enum\\PCI", "Monster 3D");

	return result;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL WINAPI FindS3ViRGE()
{
	char	szCardDescrip[MAXLEN];
	BOOL	result = FALSE;
	int len = MAXLEN;

	GetPrimaryDisplayDesc(szCardDescrip, &len);

	result = (strstr(szCardDescrip, "S3 ViRGE") != NULL);

	return result;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL WINAPI FindSTB()
{
	char	szCardDescrip[MAXLEN];
	BOOL	result = FALSE;
	int len = MAXLEN;

	GetPrimaryDisplayDesc(szCardDescrip, &len);

	result = (strstr(szCardDescrip, "STB Bitro 3D") != NULL);

	return result;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL WINAPI FindOrchid()
{
	BOOL	result = FALSE;

	result = FindDeviceInRegistry("Enum\\PCI", "Righteous 3D");

	return result;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL WINAPI FindPowerVR()
{
	BOOL	result = FALSE;

	result = FindDeviceInRegistry("Enum\\PCI", "Midas PCI");

	if (!result) {			//Give it another chance
		result = FindDeviceInRegistry("Enum\\PCI", "Apocalypse 3D");
	}

	return result;
}



//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void Get3DHardware(char *sz3DCard, unsigned int stringlen)
{
	char *Monst3D = "Monster 3D";
	char *Right3D = "Righteous 3D";
	char *RendVer = "Rendition Verite 1000";
	char *ATI3D   = "ATI 3D RAGE";
	char *S3Virg  = "S3ViRGE";
	char *STBBit  = "STB Bitro 3D";
	char *Midas   = "Midas PCI or Apocalypse 3D";
	char *Matrox  = "Matrox MGA Mystique";
	unsigned int len = stringlen;

#if 0
	return
		FindRendition()
	||	FindATI()
	||	FindMystique()
	||	FindMonster3D()
	||	FindS3ViRGE()
	||	FindSTB()
	||	FindOrchid()
	||	FindPowerVR();
#endif

/* this was the code fragment in the system inventory dialog box to display the 3D card detected.
	it's not too sophisticated... */

	if (!sz3DCard || !len) return;
	if (FindMonster3D()) {
		if (len > strlen(Monst3D)) len = strlen(Monst3D) + 1;
		strncpy(sz3DCard, Monst3D, len - 1);
	} else if (FindOrchid()) {
		if (len > strlen(Right3D)) len = strlen(Right3D) + 1;
		strncpy(sz3DCard, Right3D, len - 1);
	} else if (FindRendition()) {
		if (len > strlen(RendVer)) len = strlen(RendVer) + 1;
		strncpy(sz3DCard, RendVer, len - 1);
	} else if (FindATI())	{
		if (len > strlen(ATI3D)) len = strlen(ATI3D) + 1;
		strncpy(sz3DCard, ATI3D, len - 1);
	} else if (FindS3ViRGE())	{
		if (len > strlen(S3Virg)) len = strlen(S3Virg) + 1;
		strncpy(sz3DCard, S3Virg, len - 1);
	} else if (FindSTB())	{
		if (len > strlen(STBBit)) len = strlen(STBBit) + 1;
		strncpy(sz3DCard, STBBit, len - 1);
	} else if (FindPowerVR())	{
		if (len > strlen(Midas)) len = strlen(Midas) + 1;
		strncpy(sz3DCard, Midas, len - 1);
	} else if (FindMystique()) {
		if (len > strlen(Matrox)) len = strlen(Matrox) + 1;
		strncpy(sz3DCard, Matrox, len - 1);
	} else {
		len = 1;
	}
	sz3DCard[len - 1] = '\0';
}

#endif
