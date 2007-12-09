//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database substructs record generator
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
// - Members of substructs can now have default values, added by Martin Gühmann.
// - Modified AddBitPair function to allow bit pairs to have default values
//   so that when two records are merged, only the bit is merged 
//   in that is set. - Sep. 28th 2004 Martin Gühmann
// - Added serilization method export. (Aug 24th 2005 Martin Gühmann)
// - Output files only have spaces instead of tabs as indent and indention
//   was fixed. (Aug 25th 2005 Martin Gühmann)
// - Added alias names. (Aug 26th 2005 Martin Gühmann)
// - Costum structs can now include other custom structs, given both are
//   direct members of the record class (Support for DiffDB). (Sep 15th 2005 Martin Gühmann)
// - Parser for struct ADVANCE_CHANCES of DiffDB.txt can now be generated. (Jan 3rd 2006 Martin Gühmann)
// - Fixed subsubstruct generation so that it can be used in DiffDB.txt. (April 29th 2006 Martin Gühmann)
// - Added ParseNum so that a certain number of entries can be parsed if 
//   braces are missing so that the old pollution database can be supported. (July 15th 2006 Martin Gühmann)
// - Added map.txt support. (27-Mar-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(WIN32)
#include <windows.h>
#else
#include "windows.h"
#endif

#include "ctpdb.h"
#include "RecordDescription.h"

MemberClass::MemberClass(char const * name)
{
	strcpy(m_name, name);
	m_numBits = 0;
	m_parseNum = 0;
}

MemberClass::~MemberClass()
{
}

