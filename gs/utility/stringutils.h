
#pragma once

#ifndef __STRINGUTILS_H__
#define __STRINGUTILS_H__

class SlicContext;

#include "dbtypes.h"

#define k_MAX_INTERP_LEN 4096

void stringutils_HackColor(BOOL on);

void stringutils_Interpret(const MBCHAR *msg, SlicContext &slicObj, MBCHAR *sInterpreted);


void stringutils_SetStaticStringId(StringId & stringId, char * stringName);

#endif
