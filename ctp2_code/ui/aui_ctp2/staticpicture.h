#ifndef __STATICPICTURE_H__
#define __STATICPICTURE_H__

class Picture;

#include "aui_static.h"


#define k_AUI_STATICIMAGE_LDL_IMAGE "image"


class StaticPicture : public aui_Static
{
public:

	StaticPicture(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		const MBCHAR *picture );
	StaticPicture(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *picture );
	virtual ~StaticPicture();

protected:
	StaticPicture() : aui_Static() {}
	AUI_ERRCODE InitCommon(const MBCHAR *ldlBlock, const MBCHAR *picture );
	AUI_ERRCODE InitCommon(const MBCHAR *picture);

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	void SetPicture(MBCHAR *picture);

protected:
	Picture *m_picture;
};

#endif
