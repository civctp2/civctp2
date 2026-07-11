//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : User interface image list handling
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Unload image info when not found.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_ui.h"
#include "aui_blitter.h"
#include "aui_surface.h"
#include "aui_ldl.h"
#include "aui_imagelist.h"

aui_ImageList::aui_ImageListInfo::~aui_ImageListInfo()
{

	if(m_image) {
		g_ui->UnloadImage(m_image);
		m_image = NULL;
	}

	if(m_imageName) {
		delete [] m_imageName;
		m_imageName = NULL;
	}
}

void aui_ImageList::aui_ImageListInfo::Load()
{

	if(m_image)
		return;

	if(m_imageName) {

		m_image = g_ui->LoadImage(m_imageName);
		Assert(m_image);

		if(m_image) {
			m_image->SetChromakey(m_chromaRed,m_chromaGreen,m_chromaBlue);
			delete [] m_imageName;
			m_imageName = NULL;
		}
		else
		{
			g_ui->UnloadImage(m_imageName);
		}
	}
}

aui_ImageList::aui_ImageList(sint32 numStates, sint32 numImages,
							 bool loadOnDemand) :
m_numStates(numStates),
m_numImages(numImages),
m_loadOnDemand(loadOnDemand)
{

	Assert(m_numStates);
	Assert(m_numImages);

	m_images = new aui_ImageListInfo *[m_numStates];

	for(sint32 i = 0; i < m_numStates; i++) {
		m_images[i] = new aui_ImageListInfo[m_numImages];
	}

	m_currentState = 0;
}

aui_ImageList::~aui_ImageList()
{

	for(sint32 i = 0; i < m_numStates; i++) {
		delete [] m_images[i];
		m_images[i] = NULL;
	}

	delete [] m_images;
	m_images = NULL;
}




void aui_ImageList::SetImage(sint32 state, sint32 imageIndex, AUI_IMAGEBASE_BLTTYPE bltType,
							 AUI_IMAGEBASE_BLTFLAG bltFlag, RECT *rect,
							 const MBCHAR *imageFileName, sint32 chromaRed,
							 sint32 chromaGreen, sint32 chromaBlue)
{

	if(!VerifyRange(state, imageIndex))
		return;

	aui_ImageListInfo *info = &m_images[state][imageIndex];

	info->m_bltType = bltType;
	info->m_bltFlag = bltFlag;
	info->m_rect.left = rect->left;
	info->m_rect.right = rect->right;
	info->m_rect.top = rect->top;
	info->m_rect.bottom = rect->bottom;
	info->m_chromaRed = chromaRed;
	info->m_chromaGreen = chromaGreen;
	info->m_chromaBlue = chromaBlue;

	ExchangeImage(state, imageIndex, imageFileName);
}

void aui_ImageList::ExchangeImage(sint32 state, sint32 imageIndex,
								  const MBCHAR *imageFileName)
{

	if(!VerifyRange(state, imageIndex))
		return;

	aui_ImageListInfo *info = &m_images[state][imageIndex];

	aui_Image *oldImage = info->m_image;
	info->m_image = NULL;

	if(info->m_imageName) {
		delete [] info->m_imageName;
		info->m_imageName = NULL;
	}

	if(!imageFileName) {
		if(oldImage)
			g_ui->UnloadImage(oldImage);

		return;
	}

	if(m_loadOnDemand) {

		info->m_imageName = new char[strlen(imageFileName) + 1];
		strcpy(info->m_imageName, imageFileName);
	} else {

		aui_Image *theImage = g_ui->LoadImage(const_cast<char*>(imageFileName));
		if(theImage) {

			if(info->m_rect.right < 0)
				info->m_rect.right = info->m_rect.left +
				theImage->TheSurface()->Width();
			if(info->m_rect.bottom < 0)
				info->m_rect.bottom = info->m_rect.top +
				theImage->TheSurface()->Height();

		info->m_image = theImage;

		info->m_image->SetChromakey(info->m_chromaRed,
			info->m_chromaGreen, info->m_chromaBlue);
		}
		else
		{
			g_ui->UnloadImage(const_cast<MBCHAR *>(imageFileName));
		}
	}
	if(oldImage)
		g_ui->UnloadImage(oldImage);
}

aui_Image *aui_ImageList::GetImage(sint32 state, sint32 imageIndex)
{

	if(!VerifyRange(state, imageIndex))
		return(NULL);

	aui_ImageListInfo *info = &m_images[state][imageIndex];

	if(!info->m_image) {

		if(!info->m_imageName)
			return NULL;

		info->Load();
		Assert(info->m_image);
	}

	return info->m_image;
}

aui_ImageList::aui_ImageListInfo *aui_ImageList::GetImageInfo(sint32 state, sint32 imageIndex)
{

	if(!VerifyRange(state, imageIndex))
		return(NULL);

	aui_ImageListInfo *info = &m_images[state][imageIndex];

	if(!info->m_image) {
		if(info->m_imageName) {
			info->Load();
			Assert(info->m_image);
		}
	}

	return info;
}


