










#ifndef __NS_IMPROVEMENTS_H__
#define __NS_IMPROVEMENTS_H__


class aui_StringTable;


class ns_Improvements;
extern ns_Improvements *g_nsImprovements;



#define k_IMPROVEMENTS_MAX 70



class ns_Improvements
{
public:
	ns_Improvements();
	virtual ~ns_Improvements();

	aui_StringTable *GetStrings( void ) const { return m_stringtable; }

private:
	aui_StringTable *m_stringtable;
};


#endif 
