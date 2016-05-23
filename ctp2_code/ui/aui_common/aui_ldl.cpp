//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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

#include "c3.h"
#include "aui_ldl.h"

#include "aui_ui.h"
#include "aui_dimension.h"
#include "aui_uniqueid.h"

#include "avl.h"

#include "c3slider.h"
#include "c3window.h"
#include "c3windows.h"
#include "c3_button.h"
#include "c3_static.h"
#include "ctp2_Static.h"
#include "ctp2_button.h"
#include "ctp2_Window.h"
#include "ctp2_menubar.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_dropdown.h"
#include "ctp2_spinner.h"
#include "ctp2_hypertextbox.h"
#include "c3_hypertextbox.h"
#include "aui_switch.h"
#include "colorset.h"
#include "ctp2_Switch.h"
#include "ctp2_textfield.h"
#include "ctp2_MenuButton.h"
#include "ctp2_TabGroup.h"
#include "ctp2_Tab.h"
#include "ctp2_TabButton.h"
#include "ctp2_TabGroup.h"
#include "radarmap.h"
#include "linegraph.h"
#include "AvlTree.h"

#include "ldl_user.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

ldl *aui_Ldl::s_ldl = NULL;

AvlTree<aui_LdlObject *>	*aui_Ldl::s_objectListByObject = NULL;
AvlTree<aui_LdlObject *>	*aui_Ldl::s_objectListByString = NULL;

sint32						aui_Ldl::s_ldlRefCount = 0;

aui_LdlObject				*aui_Ldl::s_objectList = NULL;
aui_LdlObject				*aui_Ldl::s_objectListTail = NULL;










static cmp_t CompareByObject(aui_LdlObject *obj1, aui_LdlObject *obj2)
{
	if (obj1->object < obj2->object) return MIN_CMP;
	if (obj1->object > obj2->object) return MAX_CMP;

	return EQ_CMP;
}

static cmp_t CompareByString(aui_LdlObject *obj1, aui_LdlObject *obj2)
{
	if (obj1->hash < obj2->hash) return MIN_CMP;
	if (obj1->hash > obj2->hash) return MAX_CMP;

	return EQ_CMP;
}

aui_Ldl::aui_Ldl
(
	AUI_ERRCODE *   retval,
	MBCHAR const *  ldlFilename
)
:
	aui_Base    ()
{
	*retval = InitCommon( ldlFilename );
}


AUI_ERRCODE aui_Ldl::InitCommon( MBCHAR const *ldlFilename )
{
	Assert(ldlFilename != NULL);
	if (!ldlFilename) return AUI_ERRCODE_INVALIDPARAM;

	if (!s_objectListByObject)
	{
		s_objectListByObject = new AvlTree<aui_LdlObject *>;
		Assert( s_objectListByObject != NULL );
		if ( !s_objectListByObject ) return AUI_ERRCODE_MEMALLOCFAILED;
    }

    if (!s_objectListByString)
    {
		s_objectListByString = new AvlTree<aui_LdlObject *>;
		Assert( s_objectListByString != NULL );
		if ( !s_objectListByString ) return AUI_ERRCODE_MEMALLOCFAILED;
	}

	MBCHAR outDir[ MAX_PATH + 1 ];
#ifdef HAVE_UNISTD_H
	getcwd(outDir, MAX_PATH);
#else
	GetCurrentDirectory( MAX_PATH, outDir );
#endif
	strcat(outDir, FILE_SEP "ldl_out");

    delete s_ldl;
	s_ldl = new ldl( ldlFilename, outDir );
	Assert( s_ldl != NULL );
	if ( !s_ldl ) return AUI_ERRCODE_MEMALLOCFAILED;

	s_ldlRefCount++;

	return AUI_ERRCODE_OK;
}


aui_Ldl::~aui_Ldl()
{
    if (0 == --s_ldlRefCount)
    {
    	delete s_ldl;
        s_ldl = NULL;
	    delete s_objectListByObject;
        s_objectListByObject = NULL;
	    delete s_objectListByString;
        s_objectListByString = NULL;

        aui_LdlObject * nextObject  = NULL;
	    for (aui_LdlObject * curObject = s_objectList; curObject; curObject = nextObject)
	    {
		    nextObject = curObject->next;
		    DeleteLdlObject(curObject);
	    }
    }
}