void MemberClass::AddDatum(DATUM_TYPE type, struct namelist *nameInfo,
                           sint32 minSize, sint32 maxSize,
                           char *subType)
{
	
	Datum *dat = new Datum(nameInfo->name, type);
	dat->m_akaName = nameInfo->akaName;
	dat->m_defaultName = nameInfo->defaultName;
	dat->m_minSize = minSize;
	dat->m_maxSize = maxSize;
	dat->m_subType = subType;

	//Added by Martin Gühmann to allow struct members to have default values
	if(!(nameInfo->flags & k_NAMEVALUE_HAS_VALUE)
	&&   maxSize <= 0
	){
		switch(type)
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

void MemberClass::AddGroupedBits(char *name, struct namelist *list)
{
	Assert("Group Bits in Member classes not yet supported" == 0);
}

//Added by Martin Gühmann
void MemberClass::AddBitPair(struct namelist *nameInfo, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype)
{
	Datum *dat = new Datum(nameInfo->name, DATUM_BIT_PAIR);
	dat->m_minSize = minSize;
	dat->m_maxSize = maxSize;
	// Added by Martin Gühmann for adding default values
	if((nameInfo->flags & k_NAMEVALUE_HAS_VALUE)
	|| (maxSize > 0)
	){
		dat->SetValue(nameInfo->v);
	}
	dat->m_bitNum = m_numBits;
	m_numBits++;

	char * l_Name = (char *) malloc(strlen(nameInfo->name) + strlen("Value") + 1);
	strcpy(l_Name, nameInfo->name);
	strcat(l_Name, "Value");
	Datum * pairDat = new Datum(l_Name, (DATUM_TYPE) pairtype->type);
	pairDat->m_subType = (char *) pairtype->extraData;

	dat->m_bitPairDatum = pairDat;
	m_datumList.AddTail(dat);
}

void MemberClass::ExportHeader(FILE *outfile)
{
	fprintf(outfile, "\n    class %s {\n    private:\n", m_name);
	
	ExportData(outfile);

	fprintf(outfile, "\n    public:\n");

	ExportMethods(outfile);

	fprintf(outfile, "\n    }; /* class %s */\n", m_name);
}

void MemberClass::ExportBits(FILE *outfile)
{
	sint32 bit = 0;
	char nicename[k_MAX_RECORD_NAME];

	
	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		Datum *dat = walk.GetObj();
		if(dat->m_type == DATUM_BIT || dat->m_type == DATUM_BIT_PAIR) {
			if(!(bit % 32)) {
				fprintf(outfile, "//\n// m_flags%d: %s\n", bit / 32, m_name);
			}
			sprintf(nicename, "k_%s_%s_Bit", m_name, dat->m_name);
			fprintf(outfile, "#define %-40s 0x%08lx\n", nicename, 1 << (bit % 32));
			bit++;
		}
		walk.Next();
	}

	
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
    {
		Datum *dat = walk.GetObj();
		if(dat->m_type == DATUM_BIT_GROUP) {
			sint32 bit = 0;
			fprintf(outfile, "//\n// m_%s bit group\n", dat->m_name);
			struct namelist *node = dat->m_groupList;
			while(node) {
				sprintf(nicename, "k_%s_%s_%s_Bit", m_name, dat->m_name, node->name);
				fprintf(outfile, "#define %-40s 0x%08lx\n", nicename, 1 << bit);
				bit++;
				Assert(bit <= 32);
				node = node->next;
			}
		}
	}
}

void MemberClass::ExportData(FILE *outfile)
{
	for (sint32 flag = 0; flag < FlagCount(); ++flag) 
	{
		fprintf(outfile, "        uint32            m_flags%d;\n", flag);
	}

	for 
	(
		PointerList<Datum>::Walker walk(&m_datumList);
		walk.IsValid();
		walk.Next()
	)
	{
		if (walk.GetObj()->m_type != DATUM_BIT) 
		{
			walk.GetObj()->ExportVariable(outfile, 1);
		}
	}
}

void MemberClass::ExportMethods(FILE *outfile)
{
	PointerList<Datum>::Walker walk(&m_datumList);
	bool const  l_HasItems  = walk.IsValid();

	// Constructors
	fprintf(outfile,     "        %s();\n", m_name);
	fprintf(outfile,     "        %s(CivArchive & archive){ Serialize(archive); };\n", m_name);
	/// @todo Check copy constructor
	// Destructor
	fprintf(outfile,     "        ~%s();\n", m_name);
	// Assignment
	if (l_HasItems)
	{
		fprintf(outfile, "        %s const & operator = (%s const & rval);\n", m_name, m_name);
	}
	else
	{
		// No action: use compiler provided default
	}

	// Equality
	fprintf(outfile,     "        bool operator == (%s const & rval) {\n", m_name);
	bool record_element_found = false;
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next()) 
	{
		if (walk.GetObj()->m_type == DATUM_RECORD) 
		{
			fprintf(outfile, "          if (m_%s != rval.m_%s)\n",
					walk.GetObj()->m_name, walk.GetObj()->m_name);
			fprintf(outfile, "             return false;\n"); 
			record_element_found = true;
		}
	}
	fprintf(outfile,     "          return %s;\n", record_element_found ? "true" : "false");
	fprintf(outfile,     "        }\n\n");

	walk.SetList(&m_datumList);
	bool canParseSequentially = true;
	for 
	(
		walk.SetList(&m_datumList); 
		walk.IsValid() && canParseSequentially; 
		walk.Next() 
	)
	{
		switch(walk.GetObj()->m_type) {
			case DATUM_INT:
			case DATUM_FLOAT:
			case DATUM_STRING:
			case DATUM_FILE:
			case DATUM_RECORD:
				break;
			default:
				canParseSequentially = false;
				break;
		}
	}

	// Specials
	fprintf(outfile,     "        void Serialize(CivArchive &archive);\n");
	fprintf(outfile,     "        sint32 Parse(DBLexer *lex);\n");
	if(canParseSequentially) {
		fprintf(outfile, "        sint32 ParseSequential(DBLexer *lex);\n");
		fprintf(outfile, "        sint32 ParseFullySequential(DBLexer *lex);\n");
	}
	if(m_parseNum > 0){
		fprintf(outfile, "        sint32 ParseNum(DBLexer *lex);\n");
	}
	fprintf(outfile,     "        static sint32 ParseInArray(DBLexer *lex, %s **array, sint32 *numElements);\n", m_name);
	fprintf(outfile,     "        static sint32 ParseInArray(DBLexer *lex, %s *array, sint32 *numElements, sint32 maxSize);\n", m_name);
	if(canParseSequentially) {
		fprintf(outfile, "        static sint32 ParseInArraySequential(DBLexer *lex, %s **array, sint32 *numElements);\n", m_name);
		fprintf(outfile, "        static sint32 ParseInArraySequential(DBLexer *lex, %s *array, sint32 *numElements, sint32 maxSize);\n", m_name);
	}
	fprintf(outfile,     "        void ResolveDBReferences();\n\n");

	
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		walk.GetObj()->ExportAccessor(outfile, 1, m_name);
	}
}

