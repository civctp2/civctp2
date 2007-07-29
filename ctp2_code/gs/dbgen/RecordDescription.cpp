//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database record generator
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
// WIN32
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Implemented GovernmentsModified subclass (allowing cdb files including
//   a GovernmentsModified record to produce parsers capable of reading and
//   storing subrecords for Government types.)
//   See http://apolyton.net/forums/showthread.php?s=&threadid=107916 for
//   more details  _____ by MrBaggins Jan-04
//
//   * Added m_hasGovernmentsModified (bool,) used throughout to incorporate
//     appropriate code for each parser
//   * Code dynamically generates m_hasGovernmentsModified into the record 
//     parser, and sets it, accordingly.
//   * Code dynamically generates (along with a public accessor for 
//     m_hasGovernmentsModified,) generic accessors...
//     - sint32 GenericGetNumGovernmentsModified()
//     - sint32 GenericGetGovernmentsModifiedIndex(sint32 index)
//     because a template requires access to these properties which may or
//     may not exist in the templated class.  These functions return an
//     appropriate value in those cases.
//   * Code generates a conditional parser construct to deal with an
//     addition to the standard parsed record syntax.
//
// - Added return type void to Init function to make .NET quiet about the
//   missing return type, by Martin Gühmann.
// - Modified AddBitPair function to allow bit pairs to have default values
//   so that when two records are merged, only the bit is merged 
//   in that is set. - Sep. 28th 2004 Martin Gühmann
// - Added serilization method export. (Aug 24th 2005 Martin Gühmann)
// - Output files only have spaces instead of tabs as indent and indetion
//   was fixed. (Aug 25th 2005 Martin Gühmann)
// - Added alias names and the possibility to have default values from 
//   other entries. (Aug 26th 2005 Martin Gühmann)
// - Added accessors for slic database array access. (Sep 16th 2005 Martin Gühman)
// - Made float arrays possible. (Sep 16th 2005 Martin Gühman)
// - Made value of int databases accessable. (Sep 16th 2005 Martin Gühman)
// - If database records have no name a default name is generated. e.g.
//   DIFFICULTY_5 for the sixth entry in the DifficultyDB. (Jan 3rd 2006 Martin Gühman)
// - Added ParseNum so that a certain number of entries can be parsed if 
//   braces are missing so that the old pollution database can be supported. (July 15th 2006 Martin Gühmann)
// - Added default tokens for database records. (July 15th 2006 Martin Gühmann)
// - Added map.txt support. (27-Mar-2007 Martin Gühmann)
// - Added Const.txt support. (29-Jul-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------
#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#pragma warning(disable:4786)   // (Level ?)   identifier length over 255 (with templates)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(WIN32)
// Do not define the min and max *macros* in <windows.h>.
#define NOMINMAX	
#include <windows.h>
#else
#include "windows.h"
#endif

#include "minmax.h"
#include <algorithm>
#include <set>
#include <string>

#include "ctpdb.h"
#include "RecordDescription.h"

extern "C" int g_generateRequirementWarnings;

namespace
{
    char const *    AsString(bool a_Bool)
    {
        return (a_Bool) ? "true" : "false";
    }
}

RecordDescription::RecordDescription(char const * name, bool allowsSingleRecord)
:
	m_datumList                 (),
	m_memberClasses             (),
	m_hasGovernmentsModified    (false),
	m_numBits                   (0),
	m_addingToMemberClass       (false),
	m_baseType                  (DATUM_NONE),
	m_parseNum                  (0),
	m_preBody                   (false),
	m_allowsSingleRecord        (allowsSingleRecord)
{
	strncpy(m_name, name, k_MAX_RECORD_NAME);
}

RecordDescription::~RecordDescription()
{
}

void RecordDescription::SetBaseType(DATUM_TYPE type)
{ 
	m_baseType = type;

	Datum * l_Value     = new Datum("Value", type);
	l_Value->m_required = true;
	m_datumList.AddTail(l_Value);

	m_datumList.AddTail(new Datum("NameText", DATUM_STRING));
}

void RecordDescription::ExportHeader(FILE *outfile)
{
    // Multiple include guards
    fprintf(outfile, "\n#if defined(HAVE_PRAGMA_ONCE)\n#pragma once\n#endif\n");

    char    capitalizedName[1 + k_MAX_RECORD_NAME];
    size_t  nameLength  = std::min<size_t>(k_MAX_RECORD_NAME, strlen(m_name));
    std::transform(m_name, m_name + nameLength, capitalizedName, toupper);
    capitalizedName[nameLength] = 0;
	fprintf(outfile, "\n#ifndef %s_RECORD_H__\n#define %s_RECORD_H__\n",
			         capitalizedName, capitalizedName
           );

    // Exported class name
    fprintf(outfile, "\nclass %sRecord;\n\n", m_name);

    // Project imports
    fprintf(outfile, "#include \"CTPDatabase.h\"\n");
	fprintf(outfile, "#include \"CTPRecord.h\"\n");
    fprintf(outfile, "class CivArchive;\n");
	ExportForwardDeclarations(outfile);

    // Declarations
	size_t  tokenCount = 0;
	for (PointerList<Datum>::Walker walk(&m_datumList); walk.IsValid(); walk.Next())
    {
		if (walk.GetObj()->m_type == DATUM_BIT_PAIR)
        {
			tokenCount += 2;
        }
        else
        {
            ++tokenCount;
        }
	}

    // Tokens and flags
	fprintf(outfile, "\n#define k_Num_%sRecord_Tokens %d\n\n", m_name, tokenCount);
	ExportBits(outfile);
	ExportRanges(outfile);

    // Main class
	fprintf(outfile, "\nclass %sRecord : public CTPRecord\n{\npublic:\n", m_name);
	
	fprintf(outfile, "    typedef sint32 (%sRecord::*IntAccessor)() const;\n", m_name);
	fprintf(outfile, "    typedef bool   (%sRecord::*BoolAccessor)() const;\n", m_name);
	fprintf(outfile, "    typedef double (%sRecord::*FloatAccessor)() const ;\n", m_name);
	fprintf(outfile, "    typedef bool   (%sRecord::*BitIntAccessor)(sint32 &val) const;\n", m_name);
	fprintf(outfile, "    typedef bool   (%sRecord::*BitFloatAccessor)(double &val) const;\n", m_name);
	fprintf(outfile, "    typedef sint32 (%sRecord::*IntArrayAccessor)(sint32 index) const;\n", m_name);
	fprintf(outfile, "    typedef double (%sRecord::*FloatArrayAccessor)(sint32 index) const;\n", m_name);

	ExportMemberClasses(outfile);

	fprintf(outfile, "\nprivate:\n");
	ExportData(outfile);
	
	fprintf(outfile, "\npublic:\n");
	ExportMethods(outfile);

	fprintf(outfile, "\n}; /* %sRecord */\n\n", m_name);

    // Specials
	fprintf(outfile, "struct %sRecordAccessorInfo\n{\n", m_name);
	fprintf(outfile, "    %sRecord::IntAccessor        m_intAccessor;\n", m_name);
	fprintf(outfile, "    %sRecord::BoolAccessor       m_boolAccessor;\n", m_name);
	fprintf(outfile, "    %sRecord::FloatAccessor      m_floatAccessor;\n", m_name);
	fprintf(outfile, "    %sRecord::BitIntAccessor     m_bitIntAccessor;\n", m_name);
	fprintf(outfile, "    %sRecord::BitFloatAccessor   m_bitFloatAccessor;\n", m_name);
	fprintf(outfile, "    %sRecord::IntArrayAccessor   m_intArrayAccessor;\n", m_name);
	fprintf(outfile, "    %sRecord::FloatArrayAccessor m_floatArrayAccessor;\n", m_name);
	fprintf(outfile, "};\n\n");

    // Global variables
	fprintf(outfile, "extern %sRecordAccessorInfo      g_%sRecord_Accessors[];\n", m_name, m_name);
	fprintf(outfile, "extern CTPDatabase<%sRecord> *   g_the%sDB;\n\n", m_name, m_name);
	fprintf(outfile, "extern char * g_%s_Tokens[];\n", m_name);

    // Multiple include guard 
	fprintf(outfile, "\n#endif\n");
}

void RecordDescription::ExportForwardDeclarations(FILE *outfile)
{
    std::set<std::string>   forwardClasses;

    for
    (
	    PointerList<Datum>::Walker walk(&m_datumList);
	    walk.IsValid();
        walk.Next()
    ) 
    {
        Datum * dat = walk.GetObj();
		if (DATUM_RECORD == dat->m_type) 
        {
            forwardClasses.insert(dat->m_subType);
		}
		else if (    (DATUM_BIT_PAIR == dat->m_type) 
                  && (DATUM_RECORD   == dat->m_bitPairDatum->m_type)
                )
        {
		    forwardClasses.insert(dat->m_bitPairDatum->m_subType);
		}
	}

    for 
    (
        std::set<std::string>::const_iterator  p = forwardClasses.begin();
        p != forwardClasses.end();
        ++p
    )
    {
        if (strcmp(m_name, p->c_str()))
        {
	        fprintf(outfile, "class %sRecord;\n", p->c_str());
        }
        // else: The main class has been exported at the top
    }

	for 
    (
        PointerList<MemberClass>::Walker membWalk(&m_memberClasses);
	    membWalk.IsValid();
		membWalk.Next()
    ) 
    {
		membWalk.GetObj()->ExportForwardDeclarations(outfile);
	}
}



void RecordDescription::ExportDataCode(FILE *outfile)
{
	for 
    (
        PointerList<Datum>::Walker walk(&m_datumList);
	    walk.IsValid();
        walk.Next()
    ) 
    {
		walk.GetObj()->ExportDataCode(outfile, m_name);
	}
}

void RecordDescription::SetParseNum(sint32 parseNum)
{
	if(m_addingToMemberClass) {
		
		Assert(m_memberClasses.GetTail());
		if(m_memberClasses.GetTail()) {
			m_memberClasses.GetTail()->SetParseNum(parseNum);
		}
		return;
	}
	m_parseNum = parseNum;
}

void RecordDescription::SetPreBody(bool preBody)
{
	m_preBody = preBody;
}

void RecordDescription::AddDatum(DATUM_TYPE type, struct namelist *nameInfo,
                                 sint32 minSize, sint32 maxSize,
                                 char *subType, bool isPreBody)
{
	if(m_addingToMemberClass) {
		
		Assert(m_memberClasses.GetTail());
		if(m_memberClasses.GetTail()) {
			m_memberClasses.GetTail()->AddDatum(type, nameInfo,
												minSize, maxSize,
												subType);
		}
		return;
	}
	
	Datum *dat = new Datum(nameInfo->name, type);
	dat->m_akaName = nameInfo->akaName;
	dat->m_defaultName = nameInfo->defaultName;
	dat->m_minSize = minSize;
	dat->m_maxSize = maxSize;
	dat->m_subType = subType;
	dat->m_isPreBody = isPreBody;
	if ((!(nameInfo->flags & k_NAMEVALUE_HAS_VALUE)) &&
	    (dat->m_maxSize <= 0)
	   )
	{
		switch(dat->m_type) 
		{
		default:
			break;
		case DATUM_INT:
		case DATUM_FLOAT:
		case DATUM_STRUCT:
		case DATUM_STRING:
		case DATUM_FILE:
		case DATUM_STRINGID:
			dat->m_required = true;
			break;
		}
	}
	else
	{
		dat->SetValue(nameInfo->v);
	}
	m_datumList.AddTail(dat);

	if (dat->m_type == DATUM_BIT) 
	{
		dat->m_bitNum = m_numBits;
		m_numBits++;
	}
}

void RecordDescription::AddGroupedBits(char *groupName, struct namelist *list)
{
	if(m_addingToMemberClass) {
		Assert(m_memberClasses.GetTail());
		if(m_memberClasses.GetTail()) {
			m_memberClasses.GetTail()->AddGroupedBits(groupName, list);
		}
		return;
	}
	
	
	Datum *dat = new Datum(groupName, DATUM_BIT_GROUP);
	dat->m_groupList = list;
	m_datumList.AddTail(dat);
}

void RecordDescription::AddBitPair(struct namelist *nameInfo, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype)
{
	if(m_addingToMemberClass) {
		Assert(m_memberClasses.GetTail());
		if(m_memberClasses.GetTail()) {
			m_memberClasses.GetTail()->AddBitPair(nameInfo, minSize, maxSize, pairtype);
		}
		return;
	}


	Datum *dat = new Datum(nameInfo->name, DATUM_BIT_PAIR);
	dat->m_minSize = minSize;
	dat->m_maxSize = maxSize;
	// Added to have default values
	if((nameInfo->flags & k_NAMEVALUE_HAS_VALUE)
	|| (dat->m_maxSize > 0)
	){
		dat->SetValue(nameInfo->v);
	}
	dat->m_bitNum = m_numBits;
	m_numBits++;

    char * nameValue = (char *) malloc(strlen(dat->m_name) + strlen("Value") + 1);
    strcpy(nameValue, nameInfo->name);
    strcat(nameValue, "Value");
	Datum *pairDat = new Datum(nameValue, (DATUM_TYPE) pairtype->type);
	pairDat->m_subType = (char *)pairtype->extraData;

	dat->m_bitPairDatum = pairDat;
    m_datumList.AddTail(dat);
}


void RecordDescription::StartMemberClass(char const * name)
{
	m_memberClasses.AddTail(new MemberClass(name));
	m_addingToMemberClass = true;
}

void RecordDescription::EndMemberClass(char const * name)
{
	Assert(m_memberClasses.GetTail());
	if(m_memberClasses.GetTail()) {
		Assert(!strcmp(m_memberClasses.GetTail()->GetName(), name));
	}
	m_addingToMemberClass = false;
}

void RecordDescription::ExportBits(FILE *outfile)
{
	sint32 bit = 0;
	char nicename[k_MAX_RECORD_NAME];
	
	PointerList<Datum>::Walker walk(&m_datumList);

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next()) 
    {
		Datum *dat = walk.GetObj();
		if(dat->m_type == DATUM_BIT || dat->m_type == DATUM_BIT_PAIR) {
			if(!(bit % 32)) {
				fprintf(outfile, "//\n// m_flags%d: %s\n", bit / 32, m_name);
			}
			sprintf(nicename, "k_%s_%s_Bit", m_name, dat->m_name);
			fprintf(outfile, "#define %-40s 0x%08lx\n", nicename, 1 << (bit % 32));
			bit++;
		}
	}
	
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
    {
		Datum *dat = walk.GetObj();

		if (dat->m_type == DATUM_BIT_GROUP) 
        {
			fprintf(outfile, "//\n// m_%s bit group\n", dat->m_name);
			
			sint32 bit = 0;
            for 
            (
                struct namelist *node = dat->m_groupList; 
                node; 
                node = node->next
            ) 
            {
				sprintf(nicename, "k_%s_%s_%s_Bit", m_name, dat->m_name, node->name);
				fprintf(outfile, "#define %-40s 0x%08lx\n", nicename, 1 << bit);
				bit++;
				Assert(bit <= 32);
			}
		}
	}

	for
    (
        PointerList<MemberClass>::Walker membWalk(&m_memberClasses);
	    membWalk.IsValid();
        membWalk.Next()
    ) 
    {
		membWalk.GetObj()->ExportBits(outfile);
	}
}

