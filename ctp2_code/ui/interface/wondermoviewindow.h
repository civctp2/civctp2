

#pragma once
#ifndef __WONDERMOVIEWINDOW_H__
#define __WONDERMOVIEWINDOW_H__

class aui_MovieButton;
class c3_Static;
class Sequence;
class ctp2_HyperTextBox;

class WonderMovieWindow : public C3Window
{
public:
	
	WonderMovieWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	WonderMovieWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );

	virtual ~WonderMovieWindow();

	virtual AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock);
	virtual AUI_ERRCODE InitCommon(void);

	virtual AUI_ERRCODE Idle();

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	void SetMovie(const MBCHAR *filename);
	void SetWonderName(MBCHAR *name);
	void SetText(const MBCHAR *text);

	Sequence *GetSequence(void) { return m_sequence;}
	void SetSequence(Sequence *seq) { m_sequence = seq; }

private:
	aui_MovieButton		*m_movieButton;
	c3_Static			*m_wonderName;
	Sequence 			*m_sequence;
	c3_Static			*m_topBorder;
	c3_Static			*m_leftBorder;
	c3_Static			*m_rightBorder;
	c3_Static			*m_bottomBorder;
	ctp2_HyperTextBox   *m_textBox;
};


#endif
