










#ifndef __NS_UNITS_H__
#define __NS_UNITS_H__


class aui_StringTable;


class ns_Units;
extern ns_Units *g_nsUnits;



#define k_UNITS_MAX 110



class ns_Units
{
public:
	ns_Units();
	virtual ~ns_Units();

	aui_StringTable *GetStrings( void ) const { return m_stringtable; }

	sint32 *m_noIndex;

private:
	aui_StringTable *m_stringtable;
};


#endif 