void RecordDescription::ExportRanges(FILE *outfile)
{
    for 
    (
	    PointerList<Datum>::Walker walk(&m_datumList);
	    walk.IsValid();
        walk.Next()
    )
    {
		walk.GetObj()->ExportRangeDefines(outfile);
	}
}

void RecordDescription::ExportData(FILE *outfile)
{
	for (sint32 flag = 0; flag < FlagCount(); ++flag) 
    {
		fprintf(outfile, "    uint32 m_flags%d;\n", flag);
	}

	for
    (
	    PointerList<Datum>::Walker walk(&m_datumList);
	    walk.IsValid();
        walk.Next()
    ) 
    {
		if(walk.GetObj()->m_type != DATUM_BIT) {
			walk.GetObj()->ExportVariable(outfile, 0);
		}
	}

	fprintf(outfile, "    bool m_hasGovernmentsModified; // GovMod specific flag\n");
}

void RecordDescription::ExportMethods(FILE *outfile)
{
	fprintf(outfile, "    %sRecord() { Init(); };\n", m_name);
	fprintf(outfile, "    %sRecord(CivArchive &archive) { Serialize(archive); };\n", m_name);
	fprintf(outfile, "    %sRecord(%sRecord const & rval) { Init(); *this = rval; }\n", m_name, m_name);
	fprintf(outfile, "    ~%sRecord();\n", m_name);
	fprintf(outfile, "    %sRecord const & operator = (%sRecord const & rval);\n\n", m_name, m_name);

	fprintf(outfile, "    void Init();\n", m_name);
	fprintf(outfile, "    void Serialize(CivArchive &archive);\n\n", m_name);

	fprintf(outfile, "    void CheckRequiredFields(DBLexer *lex);\n");
	fprintf(outfile, "    sint32 Parse(DBLexer *lex, sint32 numRecords);\n\n");
	fprintf(outfile, "    void ResolveDBReferences();\n");
	fprintf(outfile, "    void Merge(const %sRecord & rval);\n", m_name);

	PointerList<Datum>::Walker walk(&m_datumList);

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next()) 
    {
		Datum * dat = walk.GetObj();

        if (dat->m_type == DATUM_BIT_GROUP) 
        {
			fprintf(outfile, "    sint32 Parse%sBit(DBLexer *lex);\n", dat->m_name);
		}
	}
	fprintf(outfile, "\n");
	
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next()) 
    {
		Datum * dat = walk.GetObj();

		if (strcmp("GovernmentsModified", dat->m_name) == 0) 
        {
            m_hasGovernmentsModified = true;
        }

		dat->ExportAccessor(outfile, 0, m_name);
	}

	fprintf(outfile, "    bool GetHasGovernmentsModified() const { return m_hasGovernmentsModified; }\n");
	if (m_hasGovernmentsModified) 
    {
	    fprintf(outfile, "    sint32 GenericGetNumGovernmentsModified() const { return m_numGovernmentsModified; }\n");
	    fprintf(outfile, "    sint32 GenericGetGovernmentsModifiedIndex(sint32 index) const {");
		fprintf(outfile," return GetGovernmentsModifiedIndex(index); }\n");
    }
	else
    {
	    fprintf(outfile, "    sint32 GenericGetNumGovernmentsModified() const { return 0; }\n");
	    fprintf(outfile, "    sint32 GenericGetGovernmentsModifiedIndex(sint32 index) const { return -1; }\n");
    }
}

