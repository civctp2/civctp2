#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ctpdb.h"
#include "RecordDescription.h"

MemberClass::MemberClass(char *name)
{
	strcpy(m_name, name);
	m_numBits = 0;
}

MemberClass::~MemberClass()
{
}

void MemberClass::AddDatum(DATUM_TYPE type, struct namelist *nameInfo,
						   sint32 minSize, sint32 maxSize,
						   char *subType)
{
	
	Datum *dat = new Datum;
	dat->m_type = type;
	dat->m_name = nameInfo->name;
	dat->m_minSize = minSize;
	dat->m_maxSize = maxSize;
	dat->m_subType = subType;
	m_datumList.AddTail(dat);

	if(dat->m_type == DATUM_BIT) {
		dat->m_bitNum = m_numBits;
		m_numBits++;
	} else {
		dat->m_bitNum = -1;
	}
}

void MemberClass::AddGroupedBits(char *name, struct namelist *list)
{
	Assert("Group Bits in Member classes not yet supported" == 0);
}

void MemberClass::AddBitPair(char *name, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype)
{
	Datum *dat = new Datum;
	dat->m_type = DATUM_BIT_PAIR;
	dat->m_name = name;
	dat->m_minSize = minSize;
	dat->m_maxSize = maxSize;
	dat->m_subType = NULL;
	dat->m_groupList = NULL;
	dat->m_bitNum = m_numBits;
	m_numBits++;

	Datum *pairDat = new Datum;
	dat->m_bitPairDatum = pairDat;
	pairDat->m_type = (DATUM_TYPE)pairtype->type;
	pairDat->m_name = (char *)malloc(strlen(dat->m_name) + strlen("Value") + 1);
	strcpy(pairDat->m_name, dat->m_name);
	strcat(pairDat->m_name, "Value");
	pairDat->m_minSize = -1;
	pairDat->m_maxSize = -1;
	pairDat->m_subType = (char *)pairtype->extraData;
	pairDat->m_groupList = NULL;
	pairDat->m_bitPairDatum = NULL;

	m_datumList.AddTail(dat);
}

void MemberClass::ExportHeader(FILE *outfile)
{
	
	fprintf(outfile, "    class %s {\n    private:\n", m_name);
	
	ExportData(outfile);

	fprintf(outfile, "    public:\n");

	ExportMethods(outfile);

	fprintf(outfile, "    }; /* class %s */\n\n", m_name);
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
				fprintf(outfile, "//\n// m_flags%d\n", bit / 32);
			}
			sprintf(nicename, "k_%s_%s_Bit", m_name, dat->m_name);
			fprintf(outfile, "#define %-40s 0x%08lx\n", nicename, 1 << (bit % 32));
			bit++;
		}
		walk.Next();
	}

	
	walk.SetList(&m_datumList);
	while(walk.IsValid()) {
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
		walk.Next();
	}
}

void MemberClass::ExportData(FILE *outfile)
{
	
	if(m_numBits > 0) {
		sint32 flag;
		for(flag = 0; flag <= m_numBits / 32; flag++) {
			fprintf(outfile, "        uint32 m_flags%d;\n", flag);
		}
		fprintf(outfile, "\n");
	}

	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_type != DATUM_BIT) {
			walk.GetObj()->ExportVariable(outfile, 1);
		}
		walk.Next();
	}
}

void MemberClass::ExportMethods(FILE *outfile)
{
	fprintf(outfile, "        %s();\n", m_name);
	fprintf(outfile, "        ~%s();\n", m_name);
	fprintf(outfile, "        void operator=(const %s & rval);\n", m_name);
	fprintf(outfile, "        bool operator==(const %s & rval) { \n", m_name);
	
	PointerList<Datum>::Walker walk(&m_datumList);
	bool record_element_found = false;
	while(walk.IsValid()) {
		if(walk.GetObj()->m_type == DATUM_RECORD) {
			fprintf(outfile, "          if (m_%s != rval.m_%s) \n", 
					walk.GetObj()->m_name, walk.GetObj()->m_name);
			fprintf(outfile, "             return false; \n"); 
			record_element_found = true;
		}
		walk.Next();
	}

	
	if (record_element_found == false) {
		fprintf(outfile, "          return false; \n"); 
	}
	else {
		fprintf(outfile, "          return true; \n");
	}
	fprintf(outfile, "        } \n\n");

	
	fprintf(outfile, "        sint32 Parse(DBLexer *lex);\n");
	fprintf(outfile, "        sint32 ParseSequential(DBLexer *lex);\n");
	fprintf(outfile, "        static sint32 ParseInArray(DBLexer *lex, %s **array, sint32 *numElements);\n", m_name);
	fprintf(outfile, "        static sint32 ParseInArray(DBLexer *lex, %s *array, sint32 *numElements, sint32 maxSize);\n", m_name);
	fprintf(outfile, "        void ResolveDBReferences();\n");

	
	walk.SetList(&m_datumList);
	while(walk.IsValid()) {
		walk.GetObj()->ExportAccessor(outfile, 1, m_name);
		walk.Next();
	}
}

