#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __UI_TEST_H__
#define __UI_TEST_H__

#include "aui_action.h"



class Button1Action : public aui_Action
{
public:
	Button1Action() {}
	~Button1Action() {}

	virtual ActionCallback Execute;

protected:
};


class Button2Action : public aui_Action
{
public:
	Button2Action() {}
	~Button2Action() {}

	virtual ActionCallback Execute;

protected:
};


#endif 
