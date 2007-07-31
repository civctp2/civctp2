
#ifndef __SCOREWARN_H__
#define __SCOREWARN_H__

#include "aui_action.h"



void scorewarn_AcceptWarningCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );
void scorewarn_OkButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );
void scorewarn_CancelButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );

sint32 scorewarn_Initialize(void);
sint32 scorewarn_Cleanup(void);



class DisclaimerCloseAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};

void disclaimer_AcceptButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
sint32 disclaimer_Initialize(aui_Control::ControlActionCallback *callback);
void disclaimer_Cleanup();

#endif  
