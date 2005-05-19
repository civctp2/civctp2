










#include "c3.h"
#include "aui_ui.h"
#include "aui_ldl.h"

#include "SlicEngine.h"
#include "civapp.h"

extern BOOL			g_helpMode;
extern SlicEngine	*g_slicEngine;
extern aui_UI		*g_ui;
extern CivApp		*g_civApp;


BOOL HandleGameSpecificLeftClick( void *control )
{
	BOOL handled = FALSE;

	aui_Ldl *ldl = g_ui->GetLdl();
	if ( ldl )
	{





			g_slicEngine->RunUITriggers( ldl->GetBlock( control ) );

	}

	return handled;
}


BOOL HandleGameSpecificRightClick( void *control )
{
	BOOL handled = FALSE;

	aui_Ldl *ldl = g_ui->GetLdl();
	if ( ldl )
	{
		if (g_helpMode) {

				g_slicEngine->RunHelpTriggers(ldl->GetBlock(control));
			handled = TRUE;
		} 


	}

	return handled;
}

