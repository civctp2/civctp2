









#include "c3.h"

#include "aui_ui.h"
#include "aui_ldl.h"
#include "ldl_data.hpp"
#include "ns_string.h"

#include "StrDB.h"

extern StringDB	*g_theStringDB;


ns_String::ns_String( char *ldlBlock ) 
:
    m_string    (NULL)
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return;

	const char *string;

	if ( block->GetBool(k_NS_STRING_LDL_NODATABASE) || (!block->GetString("text"))) {
		
		string = block->GetString( "text" );
	}
	else {
		string = g_theStringDB->GetNameStr( block->GetString("text") );
	}

	m_string = new char[strlen(string) + 1];
    strcpy(m_string, string);
}

ns_String::~ns_String( void ) 
{
	delete [] m_string;
}

char *ns_String::GetString( void ) {
	return m_string;
}
