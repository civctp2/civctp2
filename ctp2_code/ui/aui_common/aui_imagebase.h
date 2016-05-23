//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Image base class
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialised all members in the default constructor, to prevent a crash
//   in the destructor.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef AUI_IMAGEBASE_H__
#define AUI_IMAGEBASE_H__

class aui_ImageBase;

#define k_AUI_IMAGEBASE_LDL_BLTTYPE				"imageblttype"
#define k_AUI_IMAGEBASE_LDL_COPY				"copy"
#define k_AUI_IMAGEBASE_LDL_STRETCH				"stretch"
#define k_AUI_IMAGEBASE_LDL_TILE				"tile"

#define k_AUI_IMAGEBASE_LDL_BLTFLAG				"imagebltflag"
#define k_AUI_IMAGEBASE_LDL_BLTFLAG_COPY		"copy"
#define k_AUI_IMAGEBASE_LDL_BLTFLAG_CHROMAKEY	"chromakey"
#define k_AUI_IMAGEBASE_LDL_BLTFLAG_BLEND		"blend"

#define k_AUI_IMAGEBASE_LDL_CHROMAKEY_RED	"chromared"
#define k_AUI_IMAGEBASE_LDL_CHROMAKEY_GREEN	"chromagreen"
#define k_AUI_IMAGEBASE_LDL_CHROMAKEY_BLUE	"chromablue"

#define k_AUI_IMAGEBASE_LDL_NUMIMAGEGROUPS		"numimagegroups"

#define k_AUI_IMAGEBASE_LDL_CENTERIMAGE        "centerimage"

enum AUI_IMAGEBASE_BLTTYPE
{
	AUI_IMAGEBASE_BLTTYPE_FIRST = 0,
	AUI_IMAGEBASE_BLTTYPE_COPY = 0,
	AUI_IMAGEBASE_BLTTYPE_STRETCH,
	AUI_IMAGEBASE_BLTTYPE_TILE,
	AUI_IMAGEBASE_BLTTYPE_LAST
};


enum AUI_IMAGEBASE_BLTFLAG
{
	AUI_IMAGEBASE_BLTFLAG_FIRST = 0,
	AUI_IMAGEBASE_BLTFLAG_COPY = 0,
	AUI_IMAGEBASE_BLTFLAG_CHROMAKEY,
	AUI_IMAGEBASE_BLTFLAG_BLEND,

	AUI_IMAGEBASE_BLTFLAG_LAST
};


enum AUI_IMAGEBASE_SUBSTATE
{
	AUI_IMAGEBASE_SUBSTATE_FIRST = 0,
	AUI_IMAGEBASE_SUBSTATE_STATE = 0,
	AUI_IMAGEBASE_SUBSTATE_STATEACTIVE,
	AUI_IMAGEBASE_SUBSTATE_STATEFOCUS,
	AUI_IMAGEBASE_SUBSTATE_STATEACTIVEFOCUS,
	AUI_IMAGEBASE_SUBSTATE_STATEDISABLED,
	AUI_IMAGEBASE_SUBSTATE_LAST
};


class aui_Image;
class aui_Surface;
class ldl_datablock;

typedef aui_Image * aui_StateImageGroup[ AUI_IMAGEBASE_SUBSTATE_LAST ];

class aui_ImageBase
{
public:
	aui_ImageBase
    (
		MBCHAR const *          ldlBlock,
		bool                    loadOnDemand        = false
    );
	aui_ImageBase
    (
		sint32                  numStateImageGroups = 0,
		AUI_IMAGEBASE_BLTTYPE   blttype             = AUI_IMAGEBASE_BLTTYPE_COPY,
		AUI_IMAGEBASE_BLTFLAG   bltflag             = AUI_IMAGEBASE_BLTFLAG_COPY,
		bool                    loadOnDemand        = false
    );
	virtual ~aui_ImageBase();

protected:
	AUI_ERRCODE InitCommonLdl(MBCHAR const * ldlBlock);
	AUI_ERRCODE InitCommon(
		sint32 numImageGroups,
		AUI_IMAGEBASE_BLTTYPE blttype,
		AUI_IMAGEBASE_BLTFLAG bltflag );

	sint32 FindNumStateImageGroupsFromLdl( ldl_datablock *block );

public:

	AUI_IMAGEBASE_BLTTYPE GetImageBltType( void ) const { return m_imageblttype; }
	AUI_IMAGEBASE_BLTFLAG GetImageBltFlag( void) const { return m_imagebltflag; }
	aui_Image *GetImage(
		sint32 state = 0,
		AUI_IMAGEBASE_SUBSTATE substate = AUI_IMAGEBASE_SUBSTATE_STATE ) const;

	AUI_IMAGEBASE_BLTTYPE SetImageBltType( AUI_IMAGEBASE_BLTTYPE imageblttype );
	AUI_IMAGEBASE_BLTFLAG SetImageBltFlag( AUI_IMAGEBASE_BLTFLAG imagebltflag );
	aui_Image * SetImage
    (
		MBCHAR const *          image,
		sint32                  state       = 0,
		AUI_IMAGEBASE_SUBSTATE  substate    = AUI_IMAGEBASE_SUBSTATE_STATE
    );

	bool IsLoadOnDemand() const { return m_loadOnDemand; }

protected:

	AUI_ERRCODE DrawImage(
		aui_Surface *destSurf,
		RECT *destRect,
		sint32 state = 0,
		AUI_IMAGEBASE_SUBSTATE substate = AUI_IMAGEBASE_SUBSTATE_STATE );

	AUI_IMAGEBASE_BLTTYPE	m_imageblttype;
	AUI_IMAGEBASE_BLTFLAG	m_imagebltflag;


	aui_Image * LoadTheImage(MBCHAR const * name);

	sint32					m_numStateImageGroups;
	aui_StateImageGroup		*m_stateImageGroups;


	static MBCHAR const *   m_substateLdlKeywords[AUI_IMAGEBASE_SUBSTATE_LAST];

	bool m_loadOnDemand;

	sint32	m_chromaRed;
	sint32	m_chromaGreen;
	sint32	m_chromaBlue;
	bool	m_chromaSpecified;

	sint32 m_numberOfStateImageNames;
	MBCHAR **m_stateImageNames;

	bool m_centerImage;
};

#endif
