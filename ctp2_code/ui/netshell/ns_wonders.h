#ifndef __NS_WONDERS_H__
#define __NS_WONDERS_H__

class aui_StringTable;

class ns_Wonders;
extern ns_Wonders *g_nsWonders;


#define k_WONDERS_MAX 50


class ns_Wonders
{
public:
	ns_Wonders();
	virtual ~ns_Wonders();

	aui_StringTable *GetStrings( void ) const { return m_stringtable; }

private:
	aui_StringTable *m_stringtable;
};

#endif