void RecordDescription::ExportMemberClasses(FILE *outfile)
{
	for 
    (
        PointerList<MemberClass>::Walker walk(&m_memberClasses); 
        walk.IsValid(); 
        walk.Next()
    )
    {
		walk.GetObj()->ExportHeader(outfile);
	}
}
	
void RecordDescription::ExportCode(FILE *outfile)
{
    // Pre-compiled header
	fprintf(outfile, "#include \"c3.h\"\n");

    // Own declarations (consistency check)
	fprintf(outfile, "#include \"%sRecord.h\"\n\n", m_name);

    // Common include files
    fprintf(outfile, "#include <algorithm>\n");
	fprintf(outfile, "#include \"BitArray.h\"\n");
	fprintf(outfile, "#include \"c3errors.h\"\n");
	fprintf(outfile, "#include \"CTPDatabase.h\"\n");
	fprintf(outfile, "#include \"DBLexer.h\"\n");
	fprintf(outfile, "#include \"DBTokens.h\"\n");
	fprintf(outfile, "#include \"StrDB.h\"\n");
	fprintf(outfile, "\n");

    // Include files generated from input
	ExportOtherRecordIncludes(outfile);
	fprintf(outfile, "\n");

    // Generated database
	fprintf(outfile, "CTPDatabase<%sRecord> *g_the%sDB = NULL;\n\n", 
                        m_name, m_name
           );

    ExportManagement(outfile);
	ExportParser(outfile);
	ExportMerger(outfile);
	ExportDataCode(outfile);
	ExportMemberClassDataCode(outfile);
}