void MemberClass::ExportForwardDeclarations(FILE *outfile)
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
		    fprintf(outfile, "class %sRecord;\n", dat->m_subType);
		}
		else if (DATUM_BIT_PAIR == dat->m_type) 
        {
            if (DATUM_RECORD == dat->m_bitPairDatum->m_type)
            {
		        fprintf(outfile, "class %sRecord;\n", dat->m_bitPairDatum->m_subType);
            }
		}
	}
}

void MemberClass::ExportInitialization(FILE *outfile, char *recordName)
{
	PointerList<Datum>::Walker walk(&m_datumList);
    bool const l_HasItems   = walk.IsValid();

	fprintf(outfile, "%sRecord::%s::%s()\n", recordName, m_name, m_name);
	fprintf(outfile, "{\n");

	
	sint32 i;
	for(i = 0; i  < FlagCount(); i++) 
    {
		fprintf(outfile, "    m_flags%d = 0;\n", i);
	}

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next()) 
    {
		walk.GetObj()->ExportInitialization(outfile);
	}
	fprintf(outfile, "}\n\n");

	
	fprintf(outfile, "%sRecord::%s::~%s()\n", recordName, m_name, m_name);
	fprintf(outfile, "{\n");
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
    {
		walk.GetObj()->ExportDestructor(outfile);
	}
	fprintf(outfile, "}\n\n");

	if (l_HasItems)
    {
	    fprintf(outfile, "%sRecord::%s const & %sRecord::%s::operator = (%s const & rval)\n", 
                recordName, m_name, recordName, m_name, m_name
               );
	    fprintf(outfile, "{\n");
        fprintf(outfile, "    if (this != &rval)\n");
        fprintf(outfile, "    {\n");
        for (i = 0; i < FlagCount(); ++i)
        {
		    fprintf(outfile, "        m_flags%d = rval.m_flags%d;\n", i, i);
        }
        fprintf(outfile, "\n");
	    for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
        {
		    walk.GetObj()->ExportOperatorAssignment(outfile);
	    }
        fprintf(outfile, "    }\n\n");
        fprintf(outfile, "    return *this;\n");
	    fprintf(outfile, "}\n\n");
    }
    else
    {
        // No action: use compiler provided default
    }

	fprintf(outfile, "void %sRecord::%s::Serialize(CivArchive &archive)\n", recordName, m_name, m_name);
	fprintf(outfile, "{\n");

	fprintf(outfile, "    if(archive.IsStoring()) {\n");

	for(i = 0; i < FlagCount(); ++i) 
    {
		fprintf(outfile, "        archive << m_flags%d;\n", i);
	}

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
    {
		walk.GetObj()->ExportSerializationStoring(outfile);
	}

	fprintf(outfile, "    } else {\n");

	for(i = 0; i  < FlagCount(); i++) {
		fprintf(outfile, "        archive >> m_flags%d;\n", i);
	}

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		walk.GetObj()->ExportSerializationLoading(outfile);
	}

	fprintf(outfile, "    }\n");

	fprintf(outfile, "}\n\n");
}

