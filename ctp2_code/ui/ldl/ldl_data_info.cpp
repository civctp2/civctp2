#include "c3.h"

#include "ldl_file.hpp"
#include "ldl_data.hpp"
#include "ldl_attr.hpp"

int ldl_datablock::GetInt( const char *szName )
{
	ldl_attribute *atr = GetAttribute(szName);

	if(atr)
	{
		if(atr->GetType() == ATTRIBUTE_TYPE_INT)
			return ((ldl_attributeValue<int> *)atr)->GetValue();
		else if(atr->GetType() == ATTRIBUTE_TYPE_DOUBLE)
			return (int)((ldl_attributeValue<double> *)atr)->GetValue();
	}
	return 0;
}

double ldl_datablock::GetDouble( const char *szName )
{
	ldl_attribute *atr = GetAttribute(szName);

	if(atr) {
		if(atr->GetType() == ATTRIBUTE_TYPE_DOUBLE)
			return ((ldl_attributeValue<double> *)atr)->GetValue();
	}
	return 0.0;
}

char *ldl_datablock::GetString( const char *szName )
{
	ldl_attribute *atr = GetAttribute(szName);

	if(atr)
	{
		if(atr->GetType() == ATTRIBUTE_TYPE_STRING)
			return ((ldl_attributeValue<char *> *)atr)->GetValue();
	}

	return NULL;
}

bool ldl_datablock::GetBool( const char *szName )
{
	ldl_attribute * atr = GetAttribute(szName);

	if (atr && (atr->GetType() == ATTRIBUTE_TYPE_BOOL))
	{
		return ((ldl_attributeValue<bool> *)atr)->GetValue();
	}

	return false;
}