void RecordDescription::ExportOtherRecordIncludes(FILE *outfile)
{
    /// @todo Collect names in map to prevent multiple occurrences
	for 
    (
        PointerList<Datum>::Walker walk(&m_datumList); 
        walk.IsValid(); 
        walk.Next()
    )
    {
        Datum * dat = walk.GetObj();
		
        if (DATUM_RECORD == dat->m_type) 
        {
            if (strcmp(dat->m_subType, m_name))  // already have own declarations at the top
            {
            	fprintf(outfile, "#include \"%sRecord.h\"\n", dat->m_subType);
            }
		}
        else if (DATUM_BIT_PAIR == dat->m_type) 
        {
            if (DATUM_RECORD == dat->m_bitPairDatum->m_type) 
            {
			    fprintf(outfile, "#include \"%sRecord.h\"\n", dat->m_bitPairDatum->m_subType);
            }
		}
	}

	for 
    (
        PointerList<MemberClass>::Walker membWalk(&m_memberClasses); 
        membWalk.IsValid(); 
        membWalk.Next()
    )
	{
		membWalk.GetObj()->ExportOtherRecordIncludes(outfile);
	}
}

void RecordDescription::ExportManagement(FILE *outfile)
{
	sint32 i;
	PointerList<Datum>::Walker walk(&m_datumList);

    // Init()
    fprintf(outfile, "void %sRecord::Init()\n", m_name);
	fprintf(outfile, "{\n");
	for (i = 0; i < FlagCount(); i++) 
    {
		fprintf(outfile, "    m_flags%d = 0;\n", i);
	}
    for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
    {
		walk.GetObj()->ExportInitialization(outfile);
	}
	
	fprintf(outfile, "    //GovMod Specific flag initialization\n");
	fprintf(outfile, "    m_hasGovernmentsModified = %s;\n", AsString(m_hasGovernmentsModified));
	fprintf(outfile, "}\n\n");

    // Serialize()
    fprintf(outfile, "void %sRecord::Serialize(CivArchive &archive)\n", m_name);
	fprintf(outfile, "{\n");
	fprintf(outfile, "    if(archive.IsStoring()) {\n");

	fprintf(outfile, "        archive << m_index;\n");
	fprintf(outfile, "        if(m_name >= 0){\n");
	fprintf(outfile, "            archive << GetIDText();\n");
	fprintf(outfile, "        }\n");
	fprintf(outfile, "        else{\n");
	fprintf(outfile, "            archive << static_cast<MBCHAR*>(NULL);\n");
	fprintf(outfile, "        }\n");
	for(i = 0; i  < FlagCount(); i++) 
    {
		fprintf(outfile, "        archive << m_flags%d;\n", i);
	}
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next()) 
    {
		walk.GetObj()->ExportSerializationStoring(outfile);
	}
	fprintf(outfile, "    } else {\n");
	fprintf(outfile, "        //GovMod Specific flag initialization\n");
	fprintf(outfile, "        m_hasGovernmentsModified = %s;\n\n", AsString(m_hasGovernmentsModified));
	fprintf(outfile, "        archive >> m_index;\n");
	fprintf(outfile, "        {\n");
	fprintf(outfile, "            MBCHAR* tmpStr = NULL;\n");
	fprintf(outfile, "            archive >> tmpStr;\n");
	fprintf(outfile, "            g_theStringDB->GetStringID(tmpStr, m_name);\n");
	fprintf(outfile, "            SetTextName(g_theStringDB->GetNameStr(m_name));\n");
	fprintf(outfile, "        }\n");
	for(i = 0; i  < FlagCount(); i++) {
		fprintf(outfile, "        archive >> m_flags%d;\n", i);
	}

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		walk.GetObj()->ExportSerializationLoading(outfile);
	}

	fprintf(outfile, "    }\n");
	fprintf(outfile, "}\n\n");

    // Destructor
    fprintf(outfile, "%sRecord::~%sRecord()\n", m_name, m_name);
	fprintf(outfile, "{\n");
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		walk.GetObj()->ExportDestructor(outfile);
	}
	fprintf(outfile, "}\n\n");
	
    // Assignment operator
    fprintf(outfile, "%sRecord const & %sRecord::operator = (%sRecord const & rval)\n", 
                        m_name, m_name, m_name
           );
	fprintf(outfile, "{\n");
    fprintf(outfile, "    if (this != &rval)\n");
    fprintf(outfile, "    {\n");
	fprintf(outfile, "        m_index = rval.m_index;\n");
    for (i = 0; i < FlagCount(); ++i)
    {
		fprintf(outfile, "        m_flags%d = rval.m_flags%d;\n", i, i);
    }
    fprintf(outfile, "        m_hasGovernmentsModified = rval.m_hasGovernmentsModified;\n");
    fprintf(outfile, "\n");

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		walk.GetObj()->ExportOperatorAssignment(outfile);
	}
    fprintf(outfile, "    }\n\n");
    fprintf(outfile, "    return *this;\n");
	fprintf(outfile, "}\n\n");
}

void RecordDescription::ExportMerger(FILE *outfile)
{
	fprintf(outfile, "void %sRecord::Merge(const %sRecord & rval)",
		m_name, m_name);
	fprintf(outfile, "{\n");

	for 
    ( 
        PointerList<Datum>::Walker walk(&m_datumList);
	    walk.IsValid();
        walk.Next()
    )
    {
	    if (walk.GetObj()->m_type == DATUM_BIT_PAIR || 
	        walk.GetObj()->m_maxSize >= 0 ||
	        walk.GetObj()->m_type == DATUM_BIT_GROUP
           ) 
        {
		    walk.GetObj()->ExportMerge(outfile, m_name);
	    }
	}

	fprintf(outfile, "}\n\n");
}

