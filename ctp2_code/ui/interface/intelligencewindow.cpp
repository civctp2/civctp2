//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Intelligence window (diplomacy subwindow)
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Update the state of the embargo and war buttons after confirmation.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Moved cleanup of statics into the the cleanup method. (Sep 14th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "intelligencewindow.h"
#include "aui.h"
#include "aui_ldl.h"
#include "diplomacywindow.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_Window.h"
#include "ctp2_button.h"
#include "ctp2_Static.h"
#include "SelItem.h"
#include "aui_blitter.h"
#include "pixelutils.h"
#include "colorset.h"
#include "aui_stringtable.h"
#include "aui_tipwindow.h"
#include "ctp2_hypertextbox.h"

#include "c3ui.h"

#include "player.h"
#include "Civilisation.h"
#include "diplomattypes.h"
#include "Diplomat.h"
#include "mapanalysis.h"
#include "StrDB.h"
#include "DiplomacyProposalRecord.h"
#include "AgreementMatrix.h"
#include "diplomacyutil.h"
#include "dipwizard.h"

#include "SlicObject.h"
#include "stringutils.h"
#include "MessageBoxDialog.h"

#include "DiplomacyDetails.h"
#include "network.h"
#include "net_action.h"

static IntelligenceWindow   *s_intelligenceWindow = NULL;
static MBCHAR               *s_intelligenceBlock = "IntelligenceWindow";
static MBCHAR               *s_intelligenceAdviceBlock = "IntelligenceAdvice";
ctp2_ListBox                *IntelligenceWindow::sm_list = NULL;

extern C3UI                 *g_c3ui;

extern ColorSet             *g_colorSet;

aui_StringTable             *IntelligenceWindow::sm_strengthImages = NULL;
aui_StringTable             *IntelligenceWindow::sm_embassyImages = NULL;

ctp2_Window                 *IntelligenceWindow::sm_showTreatyDetail = NULL;


#define k_INT_FLAG_COL      0
#define k_INT_NATION_COL    1
#define k_INT_REGARD_COL    2
#define k_INT_STRENGTH_COL  3
#define k_INT_EMBASSY_COL   4
#define k_INT_TREATIES_COL  5

#define k_WEAK_STRENGTH   -50
#define k_EQUAL_STRENGTH    0
#define k_STRONG_STRENGTH  50

IntelligenceWindow::IntelligenceWindow(AUI_ERRCODE *err)
{
	*err = AUI_ERRCODE_OK;

	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_intelligenceBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_adviceWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_intelligenceAdviceBlock);
	Assert(m_adviceWindow);

	if(m_adviceWindow) {
		m_window->AddDockedWindow(m_adviceWindow);
		m_adviceWindow->SetDock(m_window);
	}

	sm_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_intelligenceBlock, "List");

	aui_Ldl::SetActionFuncAndCookie(s_intelligenceBlock, "CloseButton", Close, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_intelligenceBlock, "NegotiationButton", Negotiations, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_intelligenceBlock, "ShowHistoryButton", Advice, NULL);

	if(*err == AUI_ERRCODE_OK) {
		InitImageTables();
	}



}

IntelligenceWindow::~IntelligenceWindow()
{
	aui_Ldl::DeleteHierarchyFromRoot(s_intelligenceBlock);
	m_window = NULL;

	aui_Ldl::DeleteHierarchyFromRoot(s_intelligenceAdviceBlock);
	m_adviceWindow = NULL;

}

