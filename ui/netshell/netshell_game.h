









#ifndef __NETSHELL_GAME_H__
#define __NETSHELL_GAME_H__


#include "aui_action.h"



#define GAMEID 1503




class DestroyInitialPlayScreenAction : public aui_Action
{
public:
	DestroyInitialPlayScreenAction() {}
	virtual ~DestroyInitialPlayScreenAction() {}

	virtual ActionCallback Execute;
};


#endif 
