#ifndef __NS_STRING_H__
#define __NS_STRING_H__

#define k_NS_STRING_LDL_NODATABASE		"nodatabase"

class ns_String
{
	char *m_string;
public:

	ns_String(const char *ldlblock);

	virtual ~ns_String(void);

	const char *GetString(void);
};

#endif
