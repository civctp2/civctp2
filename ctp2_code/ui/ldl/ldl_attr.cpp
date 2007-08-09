//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "ldl_file.hpp"
#include "ldl_attr.hpp"


ldl_attribute *ldl_attribute::GetCopy()
{
	ldl_attribute *newattr = NULL;
	switch(m_type) {
		case ATTRIBUTE_TYPE_BOOL: newattr = new ldl_attributeValue<bool>(this); break;
		case ATTRIBUTE_TYPE_INT:  newattr = new ldl_attributeValue<int>(this); break;
		case ATTRIBUTE_TYPE_DOUBLE: newattr = new ldl_attributeValue<double>(this); break;
		case ATTRIBUTE_TYPE_STRING: newattr = new ldl_attributeValue<char *>(this); break;
	}
	return newattr;
}

bool ldl_attribute::GetBoolValue()
{
	Assert(m_type == ATTRIBUTE_TYPE_BOOL);
	return ((ldl_attributeValue<bool> *)this)->GetValue();
}


int ldl_attribute::GetIntValue()
{
	Assert(m_type == ATTRIBUTE_TYPE_INT);
	return ((ldl_attributeValue<int> *)this)->GetValue();
}

double ldl_attribute::GetFloatValue()
{
	Assert(m_type == ATTRIBUTE_TYPE_DOUBLE);
	return ((ldl_attributeValue<double> *)this)->GetValue();
}

char *ldl_attribute::GetStringValue()
{
	Assert(m_type == ATTRIBUTE_TYPE_STRING);
	return ((ldl_attributeValue<char *> *)this)->GetValue();
}

char *ldl_attribute::GetValueText()
{
	static char buf[1024];
	switch(m_type) {
		case ATTRIBUTE_TYPE_BOOL:
			sprintf(buf, "%s", GetBoolValue() ? "true" : "false");
			break;
		case ATTRIBUTE_TYPE_INT:
			sprintf(buf, "%d", GetIntValue());
			break;
		case ATTRIBUTE_TYPE_DOUBLE:
			sprintf(buf, "%lf", GetFloatValue());
			break;
		case ATTRIBUTE_TYPE_STRING:
			sprintf(buf, "%s", GetStringValue());
			break;
	}
	return buf;
}