AUI_ERRCODE aui_ImageList::DrawImages(aui_Surface *destSurf, RECT *destRect)
{

	AUI_ERRCODE err = AUI_ERRCODE_OK;

	for(sint32 imageIndex = 0; imageIndex < m_numImages; imageIndex++) {

		aui_ImageListInfo *info = GetImageInfo( m_currentState, imageIndex);
		if(!info || !info->m_image)
			continue;

		aui_Surface *srcSurf = info->m_image->TheSurface();

		// A failed image load can leave a non-NULL aui_Image with no surface
		// (aui_ResourceElement ctor bails before Load), or a surface whose
		// logical size is degenerate. Blitting either is an access violation
		// (software Blt16To16 trusts these values), so skip and name the file.
		if (!srcSurf || srcSurf->Width() <= 0 || srcSurf->Height() <= 0)
		{
			DPRINTF(k_DBG_FIX, ("DrawImages: skipping unloadable/degenerate image '%s' (surface=%p, w=%d, h=%d)\n",
				info->m_image->GetFilename(), srcSurf,
				srcSurf ? srcSurf->Width() : -1, srcSurf ? srcSurf->Height() : -1));
			continue;
		}

		RECT srcRect = { 0, 0, srcSurf->Width(), srcSurf->Height() };

		RECT subDestRect = {
			info->m_rect.left + destRect->left,
			info->m_rect.top + destRect->top,
			info->m_rect.right + destRect->left,
			info->m_rect.bottom + destRect->top
		};

		uint32 flag = 0;
		switch ( info->m_bltFlag )
		{
			default:
			case AUI_IMAGEBASE_BLTFLAG_COPY:
				flag |= k_AUI_BLITTER_FLAG_COPY;
				break;
			case AUI_IMAGEBASE_BLTFLAG_CHROMAKEY:
				flag |= k_AUI_BLITTER_FLAG_CHROMAKEY;
				break;
			case AUI_IMAGEBASE_BLTFLAG_BLEND:
				flag |= k_AUI_BLITTER_FLAG_BLEND;
				break;
		}


		// SEH: a control can survive in the draw list while its image surfaces
		// have been freed (seen drawing stale setup-screen statics during
		// CivApp::InitializeGame's progress redraw) — the blit then reads a dead
		// buffer whose metadata is self-consistent. Catch the AV per image for
		// EVERY blt type so one dead surface cannot kill the process.
		// (All locals are PODs, so SEH is legal here.)
		char dbgName[80];
		dbgName[0] = '\0';
		__try
		{
			// Snapshot the filename inside the guard: if the aui_Image object is
			// freed, reading it faults here (caught) instead of in the failure log.
			strncpy(dbgName, info->m_image->GetFilename(), sizeof(dbgName) - 1);
			dbgName[sizeof(dbgName) - 1] = '\0';

			switch(info->m_bltType) {
				default:
				case AUI_IMAGEBASE_BLTTYPE_COPY:
					err = g_ui->TheBlitter()->Blt(destSurf, subDestRect.left,
						subDestRect.top, srcSurf, &srcRect, flag);
					break;

				case AUI_IMAGEBASE_BLTTYPE_STRETCH:
					err = g_ui->TheBlitter()->StretchBlt(destSurf, &subDestRect,
						srcSurf, &srcRect, flag);
					break;
				case AUI_IMAGEBASE_BLTTYPE_TILE:
					err = g_ui->TheBlitter()->TileBlt(destSurf, &subDestRect,
						srcSurf, &srcRect, 0, 0, flag );
					break;
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			err = AUI_ERRCODE_INVALIDPARAM;
		}

		// Name the offending image when a blit fails (e.g. the Blt16To16 SEH
		// guard caught an access violation on a dead buffer), then keep drawing —
		// one bad image must not kill the whole UI pass.
		if (err != AUI_ERRCODE_OK)
		{
			DPRINTF(k_DBG_FIX, ("DrawImages: blit FAILED (err=%d) for image '%s' "
				"(surface %dx%d) bltType=%d bltFlag=%d state=%d index=%d\n",
				err, dbgName, srcRect.right, srcRect.bottom,
				info->m_bltType, info->m_bltFlag, m_currentState, imageIndex));
			err = AUI_ERRCODE_OK;
		}
	}

	Assert(err == AUI_ERRCODE_OK);
	return(err);
}

AUI_ERRCODE aui_ImageList::SetSize(sint32 state, sint32 imageIndex, RECT *size)
{

	aui_ImageListInfo *info = GetImageInfo(state, imageIndex);
	if(!info)
		return AUI_ERRCODE_INVALIDPARAM;

	info->m_rect.left = size->left;
	info->m_rect.right = size->right;
	info->m_rect.top = size->top;
	info->m_rect.bottom = size->bottom;

	return AUI_ERRCODE_OK;
}

RECT *aui_ImageList::GetSize(sint32 state, sint32 imageIndex)
{

	aui_ImageListInfo *info = GetImageInfo(state, imageIndex);
	if(!info)
		return NULL;

	return(&info->m_rect);
}
