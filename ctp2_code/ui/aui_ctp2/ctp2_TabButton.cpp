






#include "c3.h"


#include "ctp2_TabButton.h"


#include <string>


#include "aui_ldl.h"
#include "c3ui.h"
#include "ctp2_Tab.h"
#include "ctp2_TabGroup.h"


extern C3UI	*g_c3ui;


static const MBCHAR *k_CTP2_BUTTON_LDL_LAYER_LEFT		= "layerleft";
static const MBCHAR *k_CTP2_BUTTON_LDL_LAYER_RIGHT		= "layerright";
static const MBCHAR *k_CTP2_BUTTON_LDL_LAYER_ACTIVE		= "layeractive";


const sint32 ctp2_TabButton::k_CTP2_TAB_BUTTON_LAYER_FLAG_LEFT		= 64;
const sint32 ctp2_TabButton::k_CTP2_TAB_BUTTON_LAYER_FLAG_RIGHT		= 128;
const sint32 ctp2_TabButton::k_CTP2_TAB_BUTTON_LAYER_FLAG_ACTIVE	= 256;


ctp2_TabButton::ctp2_TabButton(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock,
							   ControlActionCallback *ActionFunc, void *cookie) :
aui_ImageBase(ldlBlock),
aui_TextBase(ldlBlock, NULL),
ctp2_Button(retval, id, ldlBlock, ActionFunc, cookie)
{
	
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	
	ldl_datablock *theBlock = NULL;

	
	if(theLdl->IsValid(ldlBlock)) {
		theBlock = theLdl->GetLdl()->FindDataBlock(ldlBlock);
	} else {
		
		*retval = AUI_ERRCODE_HACK;
		return;
	}

	
	for(sint32 layerIndex = 0; layerIndex < GetNumberOfLayers(); layerIndex++) {
		
		
		
		
		char layerIndexString[20];
		sprintf(layerIndexString, "%d", layerIndex);

		
		InitializeLayerFlag(theBlock, layerIndex, k_CTP2_BUTTON_LDL_LAYER_LEFT,
			k_CTP2_TAB_BUTTON_LAYER_FLAG_LEFT, layerIndexString);
		InitializeLayerFlag(theBlock, layerIndex, k_CTP2_BUTTON_LDL_LAYER_RIGHT,
			k_CTP2_TAB_BUTTON_LAYER_FLAG_RIGHT, layerIndexString);
		InitializeLayerFlag(theBlock, layerIndex, k_CTP2_BUTTON_LDL_LAYER_ACTIVE,
			k_CTP2_TAB_BUTTON_LAYER_FLAG_ACTIVE, layerIndexString);
	}
}


ctp2_TabButton::~ctp2_TabButton()
{
}



uint32 ctp2_TabButton::ShouldDraw(uint32 draw)
{
	
	if(draw & k_AUI_REGION_DRAWFLAG_UPDATE) {
		
		ctp2_Tab *tab = static_cast<ctp2_Tab*>(GetParent());
		Assert(tab);
		if(!tab)
			return 0;
		ctp2_TabGroup *tabGroup = static_cast<ctp2_TabGroup*>(tab->GetParent());

		
		Assert(tabGroup);
		if(tabGroup) {
			
			
			tabGroup->ShouldDraw(k_AUI_REGION_DRAWFLAG_OVERLAP);
		}
	}

	
	return(ctp2_Button::ShouldDraw(draw));
}


void ctp2_TabButton::SetTabMode(TabMode mode)
{
	
	switch(mode) {
	case TAB_LEFT:
		SetCurrentRenderFlags((GetCurrentRenderFlags() | k_CTP2_TAB_BUTTON_LAYER_FLAG_LEFT) &
			~(k_CTP2_TAB_BUTTON_LAYER_FLAG_RIGHT | k_CTP2_TAB_BUTTON_LAYER_FLAG_ACTIVE));
		break;
	case TAB_RIGHT:
		SetCurrentRenderFlags((GetCurrentRenderFlags() | k_CTP2_TAB_BUTTON_LAYER_FLAG_RIGHT) &
			~(k_CTP2_TAB_BUTTON_LAYER_FLAG_LEFT | k_CTP2_TAB_BUTTON_LAYER_FLAG_ACTIVE));
		break;
	case TAB_ACTIVE:
		SetCurrentRenderFlags((GetCurrentRenderFlags() | k_CTP2_TAB_BUTTON_LAYER_FLAG_ACTIVE) &
			~(k_CTP2_TAB_BUTTON_LAYER_FLAG_LEFT | k_CTP2_TAB_BUTTON_LAYER_FLAG_RIGHT));
		break;
	default:
		Assert(false);	
		break;
	}
}