AUI_ERRCODE IntelligenceWindow::Initialize()
{
	if(s_intelligenceWindow) {
		return AUI_ERRCODE_OK;
	}

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_intelligenceWindow = new IntelligenceWindow(&err);
	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE IntelligenceWindow::Cleanup()
{
	if(s_intelligenceWindow) {
		Hide();

		delete s_intelligenceWindow;
		s_intelligenceWindow = NULL;
	}

	if(sm_showTreatyDetail) {
		if(g_c3ui) {
			g_c3ui->RemoveWindow(sm_showTreatyDetail->Id());
		}

		aui_Ldl::DeleteHierarchyFromRoot("IntelTreatyDetail");
		sm_showTreatyDetail = NULL;
	}

	if(sm_strengthImages) {
		delete sm_strengthImages;
		sm_strengthImages = NULL;
	}

	if(sm_embassyImages) {
		delete sm_embassyImages;
		sm_embassyImages = NULL;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE IntelligenceWindow::Display()
{
	if(!s_intelligenceWindow)
		Initialize();

	Assert(s_intelligenceWindow);
	if(!s_intelligenceWindow) {
		return AUI_ERRCODE_HACK;
	}

	AUI_ERRCODE err = AUI_ERRCODE_INVALIDPARAM;
	Assert(s_intelligenceWindow->m_window);
	if(s_intelligenceWindow->m_window) {
		err = g_c3ui->AddWindow(s_intelligenceWindow->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK) {
			err = s_intelligenceWindow->m_window->Show();
		}

		if(s_intelligenceWindow->m_adviceWindow) {
			err = g_c3ui->AddWindow(s_intelligenceWindow->m_adviceWindow);
			if(err == AUI_ERRCODE_OK) {
				err = s_intelligenceWindow->m_adviceWindow->Show();
			}
		}

		Update(s_intelligenceWindow->sm_list);
	}

	return err;
}

AUI_ERRCODE IntelligenceWindow::Hide()
{
	if(!s_intelligenceWindow) {
		return AUI_ERRCODE_OK;
	}

	Assert(s_intelligenceWindow->m_window);
	if(!s_intelligenceWindow->m_window) {
		return AUI_ERRCODE_INVALIDPARAM;
	}

	if(s_intelligenceWindow->m_adviceWindow) {
		g_c3ui->RemoveWindow(s_intelligenceWindow->m_adviceWindow->Id());
	}

	return g_c3ui->RemoveWindow(s_intelligenceWindow->m_window->Id());
}

void IntelligenceWindow::SetRegardTip(MBCHAR *buf, const sint32 player, const sint32 foreigner)
{
	const MBCHAR *regard_str;
	const MBCHAR *trust_str;

	sint32 regard = Diplomat::GetDiplomat(player).GetPublicRegard(foreigner);
	if (regard <= HOTWAR_REGARD)
		regard_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_REG_HOTWAR_BUTTON");
	else if (regard <= COLDWAR_REGARD)
		regard_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_REG_COLDWAR_BUTTON");
	else if (regard >= ALLIED_REGARD)
		regard_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_REG_ALLIED_BUTTON");
	else if (regard >= FRIEND_REGARD)
		regard_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_REG_FRIEND_BUTTON");
	else
		regard_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_REG_NEUTRAL_BUTTON");

	sint32 trust = Diplomat::GetDiplomat(player).GetTrust(foreigner);
	if (trust <= HOTWAR_REGARD)
		trust_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_TRUST_HOTWAR_BUTTON");
	else if (trust <= COLDWAR_REGARD)
		trust_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_TRUST_COLDWAR_BUTTON");
	else if (trust >= ALLIED_REGARD)
		trust_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_TRUST_ALLIED_BUTTON");
	else if (trust >= FRIEND_REGARD)
		trust_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_TRUST_FRIEND_BUTTON");
	else
		trust_str = g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_TRUST_NEUTRAL_BUTTON");

	sprintf(buf, "%s%s", regard_str, trust_str);
}

void IntelligenceWindow::Update(ctp2_ListBox *theList)
{
	ctp2_ListItem *item;

	sint32 p;
	Player *visPl = g_player[g_selected_item->GetVisiblePlayer()];
	Assert(visPl);
	if(!visPl) return;

	theList->Clear();
	theList->SetAbsorbancy(FALSE);
	sm_list = theList;

	for(p = 1; p < k_MAX_PLAYERS; p++) {
		if(p == g_selected_item->GetVisiblePlayer()) continue;
		if(!visPl->HasContactWith(p)) continue;

		
		item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("IntelligenceListItem");
		
		Assert(item);
		if(!item) break;

		ctp2_Static *child = NULL;
		MBCHAR buf[k_MAX_NAME_LEN];
		Civilisation civ = *g_player[p]->m_civilisation;

		if((child = (ctp2_Static *)item->GetChildByIndex(k_INT_FLAG_COL))) {
			child->SetDrawCallbackAndCookie(DrawPlayerFlag, (void *)p, false);
			child->SetActionFuncAndCookie(SelectItem, (void *)item);
		}
		if((child = (ctp2_Static *)item->GetChildByIndex(k_INT_NATION_COL))) {
			civ->GetCountryName(buf);
			child->SetText(buf);
			
			
			child->SetActionFuncAndCookie(SelectItem, (void *)item);
		}

		if((child = (ctp2_Static *)item->GetChildByIndex(k_INT_REGARD_COL))) {
			child->SetDrawCallbackAndCookie(DrawPlayerRegard, (void *)p, true);
			MBCHAR buf[k_MAX_NAME_LEN];
			SetRegardTip(buf, p, g_selected_item->GetVisiblePlayer());
			((aui_TipWindow *)child->GetTipWindow())->SetTipText(buf);
			child->SetActionFuncAndCookie(SelectItem, (void *)item);
		}

		if((child = (ctp2_Static *)item->GetChildByIndex(k_INT_STRENGTH_COL))) {
			child->SetDrawCallbackAndCookie(DrawPlayerStrength, (void *)p, true);

			MBCHAR buf[k_MAX_NAME_LEN];




			
			
			
			
			
			
			DIPLOMATIC_STRENGTH relativeStrength = g_player[p]->GetRelativeStrength(g_selected_item->GetVisiblePlayer());

			if(relativeStrength < DIPLOMATIC_STRENGTH_WEAK)
			{
				strcpy(buf,g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_STR_VWEK_BUTTON"));
			}
			else if(relativeStrength < DIPLOMATIC_STRENGTH_AVERAGE) 
			{
				strcpy(buf,g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_STR_WEAK_BUTTON"));
			}
			else if(relativeStrength < DIPLOMATIC_STRENGTH_STRONG)
			{
				strcpy(buf,g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_STR_AVG_BUTTON"));
			}
			else
			{
				strcpy(buf,g_theStringDB->GetNameStr("TOOLTIP_DIPMAN_INTEL_STR_STRN_BUTTON"));
			}

			((aui_TipWindow *)child->GetTipWindow())->SetTipText(buf);
			child->SetActionFuncAndCookie(SelectItem, (void *)item);
		}

		if((child = (ctp2_Static *)item->GetChildByIndex(k_INT_EMBASSY_COL))) {
			child->SetDrawCallbackAndCookie(DrawEmbassy, (void *)p, true);
			child->SetActionFuncAndCookie(SelectItem, (void *)item);
		}

		if((child = (ctp2_Static *)item->GetChildByIndex(k_INT_TREATIES_COL))) {
			child->SetDrawCallbackAndCookie(DrawTreaties, (void *)p, true);
			
			
			child->SetActionFuncAndCookie(SelectItem, (void *)item);
		}
		item->SetUserData((void*)p);
		theList->AddItem(item);
	}
}

void IntelligenceWindow::Close(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Hide();
}

void IntelligenceWindow::Negotiations(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	Hide();
	DiplomacyWindow::Display();
}

void IntelligenceWindow::Advice(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(!s_intelligenceWindow) return;

	if(s_intelligenceWindow->m_adviceWindow) {
		if(g_c3ui->GetWindow(s_intelligenceWindow->m_adviceWindow->Id())) {
			g_c3ui->RemoveWindow(s_intelligenceWindow->m_adviceWindow->Id());
		} else {
			g_c3ui->AddWindow(s_intelligenceWindow->m_adviceWindow);
		}
	}

	
	IntelligenceWindow::UpdateAdviceText();
}


AUI_ERRCODE IntelligenceWindow::DrawPlayerColor(ctp2_Static *control,
												 aui_Surface *surface,
												 RECT &rect,
												 void *cookie)
{
	
	sint32 player = (sint32)cookie;
	Assert(g_colorSet);
	if(!g_colorSet)
		return AUI_ERRCODE_INVALIDPARAM;
	
	RECT drawRect = rect;
	drawRect.top += 2;
	drawRect.bottom -= 2;
	drawRect.left += 2;

	return g_c3ui->TheBlitter()->ColorBlt16(surface, &drawRect, g_colorSet->GetPlayerColor(player), 0);
}

AUI_ERRCODE IntelligenceWindow::DrawPlayerFlag(ctp2_Static *control,
												 aui_Surface *surface,
												 RECT &rect,
												 void *cookie)
{
	sint32 player = (sint32)cookie;
	Assert(g_colorSet);
	if(!g_colorSet)
		return AUI_ERRCODE_INVALIDPARAM;
	rect.left += 2;
	rect.top += 2;
	rect.right -= 2;
	rect.bottom -= 2;
	g_c3ui->TheBlitter()->ColorBlt16(surface, &rect, 0, 0);

	rect.left += 2;
	rect.top += 8;
	rect.right -= 2;
	rect.bottom -= 8;

	return g_c3ui->TheBlitter()->ColorBlt16(surface, &rect, g_colorSet->GetPlayerColor(player), 0);	
}

sint32 IntelligenceWindow::GetRegardThreshold(sint32 ofPlayer, sint32 forPlayer)
{
	
	ai::Regard airegard = Diplomat::GetDiplomat(ofPlayer).GetPublicRegard(forPlayer);

	if(airegard <= HOTWAR_REGARD) return HOTWAR_REGARD;
	else if(airegard <= COLDWAR_REGARD) return COLDWAR_REGARD;
	else if(airegard >= ALLIED_REGARD) return ALLIED_REGARD;
	else if(airegard >= FRIEND_REGARD) return FRIEND_REGARD;
	return NEUTRAL_REGARD;
}

AUI_ERRCODE IntelligenceWindow::DrawPlayerRegard(ctp2_Static *control,
												 aui_Surface *surface,
												 RECT &rect,
												 void *cookie)
{
	aui_Image *image = NULL;
	MBCHAR *imageName = NULL;
	char **toneIcons = DiplomacyWindow::GetToneIcons();
	sint32 p = (sint32)cookie;

	






	
	switch(GetRegardThreshold(p, g_selected_item->GetVisiblePlayer())) {
		case HOTWAR_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_ANGRY]; break;
		case COLDWAR_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_INDIGNANT]; break;
		case NEUTRAL_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_MEEK]; break;
		case FRIEND_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_EQUAL]; break;
		case ALLIED_REGARD: imageName = toneIcons[DIPLOMATIC_TONE_KIND]; break;
		default:
			Assert(FALSE);
			break;
	}

	
	if(imageName) {
		image = g_c3ui->LoadImage(imageName);
		if(image) {
			
			rect.left += ((rect.right - rect.left) / 2) - (image->TheSurface()->Width() / 2);
			rect.top += ((rect.bottom - rect.top) / 2) - (image->TheSurface()->Height() / 2);

			RECT srcRect = {
				0, 0,
				image->TheSurface()->Width(),
				image->TheSurface()->Height()
			};

			
			image->SetChromakey(255,0,255);

			g_c3ui->TheBlitter()->Blt(
				surface,
				rect.left,
				rect.top,
				image->TheSurface(),
				&srcRect,
				k_AUI_BLITTER_FLAG_CHROMAKEY
				);
			g_c3ui->UnloadImage(image);
		}
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE IntelligenceWindow::DrawPlayerStrength(ctp2_Static *control,
												   aui_Surface *surface,
												   RECT &rect,
												   void *cookie)
{
	aui_Image *image = NULL;
	MBCHAR *imageName = NULL;
	sint32 p = (sint32)cookie;

	if(!g_player[p]) return AUI_ERRCODE_OK;
	if(!g_player[g_selected_item->GetVisiblePlayer()]) return AUI_ERRCODE_OK;

	






	sint32 myTotalStrength = 0;
	sint32 hisTotalStrength = 0;

	
	

	
	DIPLOMATIC_STRENGTH relativeStrength = g_player[p]->GetRelativeStrength(g_selected_item->GetVisiblePlayer());
	
	if(!sm_strengthImages) {
		InitImageTables();
	}

	if(relativeStrength < DIPLOMATIC_STRENGTH_WEAK) imageName = sm_strengthImages->GetString(0);
	else if(relativeStrength < DIPLOMATIC_STRENGTH_AVERAGE) imageName = sm_strengthImages->GetString(1);
	else if(relativeStrength < DIPLOMATIC_STRENGTH_STRONG) imageName = sm_strengthImages->GetString(2);
	else imageName = sm_strengthImages->GetString(3);
											   
	
	if(imageName) {
		image = g_c3ui->LoadImage(imageName);
		if(image) {
			
			rect.left += ((rect.right - rect.left) / 2) - (image->TheSurface()->Width() / 2);
			rect.top += ((rect.bottom - rect.top) / 2) - (image->TheSurface()->Height() / 2);

			RECT srcRect = {
				0, 0,
				image->TheSurface()->Width(),
				image->TheSurface()->Height()
			};

			
			image->SetChromakey(255,0,255);

			g_c3ui->TheBlitter()->Blt(
				surface,
				rect.left,
				rect.top,
				image->TheSurface(),
				&srcRect,
				k_AUI_BLITTER_FLAG_CHROMAKEY
				);
			g_c3ui->UnloadImage(image);
		}
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE IntelligenceWindow::DrawEmbassy(ctp2_Static *control,
											aui_Surface *surface,
											RECT &rect,
											void *cookie)
{
	aui_Image *image = NULL;
	MBCHAR *imageName = NULL;
	sint32 p = (sint32)cookie;

	






	if(!sm_embassyImages) {
		InitImageTables();
	}

	if(g_player[g_selected_item->GetVisiblePlayer()]->HasEmbassyWith(p)) {
		imageName = sm_embassyImages->GetString(0);
		if(imageName) {
			aui_Image *image = g_c3ui->LoadImage(imageName);
			if(image) {
				
				rect.left += ((rect.right - rect.left) / 2) - (image->TheSurface()->Width() / 2);
				rect.top += ((rect.bottom - rect.top) / 2) - (image->TheSurface()->Height() / 2);
				
				RECT srcRect = {
					0, 0,
					image->TheSurface()->Width(),
					image->TheSurface()->Height()
				};
				
				
				image->SetChromakey(255,0,255);
				
				g_c3ui->TheBlitter()->Blt(
					surface,
					rect.left,
					rect.top,
					image->TheSurface(),
					&srcRect,
					k_AUI_BLITTER_FLAG_CHROMAKEY
					);
				g_c3ui->UnloadImage(image);
			}
		}
	} else {
		
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE IntelligenceWindow::DrawTreaties(ctp2_Static *control,
											 aui_Surface *surface,
											 RECT &rect,
											 void *cookie)
{
	sint32 p = (sint32)cookie;
	sint32 visP = g_selected_item->GetVisiblePlayer();

	







	sint32 x = 0;

	
	
	

	sint32 ag;
	sint32 slot;
	sint32 embargo_slot = -1;
	for(ag = 1; ag < PROPOSAL_MAX; ag++) {

		
		
		
		
		

		const DiplomacyProposalRecord *rec = 
			g_theDiplomacyProposalDB->Get(diplomacyutil_GetDBIndex((PROPOSAL_TYPE)ag));

		if (!rec->GetImageSlot())
			continue;

		rec->GetImageSlot(slot);

		
		if (rec->GetHasEmbargo())
		{
			if (!Diplomat::GetDiplomat(p).GetEmbargo(visP))
				continue;
		}
		else if(!AgreementMatrix::s_agreements.HasAgreement(visP, p, (PROPOSAL_TYPE)ag))
			continue;
		
		aui_Image *image = g_c3ui->LoadImage((char *)rec->GetImage());
		Assert(image);
		if(!image)
			continue;
		RECT srcRect = {
			0, 0,
				image->TheSurface()->Width(),
				image->TheSurface()->Height()
		};
		
		image->SetChromakey(255,0,255);
		
		
		x = image->TheSurface()->Width() * slot;
		
		g_c3ui->TheBlitter()->Blt(surface, rect.left + x, 
			rect.top + (((rect.bottom - rect.top) - image->TheSurface()->Height()) / 2),
			image->TheSurface(),
			&srcRect,
			k_AUI_BLITTER_FLAG_CHROMAKEY);
		
		
		
		
		g_c3ui->UnloadImage(image);
		
		
		
		
		
		
		
		
	}
	
	return AUI_ERRCODE_OK;
}
























































void IntelligenceWindow::InitImageTables()
{
	AUI_ERRCODE err;
	if(!sm_strengthImages) {
		sm_strengthImages = new aui_StringTable(&err, "StrengthImages");
	}

	if(!sm_embassyImages) {
		sm_embassyImages = new aui_StringTable(&err, "EmbassyImages");
	}
}

void IntelligenceWindow::SelectItem(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != k_CTP2_STATIC_ACTION_LMOUSE) return;
	ctp2_ListItem *item = (ctp2_ListItem *)cookie;

	if(!sm_list) return;

	
	if(sm_list->GetSelectedItem() == item) {
		sm_list->DeselectItem(item);
		DiplomacyWindow::EnableButtons(FALSE);
	} else {
		sm_list->SelectItem(item);
		DiplomacyWindow::EnableButtons( TRUE, (sint32)item->GetUserData() );
	}
	
	


}

void intelligence_DeclareWarCallback(bool response, void *cookie)
{
	if(response) {
		if(g_network.IsClient()) {
			g_network.SendAction(new NetAction(NET_ACTION_DECLARE_WAR, (sint32)cookie));
		}
		Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer()).DeclareWar((sint32)cookie);
		DiplomacyWindow::EnableButtons(TRUE, *reinterpret_cast<sint32 *>(cookie));
	}
}

void intelligence_DeclarEmbargoCallback(bool response, void *cookie)
{
	if(response) {
		Diplomat::GetDiplomat(g_selected_item->GetVisiblePlayer()).SetEmbargo((sint32)cookie, 1);
		DiplomacyWindow::EnableButtons(TRUE, *reinterpret_cast<sint32 *>(cookie));
	}
}

void IntelligenceWindow::DeclareWarOnSelected()
{
	if(!sm_list) return;

	ctp2_ListItem *item = (ctp2_ListItem *)sm_list->GetSelectedItem();

	if(!item) return;

	sint32 player = (sint32)item->GetUserData();

	sint32 visP = g_selected_item->GetVisiblePlayer();

	if(!g_player[visP]) return;

	MBCHAR buf[k_MAX_NAME_LEN];
	SlicContext so;
	so.AddPlayer(player);
	stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_IW_CONFIRM_WAR"), so, buf);

	MessageBoxDialog::Query(buf, "QueryDeclareWar", intelligence_DeclareWarCallback, (void *)player);
	
}

void IntelligenceWindow::DeclareEmbargoOnSelected()
{
	if(!sm_list) return;

	ctp2_ListItem *item = (ctp2_ListItem *)sm_list->GetSelectedItem();

	if(!item) return;

	sint32 player = (sint32)item->GetUserData();

	sint32 visP = g_selected_item->GetVisiblePlayer();

	if(!g_player[visP]) return;

	MBCHAR buf[k_MAX_NAME_LEN];
	SlicContext so;
	so.AddPlayer(player);
	stringutils_Interpret(g_theStringDB->GetNameStr("str_ldl_IW_CONFIRM_EMBARGO"), so, buf);

	MessageBoxDialog::Query(buf, "QueryDeclareEmbargo", intelligence_DeclarEmbargoCallback, (void *)player);
}

void IntelligenceWindow::SendMessageToSelected()
{
	if(!sm_list) return;

	ctp2_ListItem *item = (ctp2_ListItem *)sm_list->GetSelectedItem();

	if(!item) return;

	sint32 player = (sint32)item->GetUserData();

	sint32 visP = g_selected_item->GetVisiblePlayer();

	if(!g_player[visP]) return;
	
	if(DipWizard::CanInitiateRightNow()) {
		DipWizard::Display();
		DipWizard::SetNation(player);
	} else {
		Assert(g_network.IsActive());
		MessageBoxDialog::Information("str_code_CantInitiateDiplomacyNow", "WhyNoDiplomacy", NULL, NULL, "str_ldl_MB_OK", false);
	}
}

void IntelligenceWindow::DisplayDetailsOfSelected()
{
	if(!sm_list) return;

	ctp2_ListItem *item = (ctp2_ListItem *)sm_list->GetSelectedItem();

	if(!item) return;

	sint32 player = (sint32)item->GetUserData();

	sint32 visP = g_selected_item->GetVisiblePlayer();

	if(!g_player[visP]) return;
	
	DiplomacyDetails::SetNation(player);
	DiplomacyDetails::Display();
}


void IntelligenceWindow::UpdateAdviceText()
{
	if(!sm_list) return;

	ctp2_HyperTextBox *advice = 
		(ctp2_HyperTextBox *)aui_Ldl::GetObject(s_intelligenceAdviceBlock, "Advice");

	Assert(advice);
	if (advice == NULL)
		return;

	ctp2_ListItem *item = (ctp2_ListItem *)sm_list->GetSelectedItem();

	SlicContext sc;
	StringId adviceId;

	sint32 visP = g_selected_item->GetVisiblePlayer();
	if(!g_player[visP]) return;

	
	if(!item) {
		adviceId = Diplomat::GetDiplomat(visP).GetDiplomacyAdvice(sc);
	}
	else {
		PLAYER_INDEX player = (sint32)item->GetUserData();

		adviceId = Diplomat::GetDiplomat(visP). Diplomat::GetDiplomacyAdvice(sc, player);
	}

		
	if (adviceId < 0)
		{
			advice->SetHyperText("");
			return;
		}

	MBCHAR	strbuf[k_MAX_NAME_LEN];
	stringutils_Interpret(g_theStringDB->GetNameStr(adviceId), sc, strbuf);
	advice->SetHyperText(strbuf);
}
