










#ifndef __AUI_SHELL_H__
#define __AUI_SHELL_H__


#include "aui_base.h"
#include "tech_wllist.h"


class aui_Screen;



class aui_Shell : public aui_Base
{
public:
	
	aui_Shell(
		AUI_ERRCODE *retval );
	virtual ~aui_Shell();

protected:
	aui_Shell() : aui_Base() {}
	AUI_ERRCODE InitCommon( void );

public:
	aui_Screen *GetCurrentScreen( void ) const { return m_curScreen; }
	virtual aui_Screen *LeaveCurrentScreen( void );

	virtual AUI_ERRCODE GotoScreen( uint32 id );
	virtual aui_Screen	*FindScreen( uint32 id ) = 0;

	virtual aui_Screen	*GoBackScreen( void );

protected:
	aui_Screen			*m_curScreen;
	tech_WLList<uint32>	*m_screenHistory;
};


#endif 
