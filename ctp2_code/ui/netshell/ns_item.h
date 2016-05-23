#ifndef __NS_ITEM_H__
#define __NS_ITEM_H__

#include "aui_item.h"
#include "aui_surface.h"
#include "aui_directui.h"
#include "aui_window.h"
#include "aui_blitter.h"
#include "aui_uniqueid.h"
#include "aui_ui.h"

#include "c3_static.h"
#include "c3_listitem.h"

#include "netfunc.h"
#include "ns_aiplayersetup.h"

#define k_NS_ITEM_MAXTEXT			100
#define k_NS_ITEM_DEFAULTWIDTH		60
#define k_NS_ITEM_DEFAULTHEIGHT		25

class c3_EditButton;
class c3_Button;




class ns_ListItem : public c3_ListItem
{
public:
	ns_ListItem(
		AUI_ERRCODE *retval,
		const MBCHAR *name,
		const MBCHAR *ldlBlock);
	~ns_ListItem() {}

	virtual void Update(void) {}
	virtual sint32 Compare(c3_ListItem *item2, uint32 column) { return 0; }

protected:
	ns_ListItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(
		const MBCHAR *name,
		const MBCHAR *ldlBlock);
};


class ns_HPlayerItem : public c3_ListItem
{
public:
	ns_HPlayerItem(
		AUI_ERRCODE *retval,
		void *player,
		BOOL isAI,
		const MBCHAR *ldlBlock);

	~ns_HPlayerItem();

	virtual void Update(void) {}
	virtual sint32 Compare(c3_ListItem *item2, uint32 column) { return 0; }

	NETFunc::Player *GetPlayer( void ) const
	{ return IsAI() ? NULL : (NETFunc::Player *)m_player; }
	nf_AIPlayer *GetAIPlayer( void ) const
	{ return IsAI() ? (nf_AIPlayer *)m_player : NULL; }

	BOOL IsAI( void ) const { return m_isAI; }

	aui_Control *GetHostItem( void ) { return this; }
	aui_Control *GetLaunchedItem( void ) const { return m_launchedItem; }
	aui_Control *GetNameItem( void ) const { return m_nameItem; }
	aui_Control *GetPingItem( void ) const { return m_pingItem; }

	aui_Control *GetTribeItem( void ) const { return m_tribeItem; }
	c3_Button *GetTribeButton( void ) const
	{ return m_tribeButton; }
	void SetTribe( sint32 tribe );






	aui_Control *GetCivpointsItem( void ) const { return m_civpointsItem; }
	c3_EditButton *GetCivpointsButton( void ) const { return m_civpointsButton;}
	void SetCivpoints( sint32 civpoints );

	aui_Control *GetPwpointsItem( void ) const { return m_pwpointsItem; }
	c3_EditButton *GetPwpointsButton( void ) const { return m_pwpointsButton;}
	void SetPwpoints( sint32 pwpoints );

protected:
	ns_HPlayerItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(
		void *player,
		BOOL isAI,
		const MBCHAR *ldlBlock);

	void *m_player;
	BOOL m_isAI;

	c3_Static *m_launchedItem;
	c3_Static *m_nameItem;
	c3_Static *m_pingItem;

	c3_Static *m_tribeItem;
	c3_Button *m_tribeButton;




	c3_Static *m_civpointsItem;
	c3_EditButton *m_civpointsButton;

	c3_Static *m_pwpointsItem;
	c3_EditButton *m_pwpointsButton;
};




template<class T,class NetShellT>
class ns_Item : public aui_Item
{
public:

	ns_Item(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		T *object = NULL );
	ns_Item(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		T *object = NULL );
	virtual ~ns_Item();

protected:
	ns_Item() : aui_Item() {}
	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateNetShellObject( T *object );

public:
	NetShellT	*GetNetShellObject( void ) const { return m_netShellT; }

	AUI_ERRCODE	SetIcon(const MBCHAR *icon);
	aui_Image	*GetIcon( void ) const { return m_icon; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	NetShellT	*m_netShellT;
	aui_Image	*m_icon;
};





template<class T,class NetShellT>
ns_Item<T,NetShellT>::ns_Item(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	T *object )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Item( retval, id, ldlBlock )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateNetShellObject( object );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


template<class T,class NetShellT>
ns_Item<T,NetShellT>::ns_Item(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	T *object )
	:
	aui_Item( retval, id, x, y, width, height ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL, k_NS_ITEM_MAXTEXT )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateNetShellObject( object );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


template<class T,class NetShellT>
AUI_ERRCODE ns_Item<T,NetShellT>::InitCommonLdl(const MBCHAR *ldlBlock)
{

	return InitCommon();
}


template<class T,class NetShellT>
AUI_ERRCODE ns_Item<T,NetShellT>::InitCommon( void )
{
	m_netShellT = NULL;
	m_icon = NULL;

	m_textflags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT |
		k_AUI_BITMAPFONT_DRAWFLAG_VERTCENTER;





	return AUI_ERRCODE_OK;
}


template<class T,class NetShellT>
AUI_ERRCODE ns_Item<T,NetShellT>::CreateNetShellObject( T *object )
{
	if ( object )
	{
		m_netShellT = new NetShellT( object );
		Assert( m_netShellT != NULL );
		if ( !m_netShellT ) return AUI_ERRCODE_MEMALLOCFAILED;
	}

	return AUI_ERRCODE_OK;
}


template<class T,class NetShellT>
ns_Item<T,NetShellT>::~ns_Item()
{
	if ( m_netShellT )
	{
		delete m_netShellT;
		m_netShellT = NULL;
	}

	if ( m_icon )
	{
		g_ui->UnloadImage( m_icon );
		m_icon = NULL;
	}
}


template<class T,class NetShellT>
AUI_ERRCODE ns_Item<T,NetShellT>::SetIcon(const MBCHAR *icon)
{
	aui_Image *prevImage = m_icon;

	if ( icon )
	{
		m_icon = g_ui->LoadImage( icon );
		Assert( m_icon != NULL );
		if ( !m_icon )
		{
			m_icon = prevImage;
			return AUI_ERRCODE_LOADFAILED;
		}
	}
	else
		m_icon = NULL;

	if ( prevImage ) g_ui->UnloadImage( prevImage );

	return AUI_ERRCODE_OK;
}





template <class T,class NetShellT>
AUI_ERRCODE ns_Item<T,NetShellT>::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	InflateRect( &rect, -2, -2 );









	if ( m_icon )
	{




		RECT destRect = rect;










		RECT srcRect =
		{
			0,
			0,
			m_icon->TheSurface()->Width(),
			m_icon->TheSurface()->Height()
		};

		if ( destRect.left < destRect.right
		&&   destRect.top < destRect.bottom )
			g_ui->TheBlitter()->Blt(
				surface,
				destRect.left,
				destRect.top,
				m_icon->TheSurface(),
				&srcRect,
				k_AUI_BLITTER_FLAG_CHROMAKEY );
	}

	DrawThisText(
		surface,
		&rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

#endif
