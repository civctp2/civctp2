#include "c3window.h"

#define TestControl(p) { Assert(AUI_NEWOK(p, errcode)) ; if (!AUI_NEWOK(p, errcode)) return (FALSE) ; }
#define RemoveControl(p) { if (p) delete p ; p = NULL ; }

extern void BlockPush(MBCHAR *path, const MBCHAR *addition);
extern void BlockPop(MBCHAR *path) ;

void ui_TruncateString(aui_Control *control, const MBCHAR *str);

const MBCHAR *uiutils_ChooseLdl(const MBCHAR *firstChoice, const MBCHAR *fallback);




MBCHAR *uiutils_AppendBlock(MBCHAR *destString, const MBCHAR *srcString1, const MBCHAR *srcString2);
