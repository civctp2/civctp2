












#include "c3.h"

#include "aui.h"
#include "aui_control.h"

#include "pattern.h"
#include "primitives.h"


#include "debugwindow.h"
#include "c3cmdline.h"
#include "c3ui.h"

#include "TextBox.h"

extern C3UI				*g_c3ui;


#include "ColorSet.h"
extern ColorSet *g_colorSet;
DebugWindow::DebugWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:
	C3Window(retval, id, ldlBlock, bpp, type)
{
    m_allow_next = FALSE; 
    m_debug_mask = 0xFFFFFFFF;
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

   
}



DebugWindow::DebugWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type)
	:
	C3Window( retval, id, x, y, width, height, bpp, pattern, type )
{

    m_allow_next = FALSE; 
    m_debug_mask = 0xFFFFFFFF;
	*retval = InitCommon();
	Assert(AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;
}



AUI_ERRCODE DebugWindow::InitCommon(void)
{
	m_textBox = NULL;
		
	return C3Window::InitCommon();
}



AUI_ERRCODE DebugWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	m_pattern->Draw( m_surface, &rect );

	
	primitives_BevelRect16( m_surface, &rect, 1, 0, 16, 16 );

	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}
 


void DebugWindow::AddText(MBCHAR *text)
{
	
	
	
	if (GetParent() == NULL) return;

    
    
    

	if (m_textBox)
		m_textBox->AppendText( text , g_colorSet->GetColorRef(COLOR_BLACK), 
            FALSE, FALSE); 
}

void DebugWindow::SetDebugMask(uint32 m) 

{ 
    m_debug_mask = m; 
} 

void DebugWindow::AddText(const uint32 m, MBCHAR *text) 

{ 
    if (m & m_debug_mask) { 

    	if (m_textBox)
	    	m_textBox->AppendText( text, g_colorSet->GetColorRef(COLOR_BLACK), 
            FALSE, FALSE); 
    }
}

void DebugWindow::AddMask(uint32 m)
{
    if (m & m_debug_mask) { 
        m_allow_next = TRUE; 
    } else { 
        m_allow_next = FALSE; 
    } 
}

void DebugWindow::AddText(const char *err, ...)
{
	if (GetParent() == NULL) return;

    if (m_allow_next) { 
        m_allow_next = FALSE; 
        
        va_list		list ;
        MBCHAR	str[_MAX_PATH]={0};
        
        va_start(list, err) ;
	    vsprintf(str, err, list) ;
	    va_end(list) ;

       	if (m_textBox) { 
	    	m_textBox->AppendText(str, g_colorSet->GetColorRef(COLOR_BLACK), 
            FALSE, FALSE); 
        }
    }
}
