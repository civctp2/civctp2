#include "c3.h"

#include "ldl_file.hpp"
#include "ldl_data.hpp"
#include "ldl_attr.hpp"

#include "ldlif.h"

ldl_datablock::ldl_datablock(PointerList<char> *templateNames)
:
    m_templates     (),
    m_children      (),
    m_attributes    (),
    m_parent        (NULL),
    m_name          (NULL),
    m_hash          (0)
{
	PointerList<char>::Walker walk(templateNames);

	Assert(walk.IsValid());
	m_name = walk.GetObj();
	walk.Next();

	while(walk.IsValid()) {
		ldl_datablock *temp = ldlif_find_block(walk.GetObj());
		if(temp) {
			m_templates.AddTail(temp);
		} else {
			ldlif_log("Unknown template %s\n", walk.GetObj());
		}
		walk.Next();
	}
}

ldl_datablock::ldl_datablock(ldl_datablock *copy)
:
    m_templates     (),
    m_children      (),
    m_attributes    (),
    m_parent        (NULL),
    m_name          (copy->m_name),
    m_hash          (0)
{
	for
	(
	    ldl_attribute * attr = copy->m_attributes.GetHead();
	    attr;
	    attr = copy->m_attributes.GetNext(attr)
	)
	{
		m_attributes.AddTail(attr->GetCopy());
	}

	PointerList<ldl_datablock>::Walker bwalk(&copy->m_children);
	for(; bwalk.IsValid(); bwalk.Next()) {
		AddChild(new ldl_datablock(bwalk.GetObj()));
	}

	for(bwalk.SetList(&copy->m_templates); bwalk.IsValid(); bwalk.Next()) {
		m_templates.AddTail(bwalk.GetObj());
	}
}

ldl_datablock::ldl_datablock(sint32 hash)
:
    m_templates     (),
    m_children      (),
    m_attributes    (),
    m_parent        (NULL),
    m_name          (NULL),
    m_hash          (hash)
{ }

ldl_datablock::ldl_datablock(ldl *theLdl, char const * name)
:
    m_templates     (),
    m_children      (),
    m_attributes    (),
    m_parent        (NULL),
    m_name          (ldlif_getnameptr(name)),
    m_hash          (0)
{
	ldlif_add_block_to_tree(this);
}

ldl_datablock::~ldl_datablock()
{
	m_children.DeleteAll();
	if(GetName())
		ldlif_remove_block_from_tree(this);
}

char *ldl_datablock::GetFullName(char *output)
{
	if (m_parent)
	{
		m_parent->GetFullName(output);
		strcat(output, ".");
		strcat(output, m_name);
	}
	else
	{
		strcpy(output, m_name);
	}

	return output;
}

bool ldl_datablock::ContstructFullName(
		char *szName,
		ldl_datablock *dbParent,
		char *result )
{
	GetFullName(result);

	return true;
}

ldl_attribute *ldl_datablock::GetLastAttribute( void )
{
	return m_attributes.GetTail();
}

ldl_attribute *ldl_datablock::GetAttribute( const char *szName )
{
	char * strPtr = ldlif_getnameptr(szName);

	for
	(
	    ldl_attribute * atr = m_attributes.GetHead();
	    atr;
	    atr = m_attributes.GetNext(atr)
	)
	{
		if (atr->GetName() == strPtr)
		{
			return atr;
		}
	}

	return NULL;
}

int ldl_datablock::GetAttributeType(const char *szName)
{
	ldl_attribute *atr;

	char *strPtr = ldlif_getnameptr(szName);

	for (atr = m_attributes.GetHead(); atr; atr = m_attributes.GetNext(atr)) {
		if (strPtr == atr->GetName()) {
			return atr->GetType();
		}
	}

	return 0;
}