void MemberClass::ExportParser(FILE *outfile, char *recordName)
{
	char nicename[k_MAX_STRING];
	sint32 numTokens = 0;

	// TODO add aka and default names.

	fprintf(outfile, "static char *s_%s_%s_Tokens[] = {\n", recordName, m_name);
	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		fprintf(outfile, "    \"%s\",\n", walk.GetObj()->m_name);
		walk.Next();
	}

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		if(walk.GetObj()->m_akaName){
			fprintf(outfile, "    \"%s\",\n", walk.GetObj()->m_akaName);
		}
	}
	fprintf(outfile, "};\n");

	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		sprintf(nicename, "k_Token_%s_%s_%s", recordName, m_name, walk.GetObj()->m_name);
		fprintf(outfile, "#define %-40s ((k_Token_Custom_Base) + %d)\n", nicename, numTokens);
		numTokens++;
	}
	for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
	{
		Datum *dat = walk.GetObj();
		if(dat->m_akaName){
			sprintf(nicename, "k_Token_%s_%s_%s", recordName, m_name, dat->m_akaName);
			fprintf(outfile, "#define %-40s ((k_Token_Custom_Base) + %d)\n", nicename, numTokens);
			numTokens++;
		}
	}
	fprintf(outfile, "#define k_Token_%s_%s_Max ((k_Token_Custom_Base) + %d)\n", 
			recordName, m_name, numTokens);

	walk.SetList(&m_datumList);
	bool canParseSequentially = true;
	for 
	(
		walk.SetList(&m_datumList); 
		walk.IsValid() && canParseSequentially; 
		walk.Next() 
	)
	{
		switch(walk.GetObj()->m_type) {
			case DATUM_INT:
			case DATUM_FLOAT:
			case DATUM_STRING:
			case DATUM_FILE:
			case DATUM_RECORD:
				break;
			default:
				canParseSequentially = false;
				break;
		}
	}

	if (canParseSequentially) {
		fprintf(outfile, "sint32 %sRecord::%s::ParseSequential(DBLexer *lex)\n", recordName, m_name);
		fprintf(outfile, "{\n");
		
		for (walk.SetList(&m_datumList); walk.IsValid(); walk.Next())
		{
			Datum *dat = walk.GetObj();
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "    if(!lex->GetIntAssignment(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected integer\"));\n");
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				case DATUM_FLOAT:
					fprintf(outfile, "    if(!lex->GetFloatAssignment(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected number\"));\n");
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				case DATUM_STRING:
				case DATUM_FILE:
					fprintf(outfile, "    if(!lex->GetFileAssignment(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected string\"));\n");
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				case DATUM_RECORD:
					fprintf(outfile, "    if(!g_the%sDB->GetRecordFromLexer(lex, m_%s)) {\n", dat->m_subType, dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected record from %s DB\"));\n", dat->m_subType);
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				default:
					Assert(0);
					break;
			}
		}
		fprintf(outfile, "    return 1;\n");
		fprintf(outfile, "}\n\n");

		fprintf(outfile, "sint32 %sRecord::%s::ParseFullySequential(DBLexer *lex)\n", recordName, m_name);
		fprintf(outfile, "{\n");
		for (walk.SetList(&m_datumList); walk.IsValid(); )
		{
			Datum *dat = walk.GetObj();
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "    if(!lex->GetInt(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected integer\"));\n");
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				case DATUM_FLOAT:
					fprintf(outfile, "    if(!lex->GetFloat(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected number\"));\n");
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				case DATUM_STRING:
				case DATUM_FILE:
					fprintf(outfile, "    if(!lex->GetFile(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected string\"));\n");
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				case DATUM_RECORD:
					fprintf(outfile, "    if(!g_the%sDB->GetCurrentRecordFromLexer(lex, m_%s)) {\n", dat->m_subType, dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected record from %s DB\"));\n", dat->m_subType);
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				default:
					Assert(0);
					break;
			}
			walk.Next();
			if(walk.IsValid()){
				fprintf(outfile, "    lex->GetToken();\n");
			}
		}
		fprintf(outfile, "    return 1;\n");
		fprintf(outfile, "}\n\n");
	
	}

	fprintf(outfile, "sint32 %sRecord::%s::Parse(DBLexer *lex)\n", recordName, m_name);
	fprintf(outfile, "{\n");
	fprintf(outfile, "    bool done = false;\n");
	fprintf(outfile, "    sint32 tok;\n");
	fprintf(outfile, "    sint32 result = 0;\n");
	fprintf(outfile, "    tok = lex->PeekAhead();\n");
	fprintf(outfile, "    if(tok != k_Token_OpenBrace) {\n");
	fprintf(outfile, "        if(lex->GetCurrentToken() != k_Token_OpenBrace) {\n");
	if(canParseSequentially) {
		fprintf(outfile, "            if(ParseSequential(lex)) {\n");
		fprintf(outfile, "                return 1;\n");
		fprintf(outfile, "            }\n");
	}
	if(m_parseNum > 0){
		fprintf(outfile, "            if(ParseNum(lex)) {\n");
		fprintf(outfile, "                return 1;\n");
		fprintf(outfile, "            }\n");
	}
	fprintf(outfile, "            DBERROR((\"Expected open brace for %s\"));\n", m_name);
	fprintf(outfile, "            return 0;\n");
	fprintf(outfile, "        }\n");
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    else {\n");
	fprintf(outfile, "        tok = lex->GetToken();\n");
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    lex->SetTokens(s_%s_%s_Tokens, k_Token_%s_%s_Max);\n", recordName, m_name, recordName, m_name);
	fprintf(outfile, "    while(!done) {\n");
	fprintf(outfile, "        tok = lex->GetToken();\n");
	fprintf(outfile, "        switch(tok) {\n");

	
	ExportTokenCases(outfile, recordName);

	
	fprintf(outfile, "            case k_Token_CloseBrace:\n");
	fprintf(outfile, "                done = true;\n");
	fprintf(outfile, "                result = 1;\n");
	fprintf(outfile, "                break;\n");
	fprintf(outfile, "            default:\n");

	ExportDefaultToken(outfile, recordName);

	fprintf(outfile, "        }\n");
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    lex->RestoreTokens();\n");
	fprintf(outfile, "    return result;\n");
	fprintf(outfile, "}\n\n");

	
	fprintf(outfile, "sint32 %sRecord::%s::ParseInArray(DBLexer *lex, %s **array, sint32 *numElements)\n", 
			recordName, m_name, m_name);
	fprintf(outfile, "{\n");
	fprintf(outfile, "    if(*numElements > 0) {\n");
	fprintf(outfile, "        %s *oldArray = *array;\n", m_name);
	fprintf(outfile, "        *array = new %s[(*numElements) + 1];\n", m_name);
	fprintf(outfile, "        for (int i=0; i < (*numElements); i++)\n");
	fprintf(outfile, "             (*array)[i] = oldArray[i];\n");
	fprintf(outfile, "        delete [] oldArray;\n");
	fprintf(outfile, "    } else {\n");
	fprintf(outfile, "        *array = new %s[1];\n", m_name);
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    (*array)[*numElements].Parse(lex);\n");
	fprintf(outfile, "    *numElements += 1;\n");
	fprintf(outfile, "    return 1;\n");
	fprintf(outfile, "}\n\n");

	
	fprintf(outfile, "sint32 %sRecord::%s::ParseInArray(DBLexer *lex, %s *array, sint32 *numElements, sint32 maxSize)\n", 
			recordName, m_name, m_name);
	fprintf(outfile, "{\n");
	fprintf(outfile, "    if(*numElements >= maxSize) {\n");
	fprintf(outfile, "        return 0;\n");
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    array[*numElements].Parse(lex);\n");
	fprintf(outfile, "    *numElements += 1;\n");
	fprintf(outfile, "    return 1;\n");
	fprintf(outfile, "}\n\n");

	if(canParseSequentially) {
		fprintf(outfile, "sint32 %sRecord::%s::ParseInArraySequential(DBLexer *lex, %s **array, sint32 *numElements)\n", 
				recordName, m_name, m_name);
		fprintf(outfile, "{\n");
		fprintf(outfile, "    if(*numElements > 0) {\n");
		fprintf(outfile, "        %s *oldArray = *array;\n", m_name);
		fprintf(outfile, "        *array = new %s[(*numElements) + 1];\n", m_name);
		fprintf(outfile, "        for (int i=0; i < (*numElements); i++)\n");
		fprintf(outfile, "             (*array)[i] = oldArray[i];\n");
		fprintf(outfile, "        delete [] oldArray;\n");
		fprintf(outfile, "    } else {\n");
		fprintf(outfile, "        *array = new %s[1];\n", m_name);
		fprintf(outfile, "    }\n");
		fprintf(outfile, "    (*array)[*numElements].ParseFullySequential(lex);\n");
		fprintf(outfile, "    *numElements += 1;\n");
		fprintf(outfile, "    return 1;\n");
		fprintf(outfile, "}\n\n");

	
		fprintf(outfile, "sint32 %sRecord::%s::ParseInArraySequential(DBLexer *lex, %s *array, sint32 *numElements, sint32 maxSize)\n", 
				recordName, m_name, m_name);
		fprintf(outfile, "{\n");
		fprintf(outfile, "    if(*numElements >= maxSize) {\n");
		fprintf(outfile, "        return 0;\n");
		fprintf(outfile, "    }\n");
		fprintf(outfile, "    array[*numElements].ParseFullySequential(lex);\n");
		fprintf(outfile, "    *numElements += 1;\n");
		fprintf(outfile, "    return 1;\n");
		fprintf(outfile, "}\n\n");
	}

	if(m_parseNum > 0){
		fprintf(outfile, "sint32 %sRecord::%s::ParseNum(DBLexer *lex)\n", recordName, m_name);
		fprintf(outfile, "{\n");
		fprintf(outfile, "    bool done = false;\n");
		fprintf(outfile, "    sint32 tok;\n");

		fprintf(outfile, "    lex->SetTokens(s_%s_%s_Tokens, k_Token_%s_%s_Max);\n", recordName, m_name, recordName, m_name);
		fprintf(outfile, "    for(sint32 i = 0; i < %i; ++i) {\n", m_parseNum);
		fprintf(outfile, "        tok = lex->GetToken();\n");
		fprintf(outfile, "        switch(tok) {\n");

		ExportTokenCases(outfile, recordName);

		fprintf(outfile, "            default:\n");

		ExportDefaultToken(outfile, recordName);

		fprintf(outfile, "        }\n");
		fprintf(outfile, "    }\n");
		fprintf(outfile, "    lex->RestoreTokens();\n");
		fprintf(outfile, "\n");
		fprintf(outfile, "    if(done)\n");
		fprintf(outfile, "        return 0;\n");
		fprintf(outfile, "\n");
		fprintf(outfile, "    return 1;\n");
		fprintf(outfile, "}\n\n");
	}

	ExportResolver(outfile, recordName);
}

