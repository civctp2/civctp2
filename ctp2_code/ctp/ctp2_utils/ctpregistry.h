#ifndef __CTPREGISTRY_H__
#define __CTPREGISTRY_H__





BOOL ctpregistry_DoesKeyExist(void *key, MBCHAR *subKey);
BOOL ctpregistry_SetKeyValue(void *key, MBCHAR *subKey, 
							 MBCHAR *name,
							 MBCHAR *value);
MBCHAR *ctpregistry_GetKeyValue(void *key, MBCHAR *subKey,
								MBCHAR *name);

#endif