void MemberClass::ExportForwardDeclarations(FILE *outfile)
{
	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_type == DATUM_RECORD) {
			fprintf(outfile, "class %sRecord;\n", walk.GetObj()->m_subType);
		}
		if(walk.GetObj()->m_type == DATUM_BIT_PAIR &&
			(DATUM_TYPE)walk.GetObj()->m_bitPairDatum->m_type == DATUM_RECORD) {
			fprintf(outfile, "class %sRecord;\n", walk.GetObj()->m_bitPairDatum->m_subType);
		}
		walk.Next();
	}
}

void MemberClass::ExportInitialization(FILE *outfile, char *recordName)
{

	fprintf(outfile, "%sRecord::%s::%s()\n", recordName, m_name, m_name);
	fprintf(outfile, "{\n");

	
	sint32 i;
	for(i = 0; i  < ((m_numBits + 31)/ 32); i++) {
		fprintf(outfile, "    m_flags%d = 0;\n", i);
	}

	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		walk.GetObj()->ExportInitialization(outfile);
		walk.Next();
	}
	fprintf(outfile, "}\n");

	
	fprintf(outfile, "%sRecord::%s::~%s()\n", recordName, m_name, m_name);
	fprintf(outfile, "{\n");
	walk.SetList(&m_datumList);
	while(walk.IsValid()) {
		walk.GetObj()->ExportDestructor(outfile);
		walk.Next();
	}
	fprintf(outfile, "}\n");

	
	fprintf(outfile, "void %sRecord::%s::operator=(const %s & rval)\n", recordName, m_name, m_name);
	fprintf(outfile, "{\n");
	walk.SetList(&m_datumList);
	while(walk.IsValid()) {
		walk.GetObj()->ExportOperatorEqual(outfile);
		walk.Next();
	}
	fprintf(outfile, "}\n");


}

void MemberClass::ExportParser(FILE *outfile, char *recordName)
{
	char nicename[k_MAX_STRING];
	sint32 numTokens = 0;

	PointerList<Datum>::Walker walk(&m_datumList);
	fprintf(outfile, "static char *s_%s_%s_Tokens[] = {\n", recordName, m_name);
	while(walk.IsValid()) {
		fprintf(outfile, "    \"%s\",\n", walk.GetObj()->m_name);
		walk.Next();
	}
	fprintf(outfile, "};\n");

	walk.SetList(&m_datumList);
	while(walk.IsValid()) {
		sprintf(nicename, "k_Token_%s_%s_%s", recordName, m_name, walk.GetObj()->m_name);
		fprintf(outfile, "#define %-40s ((k_Token_Custom_Base) + %d)\n", nicename, numTokens);
		numTokens++;
		walk.Next();
	}
	fprintf(outfile, "#define k_Token_%s_%s_Max ((k_Token_Custom_Base) + %d)\n", 
			recordName, m_name, numTokens);

	walk.SetList(&m_datumList);
	bool canParseSequentially = true;
	while(walk.IsValid() && canParseSequentially) {
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
		walk.Next();
	}

	if(canParseSequentially) {
		fprintf(outfile, "sint32 %sRecord::%s::ParseSequential(DBLexer *lex)\n", recordName, m_name);
		fprintf(outfile, "{\n");
		fprintf(outfile, "    DBPARSE_ERROR err = DBPARSE_OK;\n");
		walk.SetList(&m_datumList);
		while(walk.IsValid()) {
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
					fprintf(outfile, "    if(!g_the%sDB->GetRecordFromLexer(lex, m_%s, err)) {\n", dat->m_subType, dat->m_name);
					fprintf(outfile, "        DBERROR((\"Expected record from %s DB\"));\n", dat->m_subType);
					fprintf(outfile, "        return 0;\n");
					fprintf(outfile, "    }\n");
					break;
				default:
					Assert(0);
					break;
			}
			walk.Next();
		}
		fprintf(outfile, "    return 1;\n");
		fprintf(outfile, "}\n");
	}

	fprintf(outfile, "sint32 %sRecord::%s::Parse(DBLexer *lex)\n", recordName, m_name);
	fprintf(outfile, "{\n");
	fprintf(outfile, "    bool done = false;\n");
	fprintf(outfile, "    sint32 tok;\n");
	fprintf(outfile, "    sint32 result = 0;\n");
	fprintf(outfile, "    DBPARSE_ERROR err = DBPARSE_OK;\n");
	fprintf(outfile, "    tok = lex->PeekAhead();\n");
	fprintf(outfile, "    if(tok != k_Token_OpenBrace) {\n");
	if(canParseSequentially) {
		fprintf(outfile, "        if(ParseSequential(lex)) {\n");
		fprintf(outfile, "            return 1;\n");
		fprintf(outfile, "        }\n");
	}
	fprintf(outfile, "        DBERROR((\"Expected open brace for %s\"));\n", m_name);
	fprintf(outfile, "        return 0;\n");
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    tok = lex->GetToken();\n");
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
	fprintf(outfile, "                Assert(FALSE);\n");
	fprintf(outfile, "                done = true;\n");
	fprintf(outfile, "                break;\n");
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
	fprintf(outfile, "        for (int i=0; i < (*numElements); i++) \n");
	fprintf(outfile, "             (*array)[i] = oldArray[i];\n");
	fprintf(outfile, "        delete [] oldArray;\n");
	fprintf(outfile, "    } else {\n");
	fprintf(outfile, "        *array = new %s[1];\n", m_name);
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    (*array)[*numElements].Parse(lex);\n");
	fprintf(outfile, "    *numElements += 1;\n");
	fprintf(outfile, "    return 1;\n");
	fprintf(outfile, "}\n");

	
	fprintf(outfile, "sint32 %sRecord::%s::ParseInArray(DBLexer *lex, %s *array, sint32 *numElements, sint32 maxSize)\n", 
			recordName, m_name, m_name);
	fprintf(outfile, "{\n");
	fprintf(outfile, "    if(*numElements >= maxSize) {\n");
	fprintf(outfile, "        return 0;\n");
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    array[*numElements].Parse(lex);\n");
	fprintf(outfile, "    *numElements += 1;\n");
	fprintf(outfile, "    return 1;\n");
	fprintf(outfile, "}\n");

	ExportResolver(outfile, recordName);
}