void MemberClass::ExportTokenCases(FILE *outfile, char *recordName)
{
	for 
	(
		PointerList<Datum>::Walker walk(&m_datumList);
		walk.IsValid();
		walk.Next()
	)
	{
		Datum *dat = walk.GetObj();
		fprintf(outfile,         "            case k_Token_%s_%s_%s:\n", recordName, m_name, dat->m_name);
		if(dat->m_akaName){
			fprintf(outfile,         "            case k_Token_%s_%s_%s:\n", recordName, m_name, dat->m_akaName);
		}
		if(dat->m_maxSize == k_MAX_SIZE_VARIABLE) {
			
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "                if(!CTPRecord::ParseIntInArray(lex, &m_%s, &m_num%s)) {\n", dat->m_name, dat->m_name);
					break;
				case DATUM_STRINGID:
					fprintf(outfile, "                if(!CTPRecord::ParseStringIdInArray(lex, m_%s, &m_num%s)) {\n", dat->m_name, dat->m_name);
					break;
				case DATUM_FLOAT:
					fprintf(outfile, "                if(!CTPRecord::ParseFloatInArray(lex, &m_%s, &m_num%s)) {\n", dat->m_name, dat->m_name);
					break;
				case DATUM_STRING:
				case DATUM_FILE:
					fprintf(outfile, "                if(!CTPRecord::ParseFileInArray(lex, &m_%s, &m_num%s)) {\n", dat->m_name, dat->m_name);
					break;
				case DATUM_RECORD:
					fprintf(outfile, "                if(!g_the%sDB->ParseRecordInArray(lex, &m_%s, &m_num%s)) {\n", dat->m_subType, dat->m_name, dat->m_name);
					break;
				case DATUM_STRUCT:
					fprintf(outfile, "                if(!%sRecord::%s::ParseInArray(lex, &m_%s, &m_num%s)) {\n", recordName, dat->m_subType, dat->m_name, dat->m_name);
					break;
				default:
					Assert(0);
					break;
			}
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
		} else if(dat->m_maxSize > 0) {
			
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "                if(!CTPRecord::GetIntInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
							dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_STRINGID:
					fprintf(outfile, "                if(!CTPRecord::GetStringIdInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n",
							dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_FLOAT:
					fprintf(outfile, "                if(!CTPRecord::ParseFloatInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
							dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_STRING:
				case DATUM_FILE:
					fprintf(outfile, "                if(!CTPRecord::ParseFileInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
							dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_RECORD:
					fprintf(outfile, "                if (!g_the%sDB->ParseRecordInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
							dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_STRUCT:
					fprintf(outfile, "                if(!%sRecord::%s::ParseInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
							recordName, dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
					break;
				default:
					Assert(0);
					break;
			}
		} else if(dat->m_maxSize < 0) {
			switch(dat->m_type) {
				case DATUM_INT:
					fprintf(outfile, "                if(!lex->GetIntAssignment(m_%s)) {\n", dat->m_name);
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				case DATUM_STRINGID:
					fprintf(outfile, "                if(!lex->GetStringIdAssignment(m_%s)) {\n", dat->m_name);
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
					fprintf(outfile, "                    done = true; break;\n");
					fprintf(outfile, "                }\n");
					break;
				case DATUM_STRING:
				case DATUM_FILE:
					fprintf(outfile, "                if(!lex->GetFileAssignment(m_%s)) {\n", dat->m_name);
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
		fprintf(outfile,         "                break;\n");
	}
}

void MemberClass::ExportDefaultToken(FILE *outfile, char *recordName)
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
				fprintf(outfile, "                if(!%sRecord::%s::ParseInArraySequential(lex, &m_%s, &m_num%s)) {\n", recordName, dat->m_subType, dat->m_name, dat->m_name);
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
				fprintf(outfile, "                if(!CTPRecord::GetIntInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
						dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_STRINGID:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::GetStringIdInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n",
						dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_FLOAT:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseFloatInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
						dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_STRING:
			case DATUM_FILE:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!CTPRecord::ParseFileInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
						dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_RECORD:
				fprintf(outfile, "                Assert(false)\n");
				fprintf(outfile, "                if(!g_the%sDB->ParseRecordInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
						dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "                if(!%sRecord::%s::ParseInArraySequential(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
						recordName, dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
				break;
			default:
				Assert(0);
				break;
		}
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
				fprintf(outfile, "                if(!m_%s.ParseFullySequential(lex)) {\n", dat->m_name);
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

void MemberClass::ExportOtherRecordIncludes(FILE *outfile)
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

		if(dat->m_type == DATUM_RECORD)
		{
			fprintf(outfile, "#include \"%sRecord.h\"\n", dat->m_subType);
		}
		else if(dat->m_type == DATUM_BIT_PAIR)
		{
			if (walk.GetObj()->m_bitPairDatum->m_type == DATUM_RECORD)
			{
				fprintf(outfile, "#include \"%sRecord.h\"\n", dat->m_bitPairDatum->m_subType);
			}
			else if(dat->m_bitPairDatum->m_hasDBRefValue
			     && strcmp(dat->m_bitPairDatum->drefval.DBName, m_name))
			{
				fprintf(outfile, "#include \"%sRecord.h\"\n", dat->m_bitPairDatum->drefval.DBName);
			}
		}
		else if (dat->m_hasDBRefValue)
		{
			fprintf(outfile, "#include \"%sRecord.h\"\n", dat->drefval.DBName);
		}
	}
}

void MemberClass::ExportDataCode(FILE *outfile, char *recordName)
{
	for (PointerList<Datum>::Walker walk(&m_datumList); walk.IsValid(); walk.Next())
	{
		Datum *dat = walk.GetObj();
		if(dat->m_maxSize <= 0) {
			switch(dat->m_type) {
			case DATUM_RECORD:
				fprintf(outfile, "const %sRecord *%sRecord::%s::Get%s() const\n", dat->m_subType, recordName, m_name, dat->m_name);
				fprintf(outfile, "{\n");
				fprintf(outfile, "    return g_the%sDB->Get(m_%s);\n", dat->m_subType, dat->m_name);
				fprintf(outfile, "}\n\n");
				break;
			case DATUM_BIT_PAIR:
				if(dat->m_bitPairDatum->m_type == DATUM_RECORD) {
					fprintf(outfile, "const %sRecord *%sRecord::%s::Get%sPtr() const\n", dat->m_bitPairDatum->m_subType, recordName, m_name, dat->m_name);
					fprintf(outfile, "{\n");
					fprintf(outfile, "    return g_the%sDB->Get(m_%s);\n", dat->m_bitPairDatum->m_subType, dat->m_bitPairDatum->m_name);
					fprintf(outfile, "}\n\n");
				}
				break;

			default:
				break;
			}
		} else {
			switch(dat->m_type) {
			case DATUM_RECORD:
				fprintf(outfile, "sint32 %sRecord::%s::Get%sIndex(sint32 index) const\n", recordName, m_name, dat->m_name);
				fprintf(outfile, "{\n");
				dat->ExportRangeCheck(outfile);
				fprintf(outfile, "    return m_%s[index];\n", dat->m_name);
				fprintf(outfile, "}\n\n");
				
				fprintf(outfile, "const %sRecord * %sRecord::%s::Get%s(sint32 index) const\n", dat->m_subType, recordName, m_name, dat->m_name);
				fprintf(outfile, "{\n");
				dat->ExportRangeCheck(outfile);
				fprintf(outfile, "    return g_the%sDB->Get(m_%s[index]);\n", dat->m_subType, dat->m_name);
				fprintf(outfile, "}\n\n");
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "const %sRecord::%s * %sRecord::%s::Get%s(sint32 index) const\n", recordName, dat->m_subType, recordName, m_name, dat->m_name);
				fprintf(outfile, "{\n");
				dat->ExportRangeCheck(outfile);
				fprintf(outfile, "    return &m_%s[index];\n", dat->m_name);
				fprintf(outfile, "}\n\n");
				break;
			default:
				Assert(false);
				break;
			}
		}
	}
}

void MemberClass::ExportResolver(FILE *outfile, const char *recordName)
{
	fprintf(outfile, "void %sRecord::%s::ResolveDBReferences()\n", recordName, m_name);
	fprintf(outfile, "{\n");
	for 
	(
		PointerList<Datum>::Walker walk(&m_datumList);
		walk.IsValid();
		walk.Next()
	) 
	{
		Datum *dat = walk.GetObj();
		if(dat->m_type == DATUM_RECORD ||
		   dat->m_type == DATUM_STRUCT) {
			dat->ExportResolver(outfile);
		}
	}
	fprintf(outfile, "}\n\n");
}
