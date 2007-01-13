#ifndef __AUI_SDLBLITTER_H__
#define __AUI_SDLBLITTER_H__

#ifdef __AUI_USE_SDL__

#include "aui_blitter.h"

class aui_SDLBlitter : public aui_Blitter
{
public:
	
	aui_SDLBlitter()
	{
	}
	virtual ~aui_SDLBlitter() {}
	
	virtual NakedBltFunc Blt16To16;
	virtual NakedColorBltFunc ColorBlt16;
	virtual NakedStretchBltFunc StretchBlt16To16;
	virtual NakedColorStencilBltFunc ColorStencilBlt16;

protected:
};


#endif 

#endif 
