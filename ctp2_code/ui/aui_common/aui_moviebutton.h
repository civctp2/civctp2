










#ifndef __AUI_MOVIEBUTTON_H__
#define __AUI_MOVIEBUTTON_H__


#include "aui_button.h"


class aui_Movie;



#define k_AUI_MOVIEBUTTON_LDL_MOVIE		"movie"



class aui_MovieButton : public aui_Button
{
public:
	
	aui_MovieButton(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_MovieButton(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *movie = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_MovieButton();

protected:
	aui_MovieButton() : aui_Button() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( MBCHAR *movie );
	
public:
	aui_Movie *SetMovie( const MBCHAR *movie );
	aui_Movie *GetMovie( void ) const { return m_movie; }

	void SetFullScreen(bool on) { m_fullScreen = on; }

	
	virtual AUI_ERRCODE Idle( void );

	
	
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 ) { return AUI_ERRCODE_OK; }

	void SetFlags(uint32 flags) { m_flags = flags; }
	uint32 GetFlags(void) { return m_flags; }

protected:
	aui_Movie	*m_movie;
	uint32		m_flags;
	bool m_fullScreen;
};


#endif 
