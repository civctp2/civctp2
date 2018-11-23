#include "c3.h"
#include "UIUtils.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_control.h"
#include "aui_bitmapfont.h"
#include "c3ui.h"

extern C3UI			*g_c3ui;

void BlockPush(MBCHAR *path, const MBCHAR *addition)
	{

	Assert(addition != NULL) ;

	Assert(addition[0] != 0) ;

	Assert(path != NULL) ;

	Assert((strlen(path)+strlen(addition)+2)<k_AUI_LDL_MAXBLOCK) ;

	Assert(addition[0] != '.') ;
	Assert(addition[strlen(addition)] != '.') ;

	if (path[0]!= '\0')
		strcat(path, ".") ;

	strcat(path, addition) ;
	}

void BlockPop(MBCHAR *path)
	{
	MBCHAR	*p ;

	Assert(path != NULL) ;

	Assert(path[0] != 0) ;
	p = strrchr(path, '.') ;

	Assert(p!=NULL) ;
	if (p==NULL)
		return ;

	*p = '\0';
	}

void ui_TruncateString( aui_Control *control, MBCHAR *str )
{

	MBCHAR name[ _MAX_PATH + 1 ];
	strncpy( name, str, _MAX_PATH );

	if ( !control->GetTextFont() )
		control->TextReloadFont();

	control->GetTextFont()->TruncateString(
		name,
		control->Width() );

	control->SetText(name);
}

const MBCHAR *
uiutils_ChooseLdl(const MBCHAR *firstChoice, const MBCHAR *fallback)
{
	if(g_c3ui->GetLdl()->IsValid(firstChoice))
		return firstChoice;

	if(g_c3ui->GetLdl()->IsValid(fallback))
		return fallback;

	return NULL;
}


MBCHAR *
uiutils_AppendBlock(MBCHAR *destString, const MBCHAR *srcString1, const MBCHAR *srcString2)
{
	sprintf(destString, "%s.%s", srcString1, srcString2);

	return destString;
}
