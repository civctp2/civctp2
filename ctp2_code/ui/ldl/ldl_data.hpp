//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  :
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef LDL_DATA_HPP
#define LDL_DATA_HPP

class ldl_datablock;

#include "pointerlist.h"
#include "ldl_list.h"

class ldl;
class ldl_hash;
class ldl_attribute;
class ldl_filelist;
template <class DataT> class ldl_attributeValue;

//#define k_MAX_DATABLOCK_NAME_LEN			256
//#define k_MAX_DATABLOCK_FULL_NAME_LEN		256
#define k_MAX_DATABLOCK_NAME_LEN			64
#define k_MAX_DATABLOCK_FULL_NAME_LEN		160

class ldl_datablock {
  private:
	PointerList<ldl_datablock> m_templates;
	PointerList<ldl_datablock> m_children;
	ldl_attributelist m_attributes;
	ldl_datablock *m_parent;
	char *m_name;
	sint32 m_hash;

  public:
	ldl_datablock(PointerList<char> *templateNames);
	ldl_datablock(ldl_datablock *tempChild);
	ldl_datablock(sint32 hash);
	ldl_datablock(ldl *theLdl, char const * name);

	~ldl_datablock();
	const char *GetName() { return m_name; }
	PointerList<ldl_datablock> *GetChildList() { return &m_children; }

	void SetHash(sint32 hash) { m_hash = hash;}
	sint32 GetHash() { return m_hash; }

	// Add a child block
	void AddChild(ldl_datablock *child) {
		m_children.AddTail(child);
		child->m_parent = this;
	}

	// Add a single attribute
	void AddAttribute(ldl_attribute *attr) {
		m_attributes.AddTail(attr);
	}

	void AddAttribute(char *name, char *str) {
		m_attributes.AddTail(new ldl_attributeValue<char *>(name, ATTRIBUTE_TYPE_STRING, str));
	}
	void AddAttribute(char *name, int value) {
		m_attributes.AddTail(new ldl_attributeValue<int>(name, ATTRIBUTE_TYPE_INT, value));
	}

	void CopyAttributesFrom(ldl_datablock *templ);

	void AddTemplateChildren();
	void AddTemplateChildrenTo(ldl_datablock *block);

	void Dump(sint32 indent);

	char *GetFullName(char *output);

public:

	// get the full ldl path name
	bool ContstructFullName( char *szName, ldl_datablock *dbParent, char *result );

	// returns a pointer to the attribute for the given name, error only if bMustExist = TRUE
	ldl_attribute *GetAttribute(const char *szName);

	// returns a pointer to the last attribute in the list (to preserve order)
	ldl_attribute *GetLastAttribute( void );

	// returns the attribute value or 0 if the attribute can't be found.
	int GetAttributeType(const char *szName);

	// function used for templates -only-
	bool AttributeNameTaken(char *szName);
	void SetValue(char *name, int value);

/* ------ functions found in ldl_data_info.cpp ------ */
	// used to return datablock values.
	int     GetInt(const char *szName = "int");
	double  GetDouble(const char *szName = "double");
	bool    GetBool(const char *szName = "bool");
//	RECT GetRect(const char *x1 = "rect_x1", const char *y1 = "rect_y1",
//	             const char *x2 = "rect_x2", const char *y2 = "rect_y2");
//	POINT GetPoint(const char *x1 = "point_x", const char *y1 = "point_y");
	char *  GetString(const char *szName);
};




#endif //LDL_DATA_HPP
