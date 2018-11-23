#ifndef __SCOREWARN_H__
#define __SCOREWARN_H__

#include "aui_action.h"


void scorewarn_AcceptWarningCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );
void scorewarn_OkButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );
void scorewarn_CancelButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );

sint32 scorewarn_Initialize(void);
void   scorewarn_Cleanup(void);

AUI_ACTION_BASIC(DisclaimerCloseAction);

void disclaimer_AcceptButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
sint32 disclaimer_Initialize(aui_Control::ControlActionCallback *callback);
void disclaimer_Cleanup(void);

#endif
