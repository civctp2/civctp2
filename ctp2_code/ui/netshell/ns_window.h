










#ifndef __NS_WINDOW_H__
#define __NS_WINDOW_H__



#include "c3_popupwindow.h"


#define k_NS_WINDOW_TILE_LDL_NAME		"tile"
#define k_NS_WINDOW_RETIRED_LDL_NAME	"retired"

class aui_Control;
class aui_Image;
class c3_Static;



class ns_Window : public c3_PopupWindow
{
public:
	
	ns_Window(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	virtual ~ns_Window();

protected:

	ns_Window() : c3_PopupWindow() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( MBCHAR *tile, BOOL retired );

public:
	aui_Control	*FindControl( uint32 index );

	aui_Image	*SetTile( MBCHAR *tile );
	aui_Image	*GetTile( void ) const { return m_tile; }

	BOOL		IsRetired( void ) const { return m_retired; }
	BOOL		SetRetired( BOOL retired );

	


















protected:
	sint32		m_numControls;	


	aui_Control	**m_controls;	


	aui_Image	*m_tile;		

	BOOL		m_retired;		
};


#endif 