void RecordDescription::ExportParser(FILE *outfile)
{
	char nicename[k_MAX_STRING];

	fprintf(outfile, "char *g_%s_Tokens[] =\n", m_name);
	fprintf(outfile, "{\n");
	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		fprintf(outfile, "    \"%s\",\n", walk.GetObj()->m_name);
		if(walk.GetObj()->m_type == DATUM_BIT_PAIR) {
			fprintf(outfile, "    \"%s\",\n", walk.GetObj()->m_bitPairDatum->m_name);
		}
		walk.Next();
	}

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		if(walk.GetObj()->m_akaName){
			fprintf(outfile, "    \"%s\",\n", walk.GetObj()->m_akaName);
		}
	}

	fprintf(outfile, "};\n\n");
	
	fprintf(outfile, "%sRecordAccessorInfo g_%sRecord_Accessors[] =\n", m_name, m_name);
	fprintf(outfile, "{\n");
	
    for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
    {
		Datum *dat = walk.GetObj();
		switch(dat->m_type) {
			case DATUM_INT:
				if(dat->m_maxSize < 0) {
					fprintf(outfile, "    { &%sRecord::Get%s, NULL, NULL, NULL, NULL, NULL, NULL },\n", m_name, dat->m_name);
				} else {
					fprintf(outfile, "    { &%sRecord::GetNum%s, NULL, NULL, NULL, NULL, &%sRecord::Get%s, NULL}, /* %s (array) */\n", m_name, dat->m_name, m_name, dat->m_name, dat->m_name);
				}
				break;
			case DATUM_RECORD:
				if(dat->m_maxSize < 0) {
					fprintf(outfile, "    { &%sRecord::Get%sIndex, NULL, NULL, NULL, NULL, NULL, NULL },\n", m_name, dat->m_name);
				} else {
					fprintf(outfile, "    { &%sRecord::GetNum%s, NULL, NULL, NULL, NULL, &%sRecord::Get%sIndex, NULL}, /* %s (array) */\n", m_name, dat->m_name, m_name, dat->m_name, dat->m_name);
				}
				break;
			case DATUM_BIT:
				fprintf(outfile, "    { NULL, &%sRecord::Get%s, NULL, NULL, NULL, NULL, NULL },\n", m_name, dat->m_name);
				break;
			case DATUM_FLOAT:
				if(dat->m_maxSize < 0) {
					fprintf(outfile, "    { NULL, NULL, &%sRecord::Get%s, NULL, NULL, NULL, NULL },\n", m_name, dat->m_name);
				} else {
					fprintf(outfile, "    { &%sRecord::GetNum%s, NULL, NULL, NULL, NULL, NULL, &%sRecord::Get%s}, /* %s (array) */\n", m_name, dat->m_name, m_name, dat->m_name, dat->m_name);
				}
				break;
			case DATUM_BIT_PAIR:
				if(dat->m_bitPairDatum->m_type == DATUM_INT) {
					if(!dat->m_hasValue){
						fprintf(outfile, "    { NULL, NULL, NULL, &%sRecord::Get%s, NULL, NULL, NULL },\n", m_name, dat->m_name);
					}
					else{
						fprintf(outfile, "    { &%sRecord::Get%s, NULL, NULL, NULL, NULL, NULL, NULL },\n", m_name, dat->m_name);
					}
				} else if(dat->m_bitPairDatum->m_type == DATUM_FLOAT) {
					if(!dat->m_hasValue){
						fprintf(outfile, "    { NULL, NULL, NULL, NULL, &%sRecord::Get%s, NULL, NULL },\n", m_name, dat->m_name);
					}
					else{
						fprintf(outfile, "    { NULL, NULL, &%sRecord::Get%s, NULL, NULL, NULL, NULL },\n", m_name, dat->m_name);
					}
				} else {
					fprintf(outfile, "    { NULL, NULL, NULL, NULL, NULL, NULL, NULL }, /* %s */\n", dat->m_name);
				}
				
			default:
				fprintf(outfile, "    { NULL, NULL, NULL, NULL, NULL, NULL, NULL }, /* %s */\n", dat->m_name);
				break;
		}
	}
	fprintf(outfile, "};\n\n");
	
	sint32 numTokens = 0;
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		Datum *dat = walk.GetObj();
		sprintf(nicename, "k_Token_%s_%s", m_name, dat->m_name);
		fprintf(outfile, "#define %-40s ((k_Token_Custom_Base) + %d)\n", nicename, numTokens);
		numTokens++;

		if(dat->m_type == DATUM_BIT_PAIR) {
			sprintf(nicename, "k_Token_%s_%s_Value", m_name, dat->m_name);
			fprintf(outfile, "#define %-40s ((k_Token_Custom_Base) + %d)\n", nicename, numTokens);
			numTokens++;
		}
	}

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next()) 
	{
		Datum *dat = walk.GetObj();
		if(dat->m_akaName){
			sprintf(nicename, "k_Token_%s_%s", m_name, dat->m_akaName);
			fprintf(outfile, "#define %-40s ((k_Token_Custom_Base) + %d)\n", nicename, numTokens);
			numTokens++;
		}
	}

	sprintf(nicename, "k_Token_%s_Max", m_name);
	fprintf(outfile, "#define %-40s ((k_Token_Custom_Base) + %d)\n\n\n", nicename, numTokens);

	fprintf(outfile, "static BitArray s_ParsedTokens(%d);\n", numTokens);

	fprintf(outfile, "void %sRecord::CheckRequiredFields(DBLexer *lex)\n", m_name);
	fprintf(outfile, "{\n");
	if (g_generateRequirementWarnings) 
	{
		for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next()) 
		{
			Datum *dat = walk.GetObj();
			if(dat->m_required) 
			{
				if(dat->m_akaName)
				{
					fprintf(outfile, "    if(!s_ParsedTokens.Bit(k_Token_%s_%s - k_Token_Custom_Base)\n", m_name, dat->m_name);
					fprintf(outfile, "    && !s_ParsedTokens.Bit(k_Token_%s_%s - k_Token_Custom_Base)\n", m_name, dat->m_akaName);
					fprintf(outfile, "    ){\n");
				}
				else
				{
					fprintf(outfile, "    if(!s_ParsedTokens.Bit(k_Token_%s_%s - k_Token_Custom_Base)) {\n", m_name, dat->m_name);
				}
				if (dat->m_defaultName)
				{
					fprintf(outfile, "        m_%s = m_%s;\n", dat->m_name, dat->m_defaultName);
				}
				else
				{
					fprintf(outfile, "        DBERROR((\"Warning: required field %s missing\"));\n", dat->m_name);
				}
				fprintf(outfile, "    }\n");
			}
		}
	}
	fprintf(outfile, "}\n");

	fprintf(outfile, "\n");
	fprintf(outfile, "sint32 %sRecord::Parse(DBLexer *lex, sint32 numRecords)\n", m_name);
	fprintf(outfile, "{\n");
	
	if (DATUM_NONE == m_baseType) 
	{
		char uppName[256];
		memset(uppName, 0, sizeof(uppName));
		for (size_t i = 0; i < strlen(m_name); ++i)
		{
			uppName[i] = static_cast<char>(toupper(m_name[i]));
		}

		fprintf(outfile, "    bool done = false;\n");
		fprintf(outfile, "    sint32 result = 0;\n");
		fprintf(outfile, "    sint32 tok;\n");
		fprintf(outfile, "    s_ParsedTokens.Clear();\n");
		fprintf(outfile, "    lex->SetTokens(g_%s_Tokens, k_Token_%s_Max);\n", m_name, m_name);
		fprintf(outfile, "    tok = lex->GetToken();\n");

		fprintf(outfile, "    if(tok == k_Token_Int) {\n");
		fprintf(outfile, "        tok = lex->GetToken(); // Accept number to make new db compatible with the old database format\n");
		fprintf(outfile, "    }\n");

		fprintf(outfile, "    if(tok != k_Token_Name) {\n");
		fprintf(outfile, "        char newName[256];\n");
		fprintf(outfile, "        sprintf(newName, \"%s_%s\", numRecords);\n", uppName, "%i");
		fprintf(outfile, "        if(!g_theStringDB->GetStringID(newName, m_name)) {\n");
		
		
		fprintf(outfile, "            g_theStringDB->InsertStr(newName, newName);\n");
		fprintf(outfile, "            if(!g_theStringDB->GetStringID(newName, m_name))\n");
		fprintf(outfile, "                SetTextName(newName);\n");
		fprintf(outfile, "        }\n");
		fprintf(outfile, "    }\n");
		fprintf(outfile, "    else{\n");
		fprintf(outfile, "        if(!g_theStringDB->GetStringID(lex->GetTokenText(), m_name)) {\n");
		
		
		fprintf(outfile, "            g_theStringDB->InsertStr(lex->GetTokenText(), lex->GetTokenText());\n");
		fprintf(outfile, "            if(!g_theStringDB->GetStringID(lex->GetTokenText(), m_name))\n");
		fprintf(outfile, "                SetTextName(lex->GetTokenText());\n");
		fprintf(outfile, "        }\n");
		fprintf(outfile, "        tok = lex->GetToken();\n");
		fprintf(outfile, "    }\n");
		fprintf(outfile, "\n");

		if(m_hasGovernmentsModified) {
		fprintf(outfile, "    // Start of GovMod Specific lexical analysis\n");
		fprintf(outfile, "    if(tok == k_Token_Modified) {\n");
		fprintf(outfile, "        do {\n");
		fprintf(outfile, "            tok = lex->PeekAhead();\n");
		fprintf(outfile, "            if(tok != k_Token_Name) {\n");
		fprintf(outfile, "                DBERROR((\"Modified record invalid- must be Government identifier or description.  No quotes, No spaces.\"));\n");
		fprintf(outfile, "                return 0;\n");
		fprintf(outfile, "            }\n");
		fprintf(outfile, "            g_theGovernmentDB->ParseRecordInArray(lex, (sint32 **)&m_GovernmentsModified, &m_numGovernmentsModified);\n");
		fprintf(outfile, "            tok = lex->GetToken();\n");
		fprintf(outfile, "        } while (tok == k_Token_ModifiedDelimiter);\n");
		fprintf(outfile, "    }\n");
		fprintf(outfile, "    // End of GovMod Specific lexical analysis\n");
		}

		ExportPreBodyTokens(outfile);

		fprintf(outfile, "    if(tok != k_Token_OpenBrace) {\n");
		if(!m_allowsSingleRecord)
		{
			fprintf(outfile, "        DBERROR((\"Missing open brace\"));\n");
			fprintf(outfile, "        return 0;\n");
		}
		else
		{
			fprintf(outfile, "        // Well if you have more time, you may find something better\n");
			fprintf(outfile, "        goto BypassToken;\n");
		}

		fprintf(outfile, "    }\n");
		fprintf(outfile, "\n");
		fprintf(outfile, "    while(!done) {\n");
		fprintf(outfile, "        tok = lex->GetToken();\n");
		if(m_allowsSingleRecord)
		{
			fprintf(outfile, "        BypassToken:\n");
		}
		fprintf(outfile, "        if(tok >= k_Token_Custom_Base && tok < k_Token_%s_Max) {\n", m_name);
		fprintf(outfile, "            s_ParsedTokens.SetBit(tok - k_Token_Custom_Base);\n");
		fprintf(outfile, "        }\n");
		fprintf(outfile, "        switch(tok) {\n");
		
		ExportTokenCases(outfile);
		
		if(m_allowsSingleRecord)
		{
			fprintf(outfile, "            case k_Token_Undefined:\n");
		}
		fprintf(outfile, "            case k_Token_CloseBrace:\n");
		fprintf(outfile, "                done = true;\n");
		fprintf(outfile, "                result = 1;\n");
		fprintf(outfile, "                break;\n");
		fprintf(outfile, "            default:\n");

		ExportDefaultToken(outfile);

		fprintf(outfile, "        }\n");
		fprintf(outfile, "    }\n");
		fprintf(outfile, "    CheckRequiredFields(lex);\n");
		fprintf(outfile, "    lex->RestoreTokens();\n");
		fprintf(outfile, "    return result;\n");
		fprintf(outfile, "}\n\n");
	}
	else
	{
		Assert(m_datumList.GetCount() == 2);
		fprintf(outfile, "    sint32 tok;\n");
		fprintf(outfile, "    tok = lex->GetToken();\n");
		fprintf(outfile, "    if(tok == k_Token_Int) {\n");
		fprintf(outfile, "        tok = lex->GetToken(); // Accept number to make new db compatible with the old database format\n");
		fprintf(outfile, "    }\n");
		fprintf(outfile, "    if(tok != k_Token_Name) {\n");
		fprintf(outfile, "        DBERROR((\"Record must start with name\"));\n");
		fprintf(outfile, "        return 0;\n");
		fprintf(outfile, "    }\n");
		fprintf(outfile, "    m_NameText = new char[strlen(lex->GetTokenText()) + 1];\n");
		fprintf(outfile, "    strcpy(m_NameText, lex->GetTokenText());\n");

		switch (m_baseType) 
		{
		case DATUM_INT:
			fprintf(outfile, "    if(!lex->GetIntAssignment(m_Value)) {\n");
			fprintf(outfile, "        DBERROR((\"Expected integer\"));\n");
			fprintf(outfile, "        return 0;\n");
			fprintf(outfile, "    }\n");
			break;
		case DATUM_FLOAT:
			fprintf(outfile, "    if(!lex->GetFloatAssignment(m_Value)) {\n");
			fprintf(outfile, "        DBERROR((\"Expected number\"));\n");
			fprintf(outfile, "        return 0;\n");
			fprintf(outfile, "    }\n");
			break;
		case DATUM_STRING:
		case DATUM_FILE:
			fprintf(outfile, "    if(!lex->GetFileAssignment(m_Value)) {\n");
			fprintf(outfile, "        DBERROR((\"Expected string\"));\n");
			fprintf(outfile, "        return 0;\n");
			fprintf(outfile, "    }\n");
			break;
		case DATUM_STRINGID:
			fprintf(outfile, "    if(!lex->GetStringIdAssignment(m_Value)) {\n");
			fprintf(outfile, "        DBERROR((\"Expected string\"));\n");
			fprintf(outfile, "        return 0;\n");
			fprintf(outfile, "    }\n");
			break;
		default:
			Assert(0);
			break;
		}
		fprintf(outfile, "    return 1;\n");
		fprintf(outfile, "}\n");
	} 
	
	ExportResolver(outfile);
	
	ExportMemberClassParsers(outfile);
	
	ExportDataParsers(outfile);
}

