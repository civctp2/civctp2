

#include "c3.h"
#include "aui_control.h"
#include "c3_updateaction.h"



c3_UpdateAction::c3_UpdateAction()
{
}



c3_UpdateAction *c3_UpdateAction::CopyMe(void)
{
	c3_UpdateAction *action = new c3_UpdateAction;

	memcpy((void *)action, (void *)this, sizeof(*this));

	return action;
}



c3_UpdateAction::~c3_UpdateAction()
{
}



void c3_UpdateAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
}


