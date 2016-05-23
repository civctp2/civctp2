#include "c3window.h"

#define TestControl(p) { Assert(AUI_NEWOK(p, errcode)) ; if (!AUI_NEWOK(p, errcode)) return (FALSE) ; }
#define RemoveControl(p) { if (p) delete p ; p = NULL ; }

extern void BlockPush(MBCHAR *path, MBCHAR *addition) ;
extern void BlockPop(MBCHAR *path) ;

void ui_TruncateString( aui_Control *control, MBCHAR *str );

MBCHAR *uiutils_ChooseLdl(MBCHAR *firstChoice, MBCHAR *fallback);




MBCHAR *uiutils_AppendBlock(MBCHAR *destString, MBCHAR *srcString1, MBCHAR *srcString2);
