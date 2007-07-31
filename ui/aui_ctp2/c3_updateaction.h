

#pragma once

#ifndef __C3_UPDATEACTION_H__
#define __C3_UPDATEACTION_H__

#include "aui_action.h"

class c3_UpdateAction;



class c3_UpdateAction : public aui_Action
{
public:
	c3_UpdateAction();

	virtual ~c3_UpdateAction();

	virtual c3_UpdateAction *CopyMe(void);

	virtual ActionCallback Execute;
};



#endif