void ldl_datablock::Dump(sint32 indent) {

	PointerList<ldl_datablock>::Walker bwalk;
	ldlif_indent_log(indent);

	ldlif_log("%s", m_name);

	for(bwalk.SetList(&m_templates); bwalk.IsValid(); bwalk.Next()) {
		ldlif_log(":%s", bwalk.GetObj()->GetName());
	}
	ldlif_log(" {\n");

	ldl_attribute *attr = m_attributes.GetHead();
	for(; attr ; attr = m_attributes.GetNext(attr)) {
		ldlif_indent_log(indent);
		ldlif_log("    %s %s %s\n", attr->GetTypeName(),
				  attr->GetName(),
				  attr->GetValueText());
	}

	for(bwalk.SetList(&m_children); bwalk.IsValid(); bwalk.Next()) {
		bwalk.GetObj()->Dump(indent + 1);
	}

	ldlif_indent_log(indent);
	ldlif_log("}\n");
}

bool ldl_datablock::AttributeNameTaken(char *szName)
{
	return GetAttribute(szName) != NULL;
}

void ldl_datablock::SetValue(char *name, int value)
{
	ldl_attribute *atr = GetAttribute(name);
	if(atr) {
		Assert(atr->GetType() == ATTRIBUTE_TYPE_INT);
		((ldl_attributeValue<int> *)atr)->SetValue(value);
	}
}

void ldl_datablock::AddTemplateChildren()
{
	PointerList<ldl_datablock>::Walker bwalk;
	for(bwalk.SetList(&m_templates); bwalk.IsValid(); bwalk.Next()) {
		CopyAttributesFrom(bwalk.GetObj());
		bwalk.GetObj()->AddTemplateChildrenTo(this);
	}
}

void ldl_datablock::AddTemplateChildrenTo(ldl_datablock *block)
{
	PointerList<ldl_datablock>::Walker bwalk;
	for(bwalk.SetList(&m_children); bwalk.IsValid(); bwalk.Next()) {

		PointerList<ldl_datablock>::Walker chwalk(&block->m_children);
		bool alreadyHaveIt = false;
		for(;chwalk.IsValid(); chwalk.Next()) {

			if(chwalk.GetObj()->GetName() == bwalk.GetObj()->GetName()) {

				chwalk.GetObj()->CopyAttributesFrom(bwalk.GetObj());

				PointerList<ldl_datablock>::Walker twalk(&bwalk.GetObj()->m_templates);
				for(; twalk.IsValid(); twalk.Next()) {
					chwalk.GetObj()->m_templates.AddTail(twalk.GetObj());
				}

				chwalk.GetObj()->AddTemplateChildren();

				alreadyHaveIt = true;
				break;
			}
		}
		if(!alreadyHaveIt) {
			ldl_datablock *newblock = new ldl_datablock(bwalk.GetObj());
			block->AddChild(newblock);
			newblock->AddTemplateChildren();
			ldlif_add_block_to_tree(newblock);
		}
	}
}

void ldl_datablock::CopyAttributesFrom(ldl_datablock *templ)
{
	ldl_attribute *attr = templ->m_attributes.GetHead();
	for(; attr; attr = templ->m_attributes.GetNext(attr)) {
		if(!m_attributes.FindAttribute(attr->GetName())) {
			m_attributes.AddTail(attr->GetCopy());
		}
	}

	PointerList<ldl_datablock>::Walker tcwalk(&templ->m_children);
	for(; tcwalk.IsValid(); tcwalk.Next()) {
		PointerList<ldl_datablock>::Walker cwalk(&m_children);
		bool foundIt = false;
		for(; cwalk.IsValid(); cwalk.Next()) {
			if(cwalk.GetObj()->GetName() == tcwalk.GetObj()->GetName()) {
				cwalk.GetObj()->CopyAttributesFrom(tcwalk.GetObj());
				foundIt = true;
				break;
			}
		}
		if(!foundIt) {
			ldl_datablock *newblock = new ldl_datablock(tcwalk.GetObj());
			newblock->CopyAttributesFrom(tcwalk.GetObj());
			AddChild(newblock);
			ldlif_add_block_to_tree(newblock);
		}
	}
}
