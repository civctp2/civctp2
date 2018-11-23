//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface general resource handling
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3UI_H__
#define __C3UI_H__

#if defined(__AUI_USE_SDL__)
#include "aui_sdlui.h"
#else
#include "aui_directui.h"
#endif
#include "pattern.h"
#include "icon.h"
#include "picture.h"
#include "aui_resource.h"
#include "aui_window.h"

class C3UI : public aui_NativeUI
{
public:
	C3UI(
		AUI_ERRCODE *retval,
		HINSTANCE hinst,
		HWND hwnd,
		sint32 width,
		sint32 height,
		sint32 bpp,
		const MBCHAR *ldlFilename = NULL,
		BOOL useExclusiveMode = FALSE );
	virtual ~C3UI();

	aui_Resource<Pattern>	*GetPatternResource( void ) const { return m_patternResource; }

	Pattern	*LoadPattern( const MBCHAR *name )
		{ return m_patternResource->Load( name ); }

	AUI_ERRCODE	UnloadPattern( Pattern *resource )
		{ return m_patternResource->Unload( resource ); }
	AUI_ERRCODE	UnloadPattern( const MBCHAR *name )
		{ return m_patternResource->Unload( name ); }

	AUI_ERRCODE	AddPatternSearchPath( MBCHAR *path )
		{ return m_patternResource->AddSearchPath( path ); }
	AUI_ERRCODE	RemovePatternSearchPath( MBCHAR *path )
		{ return m_patternResource->RemoveSearchPath( path ); }

	aui_Resource<Icon>	*GetIconResource( void ) const { return m_iconResource; }

	Icon	*LoadIcon( const MBCHAR *name )
		{ return m_iconResource->Load( name ); }

	AUI_ERRCODE	UnloadIcon( Icon *resource )
		{ return m_iconResource->Unload( resource ); }
	AUI_ERRCODE	UnloadIcon( const MBCHAR *name )
		{ return m_iconResource->Unload( name ); }

	AUI_ERRCODE	AddIconSearchPath( MBCHAR *path )
		{ return m_iconResource->AddSearchPath( path ); }
	AUI_ERRCODE	RemoveIconSearchPath( MBCHAR *path )
		{ return m_iconResource->RemoveSearchPath( path ); }

	aui_Resource<Picture>	*GetPictureResource( void ) const { return m_pictureResource; }

	Picture	*LoadPicture( const MBCHAR *name )
		{ return m_pictureResource->Load( name ); }

	AUI_ERRCODE	UnloadPicture( Picture *resource )
		{ return m_pictureResource->Unload( resource ); }
	AUI_ERRCODE	UnloadPicture( const MBCHAR *name )
		{ return m_pictureResource->Unload( name ); }

	AUI_ERRCODE	AddPictureSearchPath( MBCHAR *path )
		{ return m_pictureResource->AddSearchPath( path ); }
	AUI_ERRCODE	RemovePictureSearchPath( MBCHAR *path )
		{ return m_pictureResource->RemoveSearchPath( path ); }

	BOOL TopWindowIsNonBackground(void);

protected:
	aui_Resource<Pattern>	*m_patternResource;
	aui_Resource<Icon>		*m_iconResource;
	aui_Resource<Picture>	*m_pictureResource;
};

#endif
