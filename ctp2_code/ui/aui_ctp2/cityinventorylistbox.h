









#pragma once
#ifndef __CITYINVENTORYLISTBOX_H__
#define __CITYINVENTORYLISTBOX_H__

#include "C3ListBox.h"

class Unit;

class CityInventoryListBox : public C3ListBox
{
public:
	
	CityInventoryListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	CityInventoryListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~CityInventoryListBox ();

	sint32 FillInventoryBox( const Unit &unit );
	void UpdateInventoryBox( const Unit &unit );
	sint32 UpdateImage( const Unit &unit );
	void SetUnit( const Unit *unit ) { m_unit = unit; }
	const Unit *GetUnit( void ) { return m_unit; }
	void CutBuilding( const Unit &unit );

protected:
	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock );

	const Unit *m_unit;
	sint32 m_buildMode;


};

#endif