void MemberClass::ExportTokenCases(FILE *outfile, char *recordName)
{
	
	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		Datum *dat = walk.GetObj();
		fprintf(outfile,         "            case k_Token_%s_%s_%s:\n", recordName, m_name, dat->m_name);
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
					fprintf(outfile, "                if(!g_the%sDB->ParseRecordInArray(lex, &m_%s, &m_num%s, err)) {\n", dat->m_subType, dat->m_name, dat->m_name);
					break;
				case DATUM_STRUCT:
					fprintf(outfile, "                if(!%sRecord::%s::ParseInArray(lex, &m_%s, &m_num%s)) {\n", m_name, dat->m_subType, dat->m_name, dat->m_name);
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
					fprintf(outfile, "                if(!g_the%sDB->ParseRecordInArray(lex, &m_%s, &m_num%s, k_MAX_%s, err)) {\n", 
							dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
					break;
				case DATUM_STRUCT:
					fprintf(outfile, "                if(!%sRecord::%s::ParseInArray(lex, &m_%s, &m_num%s, k_MAX_%s)) {\n", 
							m_name, dat->m_subType, dat->m_name, dat->m_name, dat->m_name);
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
					fprintf(outfile, "                \n");
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
					fprintf(outfile, "                if(!g_the%sDB->GetRecordFromLexer(lex, m_%s, err)) {\n", dat->m_subType, dat->m_name);
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
		walk.Next();
	}
}


void MemberClass::ExportOtherRecordIncludes(FILE *outfile)
{
	
	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_type == DATUM_RECORD) {
			
			
			
			fprintf(outfile, "#include \"%sRecord.h\"\n", walk.GetObj()->m_subType);
		}
		if(walk.GetObj()->m_type == DATUM_BIT_PAIR) {
			
			if (walk.GetObj()->m_bitPairDatum->m_type == DATUM_RECORD)
				fprintf(outfile, "#include \"%sRecord.h\"\n", walk.GetObj()->m_bitPairDatum->m_subType);
		}
		walk.Next();
	}
}

void MemberClass::ExportDataCode(FILE *outfile, char *recordName)
{
	
	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
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
				
				fprintf(outfile, "const %sRecord *%sRecord::%s::Get%s(sint32 index) const\n", dat->m_subType, recordName, m_name, dat->m_name);
				fprintf(outfile, "{\n");
				dat->ExportRangeCheck(outfile);
				fprintf(outfile, "    return g_the%sDB->Get(m_%s[index]);\n", dat->m_subType, dat->m_name);
				fprintf(outfile, "}\n\n");
				break;
			default:
				break;
			}
		}
		walk.Next();
	}
}

void MemberClass::ExportResolver(FILE *outfile, const char *recordName)
{
	fprintf(outfile, "void %sRecord::%s::ResolveDBReferences()\n", recordName, m_name);
	fprintf(outfile, "{\n");
	PointerList<Datum>::Walker walk(&m_datumList);
	while(walk.IsValid()) {
		Datum *dat = walk.GetObj();
		if(dat->m_type == DATUM_RECORD ||
		   dat->m_type == DATUM_STRUCT) {
			dat->ExportResolver(outfile);
		}
		walk.Next();
	}
	fprintf(outfile, "}\n");
}
