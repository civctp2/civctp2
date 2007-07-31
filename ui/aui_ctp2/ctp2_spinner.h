











#pragma once
#ifndef CTP2_SPINNER_H__
#define CTP2_SPINNER_H__

#include "aui_ranger.h"


class ctp2_Spinner : public aui_Ranger
{
public:
	
	ctp2_Spinner(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

protected:
	ctp2_Spinner() : aui_Ranger() {}

public:
	static void ActionCallback(aui_Control *control, uint32 state, uint32 data, void *cookie);

	void SetDisplay();

	AUI_ERRCODE SetSpinnerCallback(ControlActionCallback *actionFunc, void *cookie) {
		m_spinnerActionFunc = actionFunc;
		m_spinnerCookie = cookie;
		return AUI_ERRCODE_OK;
	}

protected:
	ControlActionCallback	*m_spinnerActionFunc;
	void					*m_spinnerCookie;
};


#endif 
