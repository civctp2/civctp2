#include "c3.h"

#include "aui_ui.h"
#include "aui_ldl.h"
#include "ns_string.h"

#include "StrDB.h"

extern StringDB	*g_theStringDB;

ns_String::ns_String(const char *ldlBlock) {
	m_string = NULL;

	aui_Ldl *theLdl = g_ui->GetLdl();

	Assert( theLdl->IsValid( ldlBlock ) );

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return;

	const char *string;

	if ( block->GetBool(k_NS_STRING_LDL_NODATABASE) || (!block->GetString("text"))) {

		string = block->GetString( "text" );
	}
	else {
		string = g_theStringDB->GetNameStr( block->GetString("text") );
	}

	strcpy((m_string = new char[strlen(string) + 1]), string);
}

ns_String::~ns_String( void ) {
	delete m_string;
	m_string = NULL;
}

const char *ns_String::GetString(void) {
	return m_string;
}