bool aui_Ldl::IsValid(MBCHAR const * ldlBlock)
{
	return FindDataBlock(ldlBlock) != NULL;
}


AUI_ERRCODE aui_Ldl::MakeSureBlockExists(MBCHAR const *ldlBlock)
{
	Assert( ldlBlock != NULL );
	if ( !ldlBlock ) return AUI_ERRCODE_INVALIDPARAM;

	ldl_datablock *block = s_ldl->FindDataBlock( ldlBlock );
	if ( !block )
	{





#if 1





		block = new ldl_datablock(s_ldl, ldlBlock);
		Assert( block != NULL );
		if ( !block ) return AUI_ERRCODE_MEMALLOCFAILED;

		block->AddAttribute( k_AUI_LDL_HANCHOR, "left" );

		block->AddAttribute( k_AUI_LDL_VANCHOR, "top" );

		block->AddAttribute( k_AUI_LDL_HABSPOSITION, 0 );
		block->AddAttribute( k_AUI_LDL_VABSPOSITION, 0 );
		block->AddAttribute( k_AUI_LDL_HABSSIZE, 100 );
		block->AddAttribute( k_AUI_LDL_VABSSIZE, 100 );

		BOOL success = s_ldl->WriteData();
		Assert( success );
		if ( !success ) return AUI_ERRCODE_LDLFILEWRITEFAILED;
#else
		return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;
#endif
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Ldl::MakeSureDefaultTemplateExists( void )
{
	ldl_datablock *format = s_ldl->FindDataBlock( k_AUI_LDL_DEFAULTTEMPLATE );
	if ( !format )
	{

#if 1

		format = new ldl_datablock( s_ldl, k_AUI_LDL_DEFAULTTEMPLATE );
		Assert( format != NULL );
		if ( !format ) return AUI_ERRCODE_MEMALLOCFAILED;

		format->AddAttribute( k_AUI_LDL_HANCHOR, "left" );

		format->AddAttribute( k_AUI_LDL_VANCHOR, "top" );

		format->AddAttribute( k_AUI_LDL_HABSPOSITION, 0 );
		format->AddAttribute( k_AUI_LDL_VABSPOSITION, 0 );
		format->AddAttribute( k_AUI_LDL_HABSSIZE, 100 );
		format->AddAttribute( k_AUI_LDL_VABSSIZE, 100 );

		BOOL success = s_ldl->WriteData();
		Assert( success );
		if ( !success ) return AUI_ERRCODE_LDLFILEWRITEFAILED;
#else
		return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;
#endif
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Ldl::Associate(void *object, MBCHAR const * ldlBlock)
{

	if ( !s_objectListByObject || !s_objectListByString ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( object != NULL );
	if ( !object ) return AUI_ERRCODE_INVALIDPARAM;
	Assert( ldlBlock != NULL );
	if ( !ldlBlock ) return AUI_ERRCODE_INVALIDPARAM;

	if ( GetBlock( object ) ) return AUI_ERRCODE_OK;

	aui_LdlObject *ldlObject = new aui_LdlObject;
	Assert( ldlObject != NULL );
	if ( !ldlObject ) return AUI_ERRCODE_MEMALLOCFAILED;

	ldlObject->ldlBlock = new MBCHAR[ strlen( ldlBlock ) + 1 ];
	Assert( ldlObject->ldlBlock != NULL );
	if ( !ldlObject->ldlBlock ) return AUI_ERRCODE_MEMALLOCFAILED;

	ldlObject->object = object;
	strcpy( ldlObject->ldlBlock, ldlBlock );
	ldlObject->hash = aui_UI::CalculateHash( ldlBlock );




	Comparable<aui_LdlObject *> *objByObject = new Comparable<aui_LdlObject *>(ldlObject, CompareByObject);
	if(s_objectListByObject->Insert(objByObject))
		delete objByObject;

	Comparable<aui_LdlObject *> *objByString = new Comparable<aui_LdlObject *>(ldlObject, CompareByString);
	if(s_objectListByString->Insert(objByString))
		delete objByString;

	return AppendLdlObject(ldlObject);
}

AUI_ERRCODE aui_Ldl::AppendLdlObject(aui_LdlObject *object)
{
	if (NULL == object)
		return AUI_ERRCODE_INVALIDPARAM;

	object->next = NULL;
	object->prev = s_objectListTail;

	if (s_objectListTail)
    {
		s_objectListTail->next = object;
    }
    else
    {
		s_objectList = object;
	}

	s_objectListTail = object;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Ldl::RemoveLdlObject(aui_LdlObject *object)
{
    if (NULL == object)
		return AUI_ERRCODE_INVALIDPARAM;

	if (s_objectList == object)
    {
        // Object at begin
        s_objectList = object->next;

		if (s_objectList)
        {
			s_objectList->prev  = NULL;
		}
        else
        {
			s_objectListTail    = NULL;
		}
	}
    else if (s_objectListTail == object)
    {
        // Object at end
		s_objectListTail = object->prev;
	    s_objectListTail->next  = object->next;
    }
    else
    {
        // Object in between
		object->prev->next = object->next;
		object->next->prev = object->prev;
	}

    object->next    = NULL;
    object->prev    = NULL;
	return AUI_ERRCODE_OK;
}


void aui_Ldl::DeleteLdlObject( aui_LdlObject *ldlObject )
{
	if ( ldlObject )
	{
		delete [] ldlObject->ldlBlock;
		delete ldlObject;
	}
}


AUI_ERRCODE aui_Ldl::Remove( void *object )
{
	if (!object)
		return AUI_ERRCODE_INVALIDPARAM;

	if ( !s_objectListByObject || !s_objectListByString)
		return AUI_ERRCODE_INVALIDPARAM;

	aui_LdlObject		dummyObject;
	dummyObject.object = object;

	Comparable<aui_LdlObject *>	*   myKey =
        s_objectListByObject->Search(&dummyObject);

	if (myKey) {
		aui_LdlObject		*foundObject = myKey->Key();

		if (foundObject) {

			delete s_objectListByString->Delete(foundObject);

			delete s_objectListByObject->Delete(foundObject);

			RemoveLdlObject(foundObject);

			DeleteLdlObject(foundObject);
		}
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Ldl::Remove(MBCHAR const * ldlBlock)
{

	if ( !s_objectListByObject || !s_objectListByString) return AUI_ERRCODE_INVALIDPARAM;


	uint32 hash = aui_UI::CalculateHash( ldlBlock );


	aui_LdlObject		myObject;
	myObject.hash = hash;

	Comparable<aui_LdlObject *> * myKey =
		s_objectListByString->Delete(&myObject);

	if (myKey) {

		aui_LdlObject	*ldlObject = myKey->Key();

		if (ldlObject) {
			s_objectListByString->Delete(ldlObject);

			RemoveLdlObject(ldlObject);

			DeleteLdlObject(ldlObject);
		}
		delete myKey;
	}

	return AUI_ERRCODE_OK;
}


MBCHAR *aui_Ldl::GetBlock( void *object )
{

	if ( !s_objectListByObject ) return NULL;


	aui_LdlObject	dummyObject;
	dummyObject.object = object;

	Comparable<aui_LdlObject *> *myKey =
        s_objectListByObject->Search(&dummyObject);

	if (myKey) {

		aui_LdlObject *obj = myKey->Key();
		Assert(obj);
		if (obj) {
			return obj->ldlBlock;
		}
	}

	return NULL;
}


void *aui_Ldl::GetObject( const MBCHAR *ldlBlock )
{

	if ( !s_objectListByString ) return NULL;

	uint32 hash = aui_UI::CalculateHash( ldlBlock );

	aui_LdlObject	dummyObject;
	dummyObject.hash = hash;

	Comparable<aui_LdlObject *>	*myKey =
        s_objectListByString->Search(&dummyObject);

	if (myKey) {

		aui_LdlObject *obj = myKey->Key();
		Assert(obj);
		if (obj) {
			return obj->object;
		}
	}

	return NULL;
}

void *aui_Ldl::GetObject(const MBCHAR *parentBlock, const MBCHAR *regionBlock)
{
	Assert(parentBlock && regionBlock);
	if (parentBlock == NULL || regionBlock == NULL)
		return NULL;

	MBCHAR		ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf(ldlBlock, "%s.%s", parentBlock, regionBlock);

	return GetObject(ldlBlock);
}


AUI_ERRCODE aui_Ldl::SetupHeirarchyFromRoot(MBCHAR const * rootBlock )
{

	if ( !s_objectListByString ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( rootBlock != NULL );
	if ( !rootBlock ) return AUI_ERRCODE_INVALIDPARAM;

	uint32 hash = aui_UI::CalculateHash( rootBlock );

	aui_LdlObject dummyObject;
	dummyObject.hash = hash;

	Comparable<aui_LdlObject *> *myKey =
        s_objectListByString->Search(&dummyObject);

	if (myKey) {
		aui_LdlObject	*obj = myKey->Key();

		if (obj)
        {
			size_t len = strlen( rootBlock );

			for
            (
                aui_LdlObject * curObject = obj->next;
                curObject;
                curObject = curObject->next
            )
            {
				if ( strnicmp( rootBlock, curObject->ldlBlock, len ) == 0 )
				{

					AUI_ERRCODE errcode = SetupHeirarchyFromLeaf(
						curObject->ldlBlock,
						(aui_Region *)curObject->object );
					Assert( AUI_SUCCESS(errcode) );
					if ( !AUI_SUCCESS(errcode) ) return errcode;
				}
			}

			return AUI_ERRCODE_OK;
		}
	}

	Assert( FALSE );
	return AUI_ERRCODE_HACK;
}


AUI_ERRCODE aui_Ldl::SetupHeirarchyFromLeaf(MBCHAR * leafBlock, aui_Region * object)
{

	if ( !s_objectListByObject || !s_objectListByString ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( leafBlock != NULL && object != NULL );
	if ( !leafBlock || !object )
		return AUI_ERRCODE_INVALIDPARAM;

	MBCHAR * lastDot = strrchr(leafBlock, '.');
	if ( !lastDot )
		return AUI_ERRCODE_OK;

	if ( object->GetParent() )
		return AUI_ERRCODE_OK;

	*lastDot = '\0';

	aui_Region *parent = (aui_Region *)GetObject( leafBlock );
	Assert( parent != NULL );
	if ( !parent )
		return AUI_ERRCODE_HACK;

	parent->AddChild( object );

	AUI_ERRCODE errcode = SetupHeirarchyFromLeaf( leafBlock, parent );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) )
		return errcode;

	*lastDot = '.';

	return AUI_ERRCODE_OK;
}








aui_Region *aui_Ldl::BuildHierarchyFromRoot(MBCHAR const * rootBlock)
{

	Assert(s_objectListByObject);
	Assert(s_objectListByString);

	if ( !s_objectListByObject || !s_objectListByString )
		return NULL;

	Assert(rootBlock);
	if (rootBlock == NULL)
		return NULL;


	ldl_datablock * dataBlock = s_ldl->FindDataBlock(rootBlock, NULL);
	Assert(dataBlock);
	if (!dataBlock)
		return NULL;

	MBCHAR			*objTypeString = dataBlock->GetString(k_AUI_LDL_OBJECTTYPE);
	if (!objTypeString)
		return NULL;


	bool			isAtomic = dataBlock->GetBool(k_AUI_LDL_ATOMIC);


	char fullname[256];
	dataBlock->GetFullName(fullname);

	aui_Region *    myRegion    = NULL;
	AUI_ERRCODE		err         = BuildObjectFromType(objTypeString, fullname, &myRegion);
	Assert(err == AUI_ERRCODE_OK);
	if (err != AUI_ERRCODE_OK)
		return NULL;

	if (!isAtomic) {
		err = BuildHierarchyFromLeaf(dataBlock, myRegion);
		Assert(err == AUI_ERRCODE_OK);
		if (err != AUI_ERRCODE_OK)
			return NULL;
	}

	err = SetupHeirarchyFromRoot(rootBlock);
	Assert(err == AUI_ERRCODE_OK);
	if (err != AUI_ERRCODE_OK)
		return NULL;

	if (dataBlock->GetBool(k_AUI_LDL_DETACH))
    {
		DetachHierarchy(myRegion);
	}

    myRegion->DoneInstantiating();

	return myRegion;
}






AUI_ERRCODE aui_Ldl::BuildHierarchyFromLeaf(ldl_datablock *parent, aui_Region *region)
{
	Assert(s_objectListByObject && s_objectListByString && parent);
	if ( !s_objectListByObject || !s_objectListByString || !parent)
		return AUI_ERRCODE_INVALIDPARAM;

	PointerList<ldl_datablock> *    childList = parent->GetChildList();
	for
    (
	    PointerList<ldl_datablock>::Walker walk(childList);
        walk.IsValid();
        walk.Next()
    )
    {
	    ldl_datablock * dataBlock = walk.GetObj();
		Assert(dataBlock);
		if (dataBlock == NULL)
			return AUI_ERRCODE_INVALIDPARAM;




		MBCHAR			*objTypeString = dataBlock->GetString(k_AUI_LDL_OBJECTTYPE);
		if (!objTypeString)
			return AUI_ERRCODE_INVALIDPARAM;


		BOOL			isAtomic = dataBlock->GetBool(k_AUI_LDL_ATOMIC);


		char fullname[256];
		dataBlock->GetFullName(fullname);

		aui_Region *    myRegion;
		AUI_ERRCODE		err = BuildObjectFromType(objTypeString, fullname, &myRegion);
		Assert(err == AUI_ERRCODE_OK);
		if (err != AUI_ERRCODE_OK)
			return AUI_ERRCODE_INVALIDPARAM;




		if (!isAtomic) {
			err = BuildHierarchyFromLeaf(dataBlock, myRegion);
			Assert(err == AUI_ERRCODE_OK);
			if (err != AUI_ERRCODE_OK)
				return err;
		}

		region->AddChild(myRegion);
	}

	return AUI_ERRCODE_OK;
}































AUI_ERRCODE aui_Ldl::BuildObjectFromType(MBCHAR *typeString,
										 MBCHAR *ldlName,
										 aui_Region **theObject)
{
	AUI_ERRCODE		retval = AUI_ERRCODE_OK;
	aui_Region		*region = NULL;

	if (!stricmp(typeString, "C3Window")) {
		region = (aui_Region *) new C3Window(&retval, aui_UniqueId(), ldlName, 16);
	} else
	if(!stricmp(typeString, "ctp2_Window")) {
		region = (aui_Region *) new ctp2_Window(&retval, aui_UniqueId(), ldlName, 16);
	} else
	if (!stricmp(typeString, "C3Slider")) {
		region = (aui_Region *) new C3Slider(&retval, aui_UniqueId(), ldlName);
	} else
	if (!stricmp(typeString, "c3_Button")) {
		region = (aui_Region *) new c3_Button(&retval, aui_UniqueId(), ldlName);
	} else
	if (!stricmp(typeString, "c3_Static")) {
		region = (aui_Region *) new c3_Static(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString, "ctp2_Static")) {
		region = (aui_Region *) new ctp2_Static(&retval, aui_UniqueId(), ldlName);
	} else
	if (!stricmp(typeString, "ctp2_Button")) {
		region = (aui_Region *) new ctp2_Button(&retval, aui_UniqueId(), ldlName);
	} else
	if (!stricmp(typeString, "ctp2_ListBox")) {
		region = (aui_Region *) new ctp2_ListBox(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString, "ctp2_LineGraph")) {
		region = (aui_Region *) new LineGraph(&retval, aui_UniqueId(), ldlName);
	} else
	if (!stricmp(typeString, "ctp2_ListItem")) {
		region = (aui_Region *) new ctp2_ListItem(&retval, ldlName);
	} else
	if (!stricmp(typeString, "ctp2_DropDown")) {
		region = (aui_Region *) new ctp2_DropDown(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString,  "ctp2_Spinner")) {
		region = (aui_Region *) new ctp2_Spinner(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString, "ctp2_HyperTextBox")) {
		region = (aui_Region *)new ctp2_HyperTextBox(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString, "c3_HyperTextBox")) {
		region = (aui_Region *)new c3_HyperTextBox(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString, "ctp2_MenuBar")) {
		region = (aui_Region *) new ctp2_MenuBar(&retval, aui_UniqueId(), ldlName, 16);
	} else
	if(!stricmp(typeString, "aui_switch")) {
		region = (aui_Region *) new aui_Switch(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString, "ctp2_Switch")) {
		region = (aui_Region *) new ctp2_Switch(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString, "ctp2_TextField")) {
		region = (aui_Region *) new ctp2_TextField(&retval, aui_UniqueId(), ldlName);
	} else
	if(!stricmp(typeString, "ctp2_MenuButton")) {
		region = (aui_Region *) new ctp2_MenuButton(&retval, aui_UniqueId(), ldlName);
	}
	if(!stricmp(typeString, "ctp2_Tab")) {
		region = (aui_Region *) new ctp2_Tab(&retval, aui_UniqueId(), ldlName);
	}
	if(!stricmp(typeString, "ctp2_TabGroup")) {
		region = (aui_Region *) new ctp2_TabGroup(&retval, aui_UniqueId(), ldlName);
	}
	if(!stricmp(typeString, "ctp2_TabButton")) {
		region = (aui_Region *) new ctp2_TabButton(&retval, aui_UniqueId(), ldlName);
	}
	if(!stricmp(typeString, "RadarMap")) {
		region = (aui_Region *) new RadarMap(&retval, aui_UniqueId(), ldlName);
	}

	Assert(region);
	Assert(retval == AUI_ERRCODE_OK);
	if (!region || retval != AUI_ERRCODE_OK) {

		c3errors_ErrorDialog("aui_Ldl::BuildObjectFromType()",
							 "Auto-instantiate of '%s' failed on '%s'",
							 typeString,
							 ldlName);

		*theObject = NULL;

		return AUI_ERRCODE_INVALIDPARAM;
	}

	*theObject = region;

	return AUI_ERRCODE_OK;
}






AUI_ERRCODE aui_Ldl::DeleteHierarchyFromRoot(MBCHAR const * rootBlock)
{

	Assert(s_objectListByObject);
	Assert(s_objectListByString);

	if ( !s_objectListByObject || !s_objectListByString )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert(rootBlock);
	if (rootBlock == NULL) return AUI_ERRCODE_INVALIDPARAM;


	ldl_datablock * dataBlock   = s_ldl->FindDataBlock(rootBlock, NULL);
	Assert(dataBlock);
	if (!dataBlock) return AUI_ERRCODE_INVALIDPARAM;


	AUI_ERRCODE		errcode     = DeleteHierarchyFromLeaf(dataBlock);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK)
		return errcode;

	char fullname[256];
	dataBlock->GetFullName(fullname);
	aui_Region *region = (aui_Region *)GetObject(fullname);
	Assert(region);
	if (region) {
		delete region;
	} else {
		return AUI_ERRCODE_INVALIDPARAM;
	}

	return AUI_ERRCODE_OK;
}







AUI_ERRCODE aui_Ldl::DeleteHierarchyFromLeaf(ldl_datablock *parent)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	aui_Region		*region;
	ldl_datablock *dataBlock;

	if (parent == NULL) return AUI_ERRCODE_OK;

	PointerList<ldl_datablock> *childList = parent->GetChildList();
	PointerList<ldl_datablock>::Walker walk(childList);
	while(walk.IsValid()) {
		dataBlock = walk.GetObj();




		BOOL			isAtomic = dataBlock->GetBool(k_AUI_LDL_ATOMIC);

		if (!isAtomic) {
			errcode = DeleteHierarchyFromLeaf(dataBlock);
			Assert(errcode == AUI_ERRCODE_OK);
			if (errcode != AUI_ERRCODE_OK)
				return errcode;
		}

		char fullname[256];
		region = (aui_Region *)GetObject(dataBlock->GetFullName(fullname));
		Assert(region);
		if (!region)
			return AUI_ERRCODE_INVALIDPARAM;

		delete region;
		walk.Next();
	}

	return errcode;
}








AUI_ERRCODE aui_Ldl::SetActionFuncAndCookie
(
    MBCHAR const *                          ldlBlock,
	aui_Control::ControlActionCallback *    actionFunc,
	void *                                  cookie
)
{
	aui_Control	*   control = (aui_Control *) aui_Ldl::GetObject(ldlBlock);
    AUI_ERRCODE		errcode = control
                              ? control->SetActionFuncAndCookie(actionFunc, cookie)
                              : AUI_ERRCODE_NOCONTROL;
	Assert(errcode == AUI_ERRCODE_OK);

	return errcode;
}

AUI_ERRCODE aui_Ldl::SetActionFuncAndCookie
(
    MBCHAR const *                          parentBlock,
    MBCHAR const *                          regionBlock,
	aui_Control::ControlActionCallback *    actionFunc,
	void *                                  cookie
)
{
	Assert(parentBlock && regionBlock);
	if (!parentBlock || !regionBlock)
		return AUI_ERRCODE_INVALIDPARAM;

	MBCHAR		ldlBlock[k_AUI_LDL_MAXBLOCK + 1];
	sprintf(ldlBlock, "%s.%s", parentBlock, regionBlock);

	return SetActionFuncAndCookie(ldlBlock, actionFunc, cookie);
}


AUI_ERRCODE aui_Ldl::DetachHierarchy(aui_Region *region)
{
	if (region)
    {
	    sint32 numChildren = region->NumChildren();

	    for (sint32 i = 0; i < numChildren; i++)
        {
		    DetachHierarchy(region->GetChildByIndex(i));
	    }

	    Remove((void *)region);
    }

	return AUI_ERRCODE_OK;
}

sint32 aui_Ldl::GetIntDependent(MBCHAR const * strPtr)
{
	Assert( strPtr != NULL );
	if ( !strPtr ) return AUI_ERRCODE_INVALIDPARAM;

	sint32 width = g_ui->Width();
	sint32 height = g_ui->Height();

	for ( ; strPtr; ++strPtr)
	{
		sint32 w, h, value;
		if ( sscanf( strPtr, "%dx%d?%d", &w, &h, &value ) != 3 )
			if ( sscanf( strPtr, "%dX%d?%d", &w, &h, &value ) != 3 )
				break;

		if ( w == width && h == height ) return value;

        strPtr = strchr(strPtr, ':');
	}

	return 0;
}

void aui_Ldl::ModifyAttributes( MBCHAR const * ldlBlock, aui_Dimension * dimension)
{
	ldl_datablock * format = s_ldl->FindDataBlock(ldlBlock);

	if (format && dimension)
    {
		sint32 x      = dimension->HorizontalPositionData();
		sint32 y      = dimension->VerticalPositionData();
		sint32 width  = dimension->HorizontalSizeData();
		sint32 height = dimension->VerticalSizeData();

		if ( dimension->GetHorizontalPositionType() == AUI_DIMENSION_HPOSITION_ABSOLUTE ) {
			if ( format->AttributeNameTaken( k_AUI_LDL_HABSPOSITION ) )
				format->SetValue( k_AUI_LDL_HABSPOSITION, x );
			else
				format->AddAttribute( k_AUI_LDL_HABSPOSITION, x );
		} else {
			if ( format->AttributeNameTaken( k_AUI_LDL_HRELPOSITION ) )
				format->SetValue( k_AUI_LDL_HRELPOSITION, x );
			else
				format->AddAttribute( k_AUI_LDL_HRELPOSITION, x );
		}

		if ( dimension->GetVerticalPositionType() == AUI_DIMENSION_VPOSITION_ABSOLUTE ) {
			if ( format->AttributeNameTaken( k_AUI_LDL_VABSPOSITION ) )
				format->SetValue( k_AUI_LDL_VABSPOSITION, y );
			else
				format->AddAttribute( k_AUI_LDL_VABSPOSITION, y );
		} else {
			if ( format->AttributeNameTaken( k_AUI_LDL_VRELPOSITION ) )
				format->SetValue( k_AUI_LDL_VRELPOSITION, y );
			else
				format->AddAttribute( k_AUI_LDL_VRELPOSITION, y );
		}

		if ( dimension->GetHorizontalSizeType() == AUI_DIMENSION_HSIZE_ABSOLUTE ) {
			if ( format->AttributeNameTaken( k_AUI_LDL_HABSSIZE ) )
				format->SetValue( k_AUI_LDL_HABSSIZE, width );
			else
				format->AddAttribute( k_AUI_LDL_HABSSIZE, width );
		} else {
			if ( format->AttributeNameTaken( k_AUI_LDL_HRELSIZE ) )
				format->SetValue( k_AUI_LDL_HRELSIZE, width );
			else
				format->AddAttribute( k_AUI_LDL_HRELSIZE, width );
		}

		if ( dimension->GetVerticalSizeType() == AUI_DIMENSION_VSIZE_ABSOLUTE ) {
			if ( format->AttributeNameTaken( k_AUI_LDL_VABSSIZE ) )
				format->SetValue( k_AUI_LDL_VABSSIZE, height );
			else
				format->AddAttribute( k_AUI_LDL_VABSSIZE, height );
		} else {
			if ( format->AttributeNameTaken( k_AUI_LDL_VRELSIZE ) )
				format->SetValue( k_AUI_LDL_VRELSIZE, height );
			else
				format->AddAttribute( k_AUI_LDL_VRELSIZE, height );
		}

	}
}

ldl_datablock * aui_Ldl::FindDataBlock(MBCHAR const * ldlBlock)
{
    return s_ldl ? s_ldl->FindDataBlock(ldlBlock) : NULL;
}