void RecordDescription::ExportPreBodyTokens(FILE *outfile)
{
	for
	(
	    PointerList<Datum>::Walker walk(&m_datumList);
	    walk.IsValid();
	    walk.Next()
	)
	{
		Datum *dat = walk.GetObj();
		if(dat->m_isPreBody)
		{
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "    if(tok == k_Token_Int) {\n");
					fprintf(outfile, "        s_ParsedTokens.SetBit(k_Token_%s_%s - k_Token_Custom_Base);\n", m_name, dat->m_name);
					fprintf(outfile, "        m_%s = atoi(lex->GetTokenText());\n", dat->m_name);
					fprintf(outfile, "        tok = lex->GetToken();\n");
					fprintf(outfile, "    }\n");
					break;
				default:
					Assert(0);
					break;
			}
		}
	}

	fprintf(outfile, "\n");
}

void RecordDescription::ExportTokenCases(FILE *outfile)
{
	for 
	(
	    PointerList<Datum>::Walker walk(&m_datumList); 
	    walk.IsValid(); 
	    walk.Next()
	)
	{
		Datum *dat = walk.GetObj();
		if (dat->m_type == DATUM_BIT_PAIR) {
			dat->ExportParseBitPairCase(outfile, m_name);
		}
		fprintf(outfile, "            case k_Token_%s_%s:\n", m_name, dat->m_name);
		if (dat->m_akaName)
		{
			fprintf(outfile, "            case k_Token_%s_%s:\n", m_name, dat->m_akaName);
		}
		if(dat->m_maxSize == k_MAX_SIZE_VARIABLE) {
			
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "                if(!CTPRecord::ParseIntInArray(lex, &m_%s, &m_num%s))\n", dat->m_name, dat->m_name);
					break;
				case DATUM_STRINGID:
					fprintf(outfile, "                if(!CTPRecord::ParseStringIdInArray(lex, &m_%s, &m_num%s))\n", dat->m_name, dat->m_name);
					break;
				case DATUM_FLOAT:
					fprintf(outfile, "                if(!CTPRecord::ParseFloatInArray(lex, &m_%s, &m_num%s))\n", dat->m_name, dat->m_name);
					break;
				case DATUM_STRING:
				case DATUM_FILE:
					fprintf(outfile, "                if(!CTPRecord::ParseFileInArray(lex, &m_%s, &m_num%s))\n", dat->m_name, dat->m_name);
					break;
				case DATUM_RECORD:
					fprintf(outfile, "                if(!g_the%sDB->ParseRecordInArray(lex, (sint32 **)&m_%s, &m_num%s))\n", dat->m_subType, dat->m_name, dat->m_name);
					break;
				case DATUM_STRUCT:
					fprintf(outfile, "                if(!%sRecord::%s::ParseInArray(lex, &m_%s, &m_num%s))\n", m_name, dat->m_subType, dat->m_name, dat->m_name);
					break;
				default:
					Assert(0);
					break;
			}
			fprintf(outfile, "                {\n");
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
		} else if(dat->m_maxSize > 0) {
			
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "                if(!CTPRecord::ParseIntInArray(lex, (sint32 *)m_%s, &m_num%s, k_MAX_%s)) {\n", 
							dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_STRINGID:
					fprintf(outfile, "                if(!CTPRecord::ParseStringIdInArray(lex, (sint32 *)m_%s, &m_num%s, k_MAX_%s)) {\n",
							dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_FLOAT:
					fprintf(outfile, "                if(!CTPRecord::ParseFloatInArray(lex, (double *)m_%s, &m_num%s, k_MAX_%s)) {\n", 
							dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_STRING:
				case DATUM_FILE:
					fprintf(outfile, "                if(!CTPRecord::ParseFileInArray(lex, (char **)m_%s, &m_num%s, k_MAX_%s)) {\n", 
							dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_RECORD:
					fprintf(outfile, "                if(!g_the%sDB->ParseRecordInArray(lex, (sint32 *)m_%s, &m_num%s, k_MAX_%s)) {\n", 
							dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_STRUCT:
					fprintf(outfile, "                if(!%sRecord::%s::ParseInArray(lex, (sint32 *)m_%s, &m_num%s, k_MAX_%s)) {\n", 
							m_name, dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
					break;
				default:
					Assert(0);
					break;
			}
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
		} else if(dat->m_maxSize < 0) {
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "                if(!lex->GetIntAssignment(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "                    DBERROR((\"Expected integer\"));\n");
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				case DATUM_STRINGID:
					fprintf(outfile, "                if(!lex->GetStringIdAssignment(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "                    DBERROR((\"Expected string ID\"));\n");
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				case DATUM_BIT:
					fprintf(outfile, "                m_flags%d |= k_%s_%s_Bit;\n", dat->m_bitNum / 32,
							m_name, dat->m_name);
					break;
				case DATUM_BIT_PAIR:
					fprintf(outfile, "                m_flags%d |= k_%s_%s_Bit;\n", dat->m_bitNum / 32,
							m_name, dat->m_name);
					dat->ExportBitPairDirectParse(outfile, m_name);
					break;
				case DATUM_FLOAT:
					fprintf(outfile, "                if(!lex->GetFloatAssignment(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "                    DBERROR((\"Expected number\"));\n");
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				case DATUM_STRING:
				case DATUM_FILE:
					fprintf(outfile, "                if(!lex->GetFileAssignment(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "                    DBERROR((\"Expected string\"));\n");
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				case DATUM_RECORD:
					fprintf(outfile, "                if (!g_the%sDB->GetRecordFromLexer(lex, m_%s)) {\n", dat->m_subType, dat->m_name);
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				case DATUM_STRUCT:
					fprintf(outfile, "                if(!m_%s.Parse(lex)) {\n", dat->m_name);
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				case DATUM_BIT_GROUP:
					fprintf(outfile, "                if(!Parse%sBit(lex)) {\n", dat->m_name);
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				default:
					Assert(0);
					break;
			}
		}
		fprintf(outfile,             "                break;\n");
	}
}

void RecordDescription::ExportDefaultToken(FILE *outfile)
{
	PointerList<Datum>::Walker walk(&m_datumList);
	Datum *dat = walk.GetObj();
	if(dat->m_maxSize == k_MAX_SIZE_VARIABLE) {
			
		switch(dat->m_type) {
			case DATUM_INT:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseIntInArray(lex, &m_%s, &m_num%s)) {\n", dat->m_name, dat->m_name);
				break;
			case DATUM_STRINGID:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseStringIdInArray(lex, m_%s, &m_num%s)) {\n", dat->m_name, dat->m_name);
				break;
			case DATUM_FLOAT:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseFloatInArray(lex, &m_%s, &m_num%s)) {\n", dat->m_name, dat->m_name);
				break;
			case DATUM_STRING:
			case DATUM_FILE:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseFileInArray(lex, &m_%s, &m_num%s)) {\n", dat->m_name, dat->m_name);
				break;
			case DATUM_RECORD:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!g_the%sDB->ParseRecordInArray(lex, &m_%s, &m_num%s)) {\n", dat->m_subType, dat->m_name, dat->m_name);
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "                if(!%sRecord::%s::ParseInArray(lex, &m_%s, &m_num%s)) {\n", m_name, dat->m_subType, dat->m_name, dat->m_name);
				break;
			default:
				Assert(0);
				break;
		}
		fprintf(outfile,         "                    DBERROR((\"Unknown token\"));\n");
		fprintf(outfile,         "                    done = true; break;\n");
		fprintf(outfile,         "                }\n");
	} else if(dat->m_maxSize > 0) {

		switch(dat->m_type) {
			case DATUM_INT:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseIntInArray(lex, (sint32 *)m_%s, &m_num%s, k_MAX_%s)) {\n", 
						dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_STRINGID:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseStringIdInArray(lex, (sint32 *)m_%s, &m_num%s, k_MAX_%s)) {\n",
						dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_FLOAT:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseFloatInArray(lex, (double *)m_%s, &m_num%s, k_MAX_%s)) {\n", 
						dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_STRING:
			case DATUM_FILE:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseFileInArray(lex, (char **)m_%s, &m_num%s, k_MAX_%s)) {\n", 
						dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_RECORD:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!g_the%sDB->ParseRecordInArray(lex, (sint32 *)m_%s, &m_num%s, k_MAX_%s)) {\n", 
						dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "                if(!%sRecord::%s::ParseInArray(lex, (sint32 *)m_%s, &m_num%s, k_MAX_%s)) {\n", 
						m_name, dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
				break;
			default:
				Assert(0);
				break;
		}
		fprintf(outfile,         "                    DBERROR((\"Unknown token\"));\n");
		fprintf(outfile,         "                    done = true; break;\n");
		fprintf(outfile,         "                }\n");
	} else if(dat->m_maxSize < 0) {
		switch(dat->m_type) {
			case DATUM_INT:
				fprintf(outfile, "                if(!lex->GetInt(m_%s)) {\n", dat->m_name);
				fprintf(outfile, "                    DBERROR((\"Unknown token\"));\n");
				fprintf(outfile, "                    done = true; break;\n");
				fprintf(outfile, "                }\n");
				break;
			case DATUM_STRINGID:
				fprintf(outfile, "                if(!lex->GetStringId(m_%s)) {\n", dat->m_name);
				fprintf(outfile, "                    DBERROR((\"Unknown token\"));\n");
				fprintf(outfile, "                    done = true; break;\n");
				fprintf(outfile, "                }\n");
				break;
			case DATUM_BIT:
				fprintf(outfile, "                m_flags%d |= k_%s_%s_Bit;\n", dat->m_bitNum / 32,
						m_name, dat->m_name);
				break;
			case DATUM_BIT_PAIR:
				fprintf(outfile, "                m_flags%d |= k_%s_%s_Bit;\n", dat->m_bitNum / 32,
						m_name, dat->m_name);
				dat->ExportBitPairDirectParse(outfile, m_name);
				break;
			case DATUM_FLOAT:
				fprintf(outfile, "                if(!lex->GetFloat(m_%s)) {\n", dat->m_name);
				fprintf(outfile, "                    DBERROR((\"Unknown token\"));\n");
				fprintf(outfile, "                    done = true; break;\n");
				fprintf(outfile, "                }\n");
				break;
			case DATUM_STRING:
			case DATUM_FILE:
				fprintf(outfile, "                if(!lex->GetFile(m_%s)) {\n", dat->m_name);
				fprintf(outfile, "                    DBERROR((\"Unknown token\"));\n");
				fprintf(outfile, "                    done = true; break;\n");
				fprintf(outfile, "                }\n");
				break;
			case DATUM_RECORD:
				fprintf(outfile, "                if(!g_the%sDB->GetCurrentRecordFromLexer(lex, m_%s)) {\n", dat->m_subType, dat->m_name);
				fprintf(outfile, "                    DBERROR((\"Unknown token\"));\n");
				fprintf(outfile, "                    done = true; break;\n");
				fprintf(outfile, "                }\n");
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "                if(!m_%s.Parse(lex)) {\n", dat->m_name);
				fprintf(outfile, "                    DBERROR((\"Unknown token\"));\n");
				fprintf(outfile, "                    done = true; break;\n");
				fprintf(outfile, "                }\n");
				break;
			case DATUM_BIT_GROUP:
				fprintf(outfile, "                if(!Parse%sBit(lex)) {\n", dat->m_name);
				fprintf(outfile, "                    DBERROR((\"Unknown token\"));\n");
				fprintf(outfile, "                    done = true; break;\n");
				fprintf(outfile, "                }\n");
				break;
			default:
				Assert(0);
				break;
		}
	}
	fprintf(outfile,             "                break;\n");
}

void RecordDescription::ExportMemberClassParsers(FILE *outfile)
{
	for 
    (
        PointerList<MemberClass>::Walker walk(&m_memberClasses); 
	    walk.IsValid();
        walk.Next()
    ) 
    {
		walk.GetObj()->ExportInitialization(outfile, m_name);
		walk.GetObj()->ExportParser(outfile, m_name);
	}
}
	
void RecordDescription::ExportDataParsers(FILE *outfile)
{
    for
    (
        PointerList<Datum>::Walker walk(&m_datumList); 
        walk.IsValid(); 
        walk.Next()
    )
    {
		Datum * dat = walk.GetObj();

		switch (dat->m_type)
        {
        default:
            break;
        
        case DATUM_BIT_GROUP: 
    		dat->ExportBitGroupParser(outfile, m_name);
            break;
		}
	}
}

void RecordDescription::ExportMemberClassDataCode(FILE *outfile)
{
	for
    (
        PointerList<MemberClass>::Walker walk(&m_memberClasses);
	    walk.IsValid();
        walk.Next()
    )
    {
        walk.GetObj()->ExportDataCode(outfile, m_name);
	}
}


void RecordDescription::ExportResolver(FILE *outfile)
{
	fprintf(outfile, "void %sRecord::ResolveDBReferences()\n", m_name);
	fprintf(outfile, "{\n");

	for 
    (
        PointerList<Datum>::Walker walk(&m_datumList);
	    walk.IsValid();
        walk.Next()
    ) 
    {
		Datum * dat = walk.GetObj();

		switch (dat->m_type)
        {
        default:
            break;
        
        case DATUM_BIT_PAIR: 
        case DATUM_RECORD:
        case DATUM_STRUCT:
			dat->ExportResolver(outfile);
            break;
		}
	}

	fprintf(outfile, "}\n");
}

