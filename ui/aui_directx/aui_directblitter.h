










#ifndef __AUI_DIRECTBLITTER_H__
#define __AUI_DIRECTBLITTER_H__

#ifdef __AUI_USE_DIRECTX__


#include "aui_blitter.h"



class aui_DirectBlitter : public aui_Blitter
{
public:
	
	aui_DirectBlitter()
	{
		memset( &m_ddbfx, 0, sizeof( m_ddbfx ) );
		m_ddbfx.dwSize = sizeof( m_ddbfx );
	}
	virtual ~aui_DirectBlitter() {}

	
	
	virtual NakedBltFunc Blt16To16;
	virtual NakedColorBltFunc ColorBlt16;
	virtual NakedStretchBltFunc StretchBlt16To16;
	virtual NakedColorStencilBltFunc ColorStencilBlt16;

protected:
	DDBLTFX m_ddbfx;
};


#endif 

#endif 
