

#pragma once
#ifndef __VICTORYMOVIEWINDOW_H__
#define __VICTORYMOVIEWINDOW_H__

class aui_MovieButton;
class c3_Static;
class Sequence;

class VictoryMovieWindow : public C3Window
{
public:
	VictoryMovieWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_POPUP );
	VictoryMovieWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_POPUP );

	virtual ~VictoryMovieWindow();

	virtual AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock);
	virtual AUI_ERRCODE InitCommon(void);

	virtual AUI_ERRCODE Idle();

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	void SetMovie(MBCHAR *filename);
	void SetVictoryName(MBCHAR *name);

	Sequence *GetSequence(void) { return m_sequence; }
	void SetSequence(Sequence *seq) { m_sequence = seq; }

private:
	aui_MovieButton		*m_movieButton;
	c3_Static			*m_wonderName;

	Sequence			*m_sequence;
};


#endif