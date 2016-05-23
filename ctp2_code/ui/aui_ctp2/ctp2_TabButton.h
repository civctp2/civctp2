#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef CTP2_TAB_BUTTON_H__
#define CTP2_TAB_BUTTON_H__

#include "ctp2_button.h"

class ctp2_TabGroup;

class ctp2_TabButton : public ctp2_Button {
public:

	enum TabMode {
		TAB_LEFT,
		TAB_RIGHT,
		TAB_ACTIVE
	};

	ctp2_TabButton(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL, void *cookie = NULL);

	virtual ~ctp2_TabButton();


	virtual uint32 ShouldDraw(uint32 draw = k_AUI_REGION_DRAWFLAG_UPDATE);

private:

	static const sint32 k_CTP2_TAB_BUTTON_LAYER_FLAG_LEFT;
	static const sint32 k_CTP2_TAB_BUTTON_LAYER_FLAG_RIGHT;
	static const sint32 k_CTP2_TAB_BUTTON_LAYER_FLAG_ACTIVE;

	void SetTabMode(TabMode mode);


	friend class ctp2_TabGroup;
};

#endif
