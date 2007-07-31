










#ifndef __AUI_TEXTFIELD_H__
#define __AUI_TEXTFIELD_H__


#include "aui_win.h"



enum AUI_TEXTFIELD_ACTION
{
	AUI_TEXTFIELD_ACTION_FIRST = 0,
	AUI_TEXTFIELD_ACTION_NULL = 0,
	AUI_TEXTFIELD_ACTION_EXECUTE,	
	AUI_TEXTFIELD_ACTION_DISMISS,	
	AUI_TEXTFIELD_ACTION_LAST
};



#define k_AUI_TEXTFIELD_LDL_TEXT		"fieldtext"
#define k_AUI_TEXTFIELD_LDL_MULTILINE	"multiline"
#define k_AUI_TEXTFIELD_LDL_AUTOVSCROLL	"autovscroll" 
#define k_AUI_TEXTFIELD_LDL_AUTOHSCROLL	"autohscroll" 
#define k_AUI_TEXTFIELD_LDL_ISFILENAME	"isfilename"
#define k_AUI_TEXTFIELD_LDL_PASSWORD	"password"
#define k_AUI_TEXTFIELD_LDL_FONT		"fontname"
#define k_AUI_TEXTFIELD_LDL_FONTHEIGHT	"fontheight"
#define k_AUI_TEXTFIELD_LDL_MAXFIELDLEN	"maxfieldlen"



class aui_TextField : public aui_Win
{
public:
	
	aui_TextField(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_TextField(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *text = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_TextField();

protected:
	aui_TextField() : aui_Win() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(
		MBCHAR *text,
		MBCHAR *font,
		sint32 fontheight,
		BOOL multiLine,
		BOOL autovscroll = TRUE,
		BOOL autohscroll = TRUE,
		BOOL isfilename = FALSE,
		sint32 maxFieldLen = 1024,
		BOOL passwordReady = FALSE);

public:
	sint32	GetFieldText( MBCHAR *text, sint32 maxCount );
	BOOL	SetFieldText( const MBCHAR *text );

	BOOL	IsMultiLine( void ) const { return m_multiLine; }
	BOOL	SetMultiLine( BOOL multiLine );

	BOOL	IsPasswordReady( void ) const { return m_passwordReady; }
	BOOL	SetPasswordReady( BOOL passwordReady );

	BOOL	IsFileName( void ) const { return m_isFileName; }
	BOOL	SetIsFileName( BOOL isFileName );

	sint32	GetMaxFieldLen( void ) const { return m_maxFieldLen; }
	sint32	SetMaxFieldLen( sint32 maxFieldLen );

	virtual aui_Control	*SetKeyboardFocus( void );
	virtual AUI_ERRCODE	ReleaseKeyboardFocus( void );

	virtual AUI_ERRCODE	DrawThis(
		aui_Surface *surface,
		sint32 x,
		sint32 y );

	
	static WNDPROC	m_windowProc;

	static void HitEnter( HWND hwnd );
	static BOOL IsFileName( HWND hwnd );
	static sint32 GetMaxFieldLen( HWND hwnd );

	
	
	void SetSelection(sint32 start, sint32 end);
	void GetSelection(sint32 *start, sint32 *end);
	void SelectAll(void);

protected:
	BOOL	m_blink;			
	BOOL	m_blinkThisFrame;	
								
	BOOL	m_multiLine;		
	BOOL	m_passwordReady;	

	BOOL	m_isFileName;		

	sint32	m_maxFieldLen;

public:
	sint32	m_textHeight;		
	MBCHAR	m_desiredFont[256];	
	HFONT	m_hfont;			
	HFONT	m_holdfont;			

	
	
	

	
	virtual MouseEventCallback PostChildrenCallback;

	virtual MouseEventCallback MouseLGrabOutside;
};




LRESULT CALLBACK TextFieldWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
int CALLBACK EnumTextFontsProc( LOGFONT *lplf, TEXTMETRIC *lptm, DWORD dwType, LPARAM lParam );

#endif 
