//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Activision User Interface - ldl handling
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fixed memory leaks.
//
//----------------------------------------------------------------------------

#ifndef __AUI_LDL_H__
#define __AUI_LDL_H__

#include "aui_base.h"
#include "tech_wllist.h"

#include "aui_control.h"

#include "ldl_user.h"

class aui_Region;
class aui_Dimension;
template <class T> class AvlTree;

struct aui_LdlObject
{
	void			*object;
	MBCHAR			*ldlBlock;
	uint32			hash;
	aui_LdlObject	*prev;
	aui_LdlObject	*next;
};


#define k_AUI_LDL_MAXBLOCK			1024

#define k_AUI_LDL_DEFAULTTEMPLATE	"default"

#define k_AUI_LDL_PARENT			"parent"
#define k_AUI_LDL_HANCHOR			"xanchor"
#define k_AUI_LDL_VANCHOR			"yanchor"
#define k_AUI_LDL_HABSPOSITION		"xpix"
#define k_AUI_LDL_HRELPOSITION		"xpct"
#define k_AUI_LDL_VABSPOSITION		"ypix"
#define k_AUI_LDL_VRELPOSITION		"ypct"
#define k_AUI_LDL_HABSSIZE			"widthpix"
#define k_AUI_LDL_HRELSIZE			"widthpct"
#define k_AUI_LDL_VABSSIZE			"heightpix"
#define k_AUI_LDL_VRELSIZE			"heightpct"




#define k_AUI_LDL_OBJECTTYPE		"objecttype"





#define k_AUI_LDL_ATOMIC			"atomic"






#define k_AUI_LDL_DETACH			"detach"

class aui_Ldl : public aui_Base
{
public:

	aui_Ldl(
		AUI_ERRCODE *retval,
		const MBCHAR *ldlFilename );
	virtual ~aui_Ldl();

protected:
	aui_Ldl() : aui_Base() {}
	AUI_ERRCODE InitCommon( const MBCHAR *ldlFilename );

public:

	static BOOL			IsValid(const MBCHAR *ldlBlock);

	static ldl			*GetLdl( void ) { return m_ldl; }

	static AUI_ERRCODE	Associate( void *object, const MBCHAR *ldlBlock );
	static AUI_ERRCODE	Remove( void *object );
	static AUI_ERRCODE	Remove(const MBCHAR *ldlBlock);
	static MBCHAR		*GetBlock( void *object );
	static void			*GetObject(const MBCHAR *ldlBlock);
	static void			*GetObject(const MBCHAR *parentBlock, const MBCHAR *regionBlock);

	static AUI_ERRCODE	SetupHeirarchyFromRoot(const MBCHAR *rootBlock);
	static AUI_ERRCODE	SetupHeirarchyFromLeaf(const MBCHAR *leafBlock, aui_Region *object);







	static aui_Region	*BuildHierarchyFromRoot(const MBCHAR *rootBlock);






	static AUI_ERRCODE	BuildHierarchyFromLeaf(ldl_datablock *dataBlock, aui_Region *region);






	static AUI_ERRCODE	BuildObjectFromType(const MBCHAR *typeString,
	    const MBCHAR *ldlName, aui_Region **theObject);






	static AUI_ERRCODE	DeleteHierarchyFromRoot(const MBCHAR *rootBlock);







	static AUI_ERRCODE	DeleteHierarchyFromLeaf(ldl_datablock *dataBlock);








	static AUI_ERRCODE  SetActionFuncAndCookie(const MBCHAR *ldlBlock,
	    aui_Control::ControlActionCallback *actionFunc,
	    void *cookie = NULL);
	static AUI_ERRCODE  SetActionFuncAndCookie(const MBCHAR *parentBlock, const MBCHAR *regionBlock,
	    aui_Control::ControlActionCallback *actionFunc,
	    void *cookie = NULL);






	static AUI_ERRCODE	DetachHierarchy(aui_Region *root);

	static void			ModifyAttributes(const MBCHAR *ldlBlock, aui_Dimension *dimension );


	static sint32		GetIntDependent(const MBCHAR *strPtr);

protected:
	static void			DeleteLdlObject( aui_LdlObject *ldlObject );

	static AUI_ERRCODE	MakeSureBlockExists(const MBCHAR *ldlBlock);
	static AUI_ERRCODE	MakeSureDefaultTemplateExists( void );

	static AUI_ERRCODE	AppendLdlObject(aui_LdlObject *object);
	static AUI_ERRCODE	RemoveLdlObject(aui_LdlObject *object);

	static ldl			*m_ldl;

	static aui_LdlObject				*m_objectList;
	static aui_LdlObject				*m_objectListTail;

	static AvlTree<aui_LdlObject *>		*m_objectListByObject;
	static AvlTree<aui_LdlObject *>		*m_objectListByString;

	static sint32						m_ldlRefCount;
};

#endif
