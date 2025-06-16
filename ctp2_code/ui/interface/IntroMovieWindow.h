#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __INTROMOVIEWINDOW_H__
#define __INTROMOVIEWINDOW_H__

class aui_MovieButton;
class c3_Static;

class IntroMovieWindow : public C3Window
{
public:
	IntroMovieWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	IntroMovieWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		const MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );

	virtual ~IntroMovieWindow();

	virtual AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	virtual AUI_ERRCODE InitCommon(void);

	virtual AUI_ERRCODE Idle();

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	void SetMovie(const MBCHAR *filename);

private:
	aui_MovieButton		*m_movieButton;
};

#endif
