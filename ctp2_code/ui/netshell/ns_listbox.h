//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Listbox for network game setup
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
// - Corrected strange access of non-static members from static data.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifndef __NS_LISTBOX_H__
#define __NS_LISTBOX_H__

#include "aui.h"
#include "ns_civlistbox.h"
#include "aui_directui.h"
#include "aui_ranger.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_bitmapfont.h"
#include "ns_item.h"
#include "ns_header.h"
#include "ns_headerswitch.h"

#include "netfunc.h"


template<class T,class NetShellT>
class ns_ListBox : public ns_CivListBox, public NETFunc::ListHandler<T>
{
public:

	ns_ListBox(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ns_ListBox();

protected:
	sint32 m_artXOffset;
	sint32 m_artYOffset;
	aui_Image *m_itemsSelectImage;

	ns_ListBox() : ns_CivListBox() {}
	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon( void );

public:

	virtual void Insert( T *object );
	virtual void Delete( T *object );
	virtual void Change( T *object );
	virtual void Destroy( void );

	ns_Item<T,NetShellT> *GetSelectedNetShellItem( void ) const
	{ return (ns_Item<T,NetShellT> *)GetSelectedItem(); }

	ns_Item<T,NetShellT> *FindItem( T *object );

	virtual AUI_ERRCODE SortByColumn( sint32 column, BOOL ascending );

protected:

	AUI_ERRCODE	AddNetShellItem( ns_Item<T,NetShellT> *item );
	AUI_ERRCODE	RemoveNetShellItem( ns_Item<T,NetShellT> *item );
	AUI_ERRCODE UpdateNetShellItem( ns_Item<T,NetShellT> *item );

private:

	AUI_ERRCODE	StoreAppropriateData( ns_Item<T,NetShellT> *item, sint32 i );
};








template<class T,class NetShellT>
void ns_ListBox<T,NetShellT>::Insert( T *object )
{

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	ns_Item<T,NetShellT> *item = new ns_Item<T,NetShellT>(
		&errcode,
		aui_UniqueId(),
		"listitems.nsitem",
		object );
	Assert( item != NULL );
	if ( !item ) return;

	AddNetShellItem( item );
}


template<class T,class NetShellT>
void ns_ListBox<T,NetShellT>::Delete( T *object )
{

	ns_Item<T,NetShellT> *item = FindItem( object );
	if ( item )
		RemoveNetShellItem( item );
}


template<class T,class NetShellT>
void ns_ListBox<T,NetShellT>::Change( T *object )
{

	ns_Item<T,NetShellT> *item = FindItem( object );
	if ( item )
	{
		item->GetNetShellObject()->Update(object);
		UpdateNetShellItem( item );
		item->ShouldDraw();

		SortByColumn( m_sortColumn, m_sortAscending );
	}
}


template<class T,class NetShellT>
void ns_ListBox<T,NetShellT>::Destroy( void )
{

	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		ns_Item<T,NetShellT> *item =
			(ns_Item<T,NetShellT> *)m_pane->ChildList()->GetNext( position );

		RemoveNetShellItem( item );
	}
}


template<class T,class NetShellT>
ns_Item<T,NetShellT> *ns_ListBox<T,NetShellT>::FindItem( T *object )
{
	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		ns_Item<T,NetShellT> *item =
			(ns_Item<T,NetShellT> *)m_pane->ChildList()->GetNext( position );

		if ( object->Equals( item->GetNetShellObject() ) )
			return item;
	}

	return NULL;
}








