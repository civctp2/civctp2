//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __AUI_MOVIEMANAGER_H__
#define __AUI_MOVIEMANAGER_H__

class aui_MovieManager;

#include "aui_base.h"       // aui_Base
#include "aui_movie.h"
#include "aui_resource.h"
#include "c3files.h"        // C3DIR

class aui_MovieManager : aui_Base
{
public:
	aui_MovieManager(bool init = true);
	virtual ~aui_MovieManager();

	virtual aui_Movie	*Load( const MBCHAR *filename, C3DIR dir = C3DIR_DIRECT )
	{ return m_movieResource->Load( filename, dir ); }

	virtual AUI_ERRCODE Unload( aui_Movie *movie )
	{ return m_movieResource->Unload( movie ); }
	virtual AUI_ERRCODE Unload( const MBCHAR *movie )
	{ return m_movieResource->Unload( movie ); }

	virtual AUI_ERRCODE AddSearchPath( const MBCHAR *path )
	{ return m_movieResource->AddSearchPath( path ); }
	virtual AUI_ERRCODE RemoveSearchPath( const MBCHAR *path )
	{ return m_movieResource->RemoveSearchPath( path ); }

	aui_Resource<aui_Movie> *MovieResource( void ) const
	{ return m_movieResource; }

protected:
	aui_Resource<aui_Movie>	*m_movieResource;
};


#endif 
