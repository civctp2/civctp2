


#include "c3.h"
#include "ctpregistry.h"

BOOL ctpregistry_DoesKeyExist(void *key, MBCHAR *subKey)
{
	BOOL opened;
	HKEY newKey;

	opened = RegOpenKeyEx((HKEY)key, subKey, 
						  0,  
						  KEY_READ, 
						  &newKey) == ERROR_SUCCESS;
	if(opened) {
		RegCloseKey(newKey);
	}
	return opened;
}






BOOL ctpregistry_SetKeyValue(void *key, MBCHAR *subKey, 
							 MBCHAR *name,
							 MBCHAR *value)
{
	BOOL opened;
	HKEY theKey;
	DWORD disposition;

	opened = RegCreateKeyEx((HKEY)key,
							subKey,
							0,
							"",
							REG_OPTION_NON_VOLATILE,
							KEY_ALL_ACCESS,
							NULL,
							&theKey,
							&disposition) == ERROR_SUCCESS;
	if(opened) {
		RegSetValueEx(theKey,
					  name,
					  0,
					  REG_SZ,
					  (CONST BYTE *)value,
					  strlen(value) + 1);
		RegCloseKey(theKey);
		return TRUE;
	}
	return FALSE;
}

MBCHAR *ctpregistry_GetKeyValue(void *key,
								MBCHAR *subKey,
								MBCHAR *name)
{
	
	
	static MBCHAR valBuf[_MAX_PATH];
	BOOL opened;
	HKEY theKey;
	DWORD valueType;
	DWORD bufSize;
	opened = RegOpenKeyEx((HKEY)key, subKey, 
						  0,  
						  KEY_READ, 
						  &theKey) == ERROR_SUCCESS;
	if(opened) {
		BOOL queried;
		bufSize = _MAX_PATH;
		queried = RegQueryValueEx(theKey,
								  name,
								  0,
								  &valueType,
								  (BYTE *)valBuf,
								  &bufSize) == ERROR_SUCCESS;
		RegCloseKey(theKey);
		if(queried) {
			return valBuf;
		}
	}
	return NULL;
}
