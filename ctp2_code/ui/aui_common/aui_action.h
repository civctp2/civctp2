










#ifndef __AUI_ACTION_H__
#define __AUI_ACTION_H__


#include "aui_base.h"


class aui_Control;




class aui_Action : public aui_Base
{
public:
	typedef void (ActionCallback)(
		aui_Control *control,
		uint32 action,
		uint32 data );

	
	aui_Action() {}
	virtual ~aui_Action() {}

	
	
	
	virtual ActionCallback Execute {}
};


#endif 