template<class T,class NetShellT>
ns_ListBox<T,NetShellT>::ns_ListBox(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	ns_CivListBox( retval, id, ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


template<class T,class NetShellT>
AUI_ERRCODE ns_ListBox<T,NetShellT>::InitCommonLdl(const MBCHAR *ldlBlock)
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	Assert( theLdl->IsValid( ldlBlock ) );

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	m_artXOffset = block->GetInt( "xoffset" );
	m_artYOffset = block->GetInt( "yoffset" );

	const char *image = block->GetString("selectimage");
	if(image)
		m_itemsSelectImage = g_ui->LoadImage( image );

	return AUI_ERRCODE_OK;
}


template<class T,class NetShellT>
AUI_ERRCODE ns_ListBox<T,NetShellT>::InitCommon( void )
{
	m_artXOffset = 0;
	m_artYOffset = 0;
	m_itemsSelectImage = NULL;

	SetAbsorbancy( TRUE );

	return AUI_ERRCODE_OK;
}


template<class T,class NetShellT>
ns_ListBox<T,NetShellT>::~ns_ListBox()
{

	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		ListPos prevPosition = position;
		aui_Item *item = (aui_Item *)m_pane->ChildList()->GetNext( position );

		ListPos childPosition = item->ChildList()->GetHeadPosition();
		for ( sint32 j = item->ChildList()->L(); j; j-- )
		{
			ListPos prevChildPosition = childPosition;
			delete item->ChildList()->GetNext( childPosition );
			item->ChildList()->DeleteAt( prevChildPosition );
		}

		delete item;
		m_pane->ChildList()->DeleteAt( prevPosition );
	}

	if ( m_itemsSelectImage )
	{
		g_ui->UnloadImage(m_itemsSelectImage);
		m_itemsSelectImage = NULL;
	}
}








template<class T,class NetShellT>
AUI_ERRCODE ns_ListBox<T,NetShellT>::AddNetShellItem(
	ns_Item<T,NetShellT> *item )
{
	Assert( item != NULL );
	if ( !item ) return AUI_ERRCODE_INVALIDPARAM;

	if ( item->GetNetShellObject() )
	{
		sint32 const	numProperties =
			item->GetNetShellObject()->list.size();
		for ( sint32 i = 1; i < numProperties; i++ )
		{

			AUI_ERRCODE errcode = AUI_ERRCODE_OK;
			ns_Item<T,NetShellT> *childItem = new ns_Item<T,NetShellT>(
				&errcode,
				aui_UniqueId(),
				"listitems.nsitem",
				NULL );
			Assert( AUI_NEWOK(childItem,errcode) );
			if ( !AUI_NEWOK(childItem,errcode) )
				return AUI_ERRCODE_MEMALLOCFAILED;

			item->AddChild( childItem );

			NetShellT *netShellObject = item->GetNetShellObject();
			if ( netShellObject->type( i ) == ns_Accessor<NetShellT>::INT )
			{
				childItem->TextFlags() =
					k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER |
					k_AUI_BITMAPFONT_DRAWFLAG_VERTCENTER;
			}
		}
	}

	AddItem( item );

	UpdateNetShellItem( item );

	return AUI_ERRCODE_OK;
}


template<class T,class NetShellT>
AUI_ERRCODE ns_ListBox<T,NetShellT>::RemoveNetShellItem(
	ns_Item<T,NetShellT> *item )
{
	Assert( item != NULL );
	if ( !item ) return AUI_ERRCODE_INVALIDPARAM;

	DeselectItem( item );

	ListPos position = m_pane->ChildList()->GetHeadPosition();
	for ( sint32 i = m_pane->ChildList()->L(); i; i-- )
	{
		ns_Item<T,NetShellT> *curItem =
			(ns_Item<T,NetShellT> *)m_pane->ChildList()->GetNext( position );

		if ( curItem == item )
		{
			RemoveItem( item->Id() );

			position = item->ChildList()->GetHeadPosition();
			for ( sint32 j = item->ChildList()->L(); j; j-- )
				delete item->ChildList()->GetNext( position );

			delete item;

			break;
		}
	}

	return AUI_ERRCODE_OK;
}


template<class T,class NetShellT>
AUI_ERRCODE ns_ListBox<T,NetShellT>::UpdateNetShellItem(
	ns_Item<T,NetShellT> *item )
{
	Assert( item != NULL );
	if ( !item ) return AUI_ERRCODE_INVALIDPARAM;

	if ( item->GetNetShellObject() )
	{
		StoreAppropriateData( item, 0 );
		sint32 const numProperties =
			item->GetNetShellObject()->list.size();
		ListPos position = item->ChildList()->GetHeadPosition();
		for ( sint32 i = 1; i < numProperties; i++ )
		{
			ns_Item<T,NetShellT> *childItem =
				(ns_Item<T,NetShellT> *)item->ChildList()->GetNext( position );
			StoreAppropriateData( childItem, i );
		}
	}


	m_draw |= k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}


