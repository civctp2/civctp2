









#ifndef __AUI_IMAGELIST_H__
#define __AUI_IMAGELIST_H__

#include "aui_Image.h"
#include "aui_ImageBase.h"	


class aui_ImageList {
public:
	
	aui_ImageList(sint32 numStates, sint32 numImages, bool loadOnDemand = false);

	
	virtual ~aui_ImageList();

	
	
	
	void SetImage(sint32 state, sint32 imageIndex,
				  AUI_IMAGEBASE_BLTTYPE bltType, AUI_IMAGEBASE_BLTFLAG bltFlag,
				  RECT *rect, const MBCHAR *imageFileName,
				  sint32 chromaRed = 0, sint32 chromaGreen = 255,
			      sint32 chromaBlue = 0);

	
	void ExchangeImage(sint32 state, sint32 imageIndex, const MBCHAR *imageFileName);

	
	aui_Image *GetImage(sint32 state, sint32 imageIndex);

	
	RECT *GetSize(sint32 state, sint32 imageIndex);

	
	AUI_ERRCODE SetSize(sint32 state, sint32 imageIndex, RECT *size);

	
	bool IsLoadOnDemand() { return(m_loadOnDemand); }

	
	void SetState(sint32 state) { m_currentState = state; }

	
	
	AUI_ERRCODE DrawImages(
		aui_Surface *destSurf,
		RECT *destRect);

	
	sint32 GetState() const { return(m_currentState); }	
	sint32 GetNumStates() const { return m_numStates; }	
	sint32 GetNumImages() const { return m_numImages; }	

private:

	
	
	class aui_ImageListInfo
	{
	public:
		
		aui_ImageListInfo() : m_imageName(NULL),
			m_image(NULL), m_bltType(AUI_IMAGEBASE_BLTTYPE_COPY),
			m_bltFlag(AUI_IMAGEBASE_BLTFLAG_COPY),
			m_chromaRed(k_DEFAULT_CHROMA_RED),
			m_chromaGreen(k_DEFAULT_CHROMA_GREEN),
			m_chromaBlue(k_DEFAULT_CHROMA_BLUE)
		{ m_rect.left = m_rect.right = m_rect.top = m_rect.bottom = 0; }

		
		~aui_ImageListInfo();

		
		void Load();

		
		MBCHAR *m_imageName;

		
		aui_Image *m_image;

		
		RECT m_rect;

		
		AUI_IMAGEBASE_BLTTYPE m_bltType;	
		AUI_IMAGEBASE_BLTFLAG m_bltFlag;	

		
		sint32	m_chromaRed	 ;
		sint32	m_chromaGreen;
		sint32	m_chromaBlue ;
	};

	
	aui_ImageListInfo *GetImageInfo(sint32 state, sint32 imageIndex);

	
	bool VerifyRange(sint32 state, sint32 imageIndex)
	{
		
		Assert(state >= 0 && state < m_numStates);
		Assert(imageIndex >= 0 && imageIndex < m_numImages);
		if(state < 0 || state >= m_numStates)
			return(false);
		if(imageIndex < 0 || imageIndex >= m_numImages)
			return(false);
		return(true);
	}

	
	sint32 m_numStates;

	
	sint32 m_numImages;

	
	
	aui_ImageListInfo **m_images;

	
	sint32 m_currentState;

	
	bool m_loadOnDemand;
};


#endif 
