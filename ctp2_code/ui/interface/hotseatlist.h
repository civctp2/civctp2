//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Hotseat (and e-mail) game setup screen
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Code strucure improvements: multiple include guard added, cleanup in
//   destructor.
//
//----------------------------------------------------------------------------

#ifndef HOTSEATLIST_H
#define HOTSEATLIST_H

#include "c3_listitem.h"
#include "aui_action.h"
#include "keyboardhandler.h"

typedef void (HotseatListCallback)(sint32 launch,
				   sint32 player,
				   sint32 civ,
				   BOOL human,
				   const MBCHAR *name,
				   const MBCHAR *email);

#define k_MAX_HOTSEAT_PLAYERS 8

class HotseatListItem : public c3_ListItem
{
public:

	HotseatListItem(AUI_ERRCODE *retval, sint32 index,
					sint32 civ, BOOL isHuman, const MBCHAR *email,
					const MBCHAR *ldlBlock);


	virtual void Update(void);

	sint32  GetCiv() { return m_civ; }
	void	SetCiv(sint32 civ) { m_civ = civ; }

	BOOL IsHuman() { return m_isHuman; }
	const MBCHAR	*GetEmail( void ) { return m_email; }
	sint32  GetIndex() { return m_index; }
	const MBCHAR *GetName() { return m_name; }

	void SetHuman(BOOL human);
	void ChooseCiv();
	void EnterEmail();

protected:
	HotseatListItem() : c3_ListItem() {}


	AUI_ERRCODE InitCommonLdl(sint32 civ, BOOL isHuman, const MBCHAR *email, const MBCHAR *ldlBlock);

public:

	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	sint32          m_index;
	MBCHAR			m_email[256];
	MBCHAR          m_name[256];
	sint32			m_civ;
	BOOL            m_isHuman;
};

class HotseatList : public KeyboardHandler
{
public:
	HotseatList(HotseatListCallback *callback = NULL, const MBCHAR *ldlBlock = NULL);
	virtual ~HotseatList();

	c3_PopupWindow	*m_window;

	c3_ListBox		*m_list;
	c3_Button		*m_ok;

	HotseatListCallback *m_callback;
	HotseatListItem *m_items[k_MAX_HOTSEAT_PLAYERS];

public:
	sint32 Initialize (const MBCHAR *ldlBlock);
	sint32 UpdateData ( void );

	sint32 EnableButtons( void );
	sint32 DisableButtons( void );
	sint32 ChooseNextOpenCiv(HotseatListItem *curItem, sint32 curCiv);

	void RemoveWindow( void );
	void DisplayWindow( void );

	void kh_Close();
};


void hotseatlist_ClearOptions(void);
void hotseatlist_SetPlayerCiv(PLAYER_INDEX index, CIV_INDEX civ);
void hotseatlist_LockCivs(void);
BOOL hotseatlist_PlayerCivsLocked(void);
void hotseatlist_EnableAllCivs(void);
void hotseatlist_DisableAllCivs(void);
void hotseatlist_EnableCiv(CIV_INDEX civ);
void hotseatlist_DisableCiv(CIV_INDEX civ);
BOOL hotseatlist_CivEnabled(CIV_INDEX civ);
sint32 hotseatlist_NumEnabled(void);





extern HotseatList *g_hotseatList;

#endif
