#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3_UPDATEACTION_H__
#define __C3_UPDATEACTION_H__

#include "aui_action.h"

class c3_UpdateAction;

class c3_UpdateAction : public aui_Action
{
public:
	c3_UpdateAction()
    :   aui_Action  ()
    { ; };

	virtual ~c3_UpdateAction(void);

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

	virtual c3_UpdateAction * CopyMe(void);
};


#endif