template<class T,class NetShellT>
AUI_ERRCODE ns_ListBox<T,NetShellT>::StoreAppropriateData(
	ns_Item<T,NetShellT> *item,
	sint32 i )
{

	NetShellT *netShellObject = item->GetNetShellObject();
	if ( !netShellObject )
		netShellObject = ((ns_Item<T,NetShellT> *)item->GetParent())->
			GetNetShellObject();

	sint32 width = 0;
	if ( GetHeader() && GetHeader()->ChildList() )
	{
		ListPos pos = GetHeader()->ChildList()->FindIndex( i );
		if ( pos )
			width = GetHeader()->ChildList()->GetAt( pos )->Width();
	}

	if ( !width && i == 0 )
		width = Width();

	if ( width )
		item->Resize( width, item->Height() );

	void *	dataPtr	= netShellObject->data(i);

	if (dataPtr)
	{
		MBCHAR text[40] = { 0 };
		switch (netShellObject->type(i))
		{
		case ns_Accessor<T>::STRING:
			{
				AUI_ERRCODE	const	r =
					item->SetText(* reinterpret_cast<MBCHAR const * *>(dataPtr));

				MBCHAR name[256 + 1];
				strncpy(name, item->GetText(), 256);

				if (!item->GetTextFont())
				{
					item->TextReloadFont();
				}

				item->GetTextFont()->TruncateString(name, item->Width());
				item->SetText(name);
				item->SetTextBold(netShellObject->IsMine());

				return r;
			}

		case ns_Accessor<T>::INT:
			item->SetTextBold(netShellObject->IsMine());
			sprintf(text, "%d", *reinterpret_cast<sint32 const *>(dataPtr));
			return item->SetText(text);

		case ns_Accessor<T>::ICON:
			return item->SetIcon(* reinterpret_cast<MBCHAR * *>(dataPtr));

		default:
			break;
		} // switch
	}

	Assert(FALSE);
	return AUI_ERRCODE_INVALIDPARAM;
}


template<class T,class NetShellT>
AUI_ERRCODE ns_ListBox<T,NetShellT>::SortByColumn(
	sint32 column,
	BOOL ascending )
{

	if ( column == -1 ) return AUI_ERRCODE_OK;

	Assert( 0 <= column && column < m_numColumns );
	if ( 0 > column || column >= m_numColumns ) return AUI_ERRCODE_INVALIDPARAM;

	m_sortColumn = column;
	m_sortAscending = ascending;

	sint32 limit = m_numRows - 1;
	if ( limit <= 0 ) return AUI_ERRCODE_OK;

	BOOL changed;
	do
	{
		changed = FALSE;

		for ( sint32 i = 0; i < limit; i++ )
		{
			ListPos positionA = m_pane->ChildList()->FindIndex( i );
			ListPos positionB = positionA;

			ns_Item<T,NetShellT> *itemA =
				(ns_Item<T,NetShellT> *)m_pane->ChildList()->GetNext( positionB );
			ns_Item<T,NetShellT> *itemB =
				(ns_Item<T,NetShellT> *)m_pane->ChildList()->GetAt( positionB );

			NetShellT *nsObjectA = itemA->GetNetShellObject();
			NetShellT *nsObjectB = itemB->GetNetShellObject();

			if ( m_sortAscending ?
				nsObjectA->comp( m_sortColumn, nsObjectB ) > 0 :
				nsObjectA->comp( m_sortColumn, nsObjectB ) < 0 )
			{
				SwitchItems( i, i + 1 );
				changed = TRUE;
			}
		}
	} while ( changed );

	RepositionItems();

	return AUI_ERRCODE_OK;
}

#endif
