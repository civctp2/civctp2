//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database record file generator
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
// Win32
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Modified ExportBitPairInitialization function to allow bit pairs to 
//   have default values so that when two records are merged, only the bit 
//   is merged in that is set. - Sep. 28th 2004 Martin Gühmann
// - Updated ExportBitPairAccessorProto function so that the default
//   values of bits can be accesses if these bits have default values.
// - Added serilization method export. (Aug 24th 2005 Martin Gühmann)
// - Output files only have spaces instead of tabs as indent and indetion
//   was fixed. (Aug 25th 2005 Martin Gühmann)
// - Added alias names and the possibility to have default values from 
//   other entries. (Aug 26th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#include <stdio.h>
#include <string.h>
#if defined(WIN32)
#include <windows.h>
#else
#include "windows.h"
#endif

#include "ctpdb.h"
#include "RecordDescription.h"

void Datum::SetValue(union dbvalue &v)
{
	m_hasValue = true;
	val = v;
}

void Datum::ExportBitGroupParser(FILE *outfile, char *recordName)
{
	fprintf(outfile, "static char *s_%s_%s_BitNames[] = {\n", recordName, m_name);
	sint32 numBits = 0;
	struct namelist *node = m_groupList;
	while(node) {
		numBits++;
		fprintf(outfile, "    \"%s\",\n", node->name);
		node = node->next;
	}
	fprintf(outfile, "};\n");
	fprintf(outfile, "#define k_%s_%s_Num_BitNames %d\n\n", recordName, m_name, numBits);
	fprintf(outfile, "sint32 %sRecord::Parse%sBit(DBLexer *lex)\n", recordName, m_name);
	fprintf(outfile, "{\n");
	fprintf(outfile, "    sint32 bitindex;\n");
	fprintf(outfile, "    if(!lex->GetBitIndex((const char **)s_%s_%s_BitNames, k_%s_%s_Num_BitNames, bitindex)) {\n", recordName, m_name, recordName, m_name);
	fprintf(outfile, "        DBERROR((\"%%s is not a member of %s\", lex->GetTokenText()));\n", m_name);
	fprintf(outfile, "        return 0;\n");
	fprintf(outfile, "    }\n");
	fprintf(outfile, "    m_%s |= (1 << bitindex);\n", m_name);
	fprintf(outfile, "    return 1;\n");
	fprintf(outfile, "}\n\n");
}

void Datum::ExportVariable(FILE *outfile, sint32 indent)
{
	if(m_type == DATUM_BIT_PAIR) {
		m_bitPairDatum->ExportVariable(outfile, indent);
		return;
	}

	
	char sizestring[50];
	const char *typestring = "";
	char comment[k_MAX_STRING];
	char star = ' ';
	char notFixedStar = ' ';
	comment[0] = 0;

	if(m_maxSize == k_MAX_SIZE_VARIABLE) {
		sizestring[0] = 0;
		notFixedStar = '*';
	} else if(m_maxSize > 0) {
		sprintf(sizestring, "[k_MAX_%s]", m_name);
	} else {
		sizestring[0] = 0;
	}

	switch(m_type) {
		case DATUM_INT:
		case DATUM_STRINGID:
			typestring = "sint32";
			break;
		case DATUM_BIT:
			typestring = "uint8";
			Assert(0);
			break;			
		case DATUM_FLOAT:
			typestring = "double";
			break;
		case DATUM_STRING:
		case DATUM_FILE:
			typestring = "char";
			star = '*';
			break;
		case DATUM_RECORD:
			typestring = "sint32";
			sprintf(comment, " // Index into %s database", m_subType);
			break;
		case DATUM_STRUCT:
			typestring = m_subType;
			break;
		case DATUM_BIT_GROUP:
			typestring = "uint32";
			break;
	}
	fprintf(outfile, "%s", indent ? "        " : "    ");
	fprintf(outfile, "%-15s %c%cm_%s%s;%s\n", typestring, star, notFixedStar, m_name, sizestring, comment);

	
	if((m_maxSize > 0) || (m_maxSize == k_MAX_SIZE_VARIABLE)) {
		fprintf(outfile, "%s", indent ? "        ": "    ");
		fprintf(outfile, "sint32            m_num%s;\n", m_name);
	}
}

void Datum::ExportRangeDefines(FILE *outfile)
{
	if(m_minSize > 0) {
		fprintf(outfile, "#define k_MIN_%-30s %d\n", m_name, m_minSize);
	}

	if(m_maxSize > 0 && (m_maxSize != (k_MAX_SIZE_VARIABLE))) {
		fprintf(outfile, "#define k_MAX_%-30s %d\n", m_name, m_maxSize);
	}
}

void Datum::ExportBitPairAccessorProto(FILE *outfile, sint32 indent, char *recordName)
{
	char *ind = "";
	if(indent) {
		ind = "    ";
	}

	fprintf(outfile,             "%s    bool             Get%s() const { return (m_flags%d & k_%s_%s_Bit) != 0; }\n", ind,
			m_name, m_bitNum / 32, recordName, m_name);
	switch(m_bitPairDatum->m_type) {
		case DATUM_INT:
		case DATUM_STRINGID:

//Added by Martin Gühmann
			if(!m_hasValue){
				fprintf(outfile, "%s    bool             Get%s(sint32 &value) const {\n", ind, m_name);
				fprintf(outfile, "%s                         if((m_flags%d & k_%s_%s_Bit) == 0) return false;\n", ind,
					m_bitNum / 32, recordName, m_name);
				fprintf(outfile, "%s                         value = m_%s;\n", ind, m_bitPairDatum->m_name);
				fprintf(outfile, "%s                         return true;\n", ind);
				fprintf(outfile, "%s                     }\n", ind);
			}
			else{
				fprintf(outfile, "%s    bool             Get%s(sint32 &value) const {\n", ind, m_name);
				fprintf(outfile, "%s                         value = m_%s;\n", ind, m_bitPairDatum->m_name);
				fprintf(outfile, "%s                         return ((m_flags%d & k_%s_%s_Bit) != 0);\n", ind,
					m_bitNum / 32, recordName, m_name);
				fprintf(outfile, "%s                     }\n", ind);
			}
			break;
		case DATUM_FLOAT:

//Added by Martin Gühmann
			if(!m_hasValue){
				fprintf(outfile, "%s    bool             Get%s(double &value) const {\n", ind, m_name);
				fprintf(outfile, "%s                         if((m_flags%d & k_%s_%s_Bit) == 0) return false;\n", ind,
					m_bitNum / 32, recordName, m_name);
				fprintf(outfile, "%s                         value = m_%s;\n", ind, m_bitPairDatum->m_name);
				fprintf(outfile, "%s                         return true;\n", ind);
				fprintf(outfile, "%s                     }\n", ind);
			}
			else{
				fprintf(outfile, "%s    bool             Get%s(double &value) const {\n", ind, m_name);
				fprintf(outfile, "%s                         value = m_%s;\n", ind, m_bitPairDatum->m_name);
				fprintf(outfile, "%s                         return ((m_flags%d & k_%s_%s_Bit) != 0);\n", ind,
					m_bitNum / 32, recordName, m_name);
				fprintf(outfile, "%s                     }\n", ind);
			}

			break;
		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile,     "%s    bool             Get%s(const char *&text) const {\n", ind, m_name);
			fprintf(outfile,     "%s                         if((m_flags%d & k_%s_%s_Bit) == 0) return false;\n", ind,
					m_bitNum / 32, recordName, m_name);
			fprintf(outfile,     "%s                         text = m_%s;\n", ind, m_bitPairDatum->m_name);
			fprintf(outfile,     "%s                         return true;\n", ind);
			fprintf(outfile,     "%s                     }\n", ind);
			break;
		case DATUM_RECORD:
			fprintf(outfile,     "%s    bool             Get%sIndex(sint32 &index) const {\n", ind, m_name);
			fprintf(outfile,     "%s                         if((m_flags%d & k_%s_%s_Bit) == 0) return false;\n", ind,
					m_bitNum / 32, recordName, m_name);
			fprintf(outfile,     "%s                         index = m_%s;\n", ind, m_bitPairDatum->m_name);
			fprintf(outfile,     "%s                         return true;\n", ind);
			fprintf(outfile,     "%s                     }\n", ind);
			
			fprintf(outfile,     "%s    bool             Get%s(const %sRecord *&rec) const;\n", ind, m_name, m_bitPairDatum->m_subType);
			fprintf(outfile,     "%s    const %sRecord  *Get%sPtr() const;\n", ind, m_bitPairDatum->m_subType, m_name);
			break;
		case DATUM_STRUCT:
			fprintf(outfile,     "%s    bool             Get%s(const %s *&ptr) const {\n", ind, m_name, m_bitPairDatum->m_subType);
			fprintf(outfile,     "%s                         if((m_flags%d & k_%s_%s_Bit) == 0) return false;\n", ind,
					m_bitNum / 32, recordName, m_name);
			fprintf(outfile,     "%s                         ptr = &m_%s;\n", ind, m_bitPairDatum->m_name);
			fprintf(outfile,     "%s                         return true;\n", ind);
			fprintf(outfile,     "%s                     }\n", ind);
			fprintf(outfile,     "%s    const %s        *Get%sPtr() const {\n", ind, m_bitPairDatum->m_subType, m_name);
			fprintf(outfile,     "%s                         if((m_flags%d & k_%s_%s_Bit) == 0) return NULL;\n", ind,
					m_bitNum / 32, recordName, m_name);
			fprintf(outfile,     "%s                         return &m_%s;\n", ind, m_bitPairDatum->m_name);
			fprintf(outfile,     "%s                     }\n", ind);
			break;
	}
}

void Datum::ExportAccessor(FILE *outfile, sint32 indent, char *recordName)
{
	char *ind = "";
	if(indent) {
		ind = "    ";
	}

	if(m_maxSize <= 0) {
		switch(m_type) {
			case DATUM_INT:
			case DATUM_STRINGID:
				fprintf(outfile, "%s    sint32           Get%s() const { return m_%s; }\n", ind, m_name, m_name);
				break;
			case DATUM_BIT:
				fprintf(outfile, "%s    bool             Get%s() const { return (m_flags%d & k_%s_%s_Bit) != 0; }\n",
						ind, m_name, m_bitNum / 32, recordName, m_name);
				break;
			case DATUM_BIT_PAIR:
				ExportBitPairAccessorProto(outfile, indent, recordName);
				break;
				
			case DATUM_FLOAT:
				fprintf(outfile, "%s    double           Get%s() const { return m_%s; }\n", ind, m_name, m_name);
				break;
			case DATUM_STRING:
			case DATUM_FILE:
				fprintf(outfile, "%s    const char      *Get%s() const { return m_%s; }\n", ind, m_name, m_name);
				break;
			case DATUM_RECORD:
				fprintf(outfile, "%s    sint32           Get%sIndex() const { return m_%s; }\n", ind, m_name, m_name);
				fprintf(outfile, "%s    const %sRecord *Get%s() const;\n", ind, m_subType, m_name);
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "%s    const %s *Get%s() const { return &m_%s; }\n", ind, m_subType, m_name, m_name);
				break;
			case DATUM_BIT_GROUP:
			{
				fprintf(outfile, "\n%s    //\n%s    // %s flag group\n", ind, ind, m_name);
				fprintf(outfile, "%s    uint32           Get%s() const { return m_%s; }\n", ind, m_name, m_name);
				struct namelist *node = m_groupList;
				while(node) {
					fprintf(outfile, "%s    bool             Get%s%s() const { return (m_%s & k_%s_%s_%s_Bit) != 0; }\n",
							ind, m_name, node->name, m_name, recordName, m_name, node->name);
					node = node->next;
				}
				fprintf(outfile, "%s    // End %s flag group\n    //\n", ind, m_name);
				break;
			}
			default:
				Assert(0);
				break;
		}
	} else {
		switch(m_type) {
			case DATUM_INT:
			case DATUM_STRINGID:
				fprintf(outfile, "    sint32           Get%s(sint32 index) const;\n", m_name);
				break;
			case DATUM_FLOAT:
				fprintf(outfile, "    double           Get%s(sint32 index) const;\n", m_name);
				break;
			case DATUM_STRING:
			case DATUM_FILE:
				fprintf(outfile, "    const char      *Get%s(sint32 index) const;\n", m_name);
				break;
			case DATUM_RECORD:
				fprintf(outfile, "    sint32           Get%sIndex(sint32 index) const;\n", m_name);
				fprintf(outfile, "    const %sRecord  *Get%s(sint32 index) const;\n", m_subType, m_name);
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "    const %s        *Get%s(sint32 index) const;\n", m_subType, m_name);
				break;
		}
		fprintf(outfile,         "    sint32           GetNum%s() const { return m_num%s;}\n", m_name, m_name);
	}
}

void Datum::ExportRangeCheck(FILE *outfile)
{
	fprintf(outfile, "    Assert(index >= 0);\n");
	fprintf(outfile, "    Assert(index < m_num%s);\n", m_name);
	fprintf(outfile, "    if((index < 0) || (index >= m_num%s)) {\n", m_name);
	fprintf(outfile, "        return 0;\n");
	fprintf(outfile, "    }\n");
}

void Datum::ExportDataCode(FILE *outfile, char *recordName)
{
	
	
	if(m_maxSize <= 0) {
		switch(m_type) {
			case DATUM_RECORD:
				fprintf(outfile, "const %sRecord *%sRecord::Get%s() const\n", m_subType, recordName, m_name);
				fprintf(outfile, "{\n");
				fprintf(outfile, "    return g_the%sDB->Get(m_%s);\n", m_subType, m_name);
				fprintf(outfile, "}\n\n");
				break;
			case DATUM_BIT_PAIR:
				if(m_bitPairDatum->m_type == DATUM_RECORD) {
					fprintf(outfile, "const %sRecord *%sRecord::Get%sPtr() const\n", m_bitPairDatum->m_subType, recordName, m_name);
					fprintf(outfile, "{\n");
					fprintf(outfile, "    return g_the%sDB->Get(m_%s);\n", m_bitPairDatum->m_subType, m_bitPairDatum->m_name);
					fprintf(outfile, "}\n\n");
				}
				break;
			default:
				break;
		}
	} else {
		switch(m_type) {
			case DATUM_INT:
			case DATUM_STRINGID:
				fprintf(outfile, "sint32 %sRecord::Get%s(sint32 index) const\n", recordName, m_name);
				fprintf(outfile, "{\n");
				ExportRangeCheck(outfile);
				fprintf(outfile, "    return m_%s[index];\n", m_name);
				fprintf(outfile, "}\n\n");
				break;
			case DATUM_FLOAT:
				fprintf(outfile, "double %sRecord::Get%s(sint32 index) const\n", recordName, m_name);
				fprintf(outfile, "{\n");
				ExportRangeCheck(outfile);
				fprintf(outfile, "    return m_%s[index];\n", m_name);
				fprintf(outfile, "}\n\n");
				break;

			case DATUM_STRING:
			case DATUM_FILE:
				fprintf(outfile, "const char      *%sRecord::Get%s(sint32 index) const\n\n", recordName, m_name);
				fprintf(outfile, "{\n");
				ExportRangeCheck(outfile);
				fprintf(outfile, "    return m_%s[index];\n", m_name);
				fprintf(outfile, "}\n\n");
				break;

			case DATUM_RECORD:
				fprintf(outfile, "sint32 %sRecord::Get%sIndex(sint32 index) const\n", recordName, m_name);
				fprintf(outfile, "{\n");
				ExportRangeCheck(outfile);
				fprintf(outfile, "    return m_%s[index];\n", m_name);
				fprintf(outfile, "}\n\n");
				
				fprintf(outfile, "const %sRecord *%sRecord::Get%s(sint32 index) const\n", m_subType, recordName, m_name);
				fprintf(outfile, "{\n");
				ExportRangeCheck(outfile);
				fprintf(outfile, "    return g_the%sDB->Get(m_%s[index]);\n", m_subType, m_name);
				fprintf(outfile, "}\n\n");
				break;

			case DATUM_STRUCT:
				fprintf(outfile, "const %sRecord::%s *%sRecord::Get%s(sint32 index) const\n", recordName, m_subType, recordName, m_name);
				fprintf(outfile, "{\n");
				ExportRangeCheck(outfile);
				fprintf(outfile, "    return &m_%s[index];\n", m_name);
				fprintf(outfile, "}\n\n");
				break;
			default:
				break;
		}
	}
}

void Datum::ExportBitPairSerializationStoring(FILE *outfile)
{
	Assert(m_type == DATUM_BIT_PAIR);
	Assert(m_bitPairDatum);

	switch(m_bitPairDatum->m_type) {
		case DATUM_STRINGID:
			fprintf(outfile, "        if(Get%s()){\n", m_name);
			fprintf(outfile, "            MBCHAR* tmpString = g_theStringDB->GetIdStr(m_%s);\n", m_bitPairDatum->m_name);
			fprintf(outfile, "            archive << tmpString;\n");
			fprintf(outfile, "        }\n\n");
			break;
		case DATUM_INT:
		case DATUM_FLOAT:
		case DATUM_RECORD:
		case DATUM_BIT_GROUP:
			fprintf(outfile, "        archive << m_%s;\n", m_bitPairDatum->m_name);
			break;
		case DATUM_STRUCT:
			fprintf(outfile, "        m_%s.Serialize(archive);\n", m_bitPairDatum->m_name);
			break;
		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "        archive << m_%s;\n", m_bitPairDatum->m_name);
			break;
		default:
			Assert(0);
			break;
	}
}

void Datum::ExportBitPairSerializationLoading(FILE *outfile)
{
	Assert(m_type == DATUM_BIT_PAIR);
	Assert(m_bitPairDatum);

	switch(m_bitPairDatum->m_type) {
			case DATUM_STRINGID:
				fprintf(outfile, "        {\n");
				fprintf(outfile, "            if(Get%s()){\n", m_name);
				fprintf(outfile, "                MBCHAR* tmpString = NULL;\n");
				fprintf(outfile, "                archive >> tmpString;\n");
				fprintf(outfile, "                g_theStringDB->GetStringID(tmpString, m_%s);\n", m_bitPairDatum->m_name);
				fprintf(outfile, "                delete[] tmpString;\n");
				fprintf(outfile, "            }\n");
				fprintf(outfile, "        }\n\n");
				break;
			case DATUM_INT:
			case DATUM_FLOAT:
			case DATUM_RECORD:
			case DATUM_BIT_GROUP:
				fprintf(outfile, "        archive >> m_%s;\n", m_bitPairDatum->m_name);
				break;
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "        memset((uint8*)&m_%s, 0, sizeof(m_%s));\n", m_bitPairDatum->m_name, m_bitPairDatum->m_name);
				fprintf(outfile, "        m_%s.Serialize(archive);\n", m_bitPairDatum->m_name);
				break;
			case DATUM_FILE:
			case DATUM_STRING:
				fprintf(outfile, "        m_%s = NULL;\n", m_bitPairDatum->m_name);
				fprintf(outfile, "        archive >> m_%s;\n\n", m_bitPairDatum->m_name);
				break;
			default:
				Assert(0);
				break;
	}
}

void Datum::ExportBitPairInitialization(FILE *outfile)
{
	Assert(m_type == DATUM_BIT_PAIR);
	Assert(m_bitPairDatum);

	switch(m_bitPairDatum->m_type) {
		case DATUM_INT:
		case DATUM_STRINGID:
			fprintf(outfile, "    m_%s = %d;\n", m_bitPairDatum->m_name, m_hasValue ? val.intValue : 0);
			break;
		case DATUM_FLOAT:
			fprintf(outfile, "    m_%s = %lf;\n", m_bitPairDatum->m_name, m_hasValue ? val.floatValue : 0);
			break;
		case DATUM_STRUCT:
			fprintf(outfile, "    memset(&m_%s, 0, sizeof(m_%s));\n", m_bitPairDatum->m_name, m_bitPairDatum->m_name);
			break;
		case DATUM_FILE:
		case DATUM_STRING:
			if(!m_hasValue) {
				fprintf(outfile, "    m_%s = NULL;\n", m_bitPairDatum->m_name);
			} else {
				fprintf(outfile, "    m_%s = new char[%d];\n", m_bitPairDatum->m_name, strlen(val.textValue) + 1);
				fprintf(outfile, "    strcpy(m_%s, \"%s\");\n", m_bitPairDatum->m_name, val.textValue);
			}
			break;
		case DATUM_RECORD:
			fprintf(outfile, "    m_%s = 0;\n", m_bitPairDatum->m_name);
			break;
		default:
			Assert(0);
			break;
	}
}

void Datum::ExportParseBitPairCase(FILE *outfile, char *recordName)
{
	Assert(m_type == DATUM_BIT_PAIR);
	Assert(m_bitPairDatum);

	return;
	fprintf(outfile, "            case k_Token_%s_%s_Value:\n", recordName, m_name);
	ExportBitPairDirectParse(outfile, recordName);
	fprintf(outfile, "                break;\n");
}

void Datum::ExportBitPairDirectParse(FILE *outfile, char *recordName)
{
	switch(m_bitPairDatum->m_type) {
		case DATUM_INT:
			fprintf(outfile, "                if(!lex->GetIntAssignment(m_%s)) {\n", m_bitPairDatum->m_name);
			fprintf(outfile, "                    DBERROR((\"Expected integer\"));\n");
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
			break;
		case DATUM_FLOAT:
			fprintf(outfile, "                if(!lex->GetFloatAssignment(m_%s)) {\n", m_bitPairDatum->m_name);
			fprintf(outfile, "                    DBERROR((\"Expected number\"));\n");
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
			break;
		case DATUM_RECORD:
			fprintf(outfile, "                if(!g_the%sDB->GetRecordFromLexer(lex, m_%s, err)) {\n", m_bitPairDatum->m_subType, m_bitPairDatum->m_name);
			fprintf(outfile, "                    DBERROR((\"Expected record\"));\n");
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
			break;
		case DATUM_STRUCT:
			fprintf(outfile, "                if(!m_%s.Parse(lex)) {\n", m_bitPairDatum->m_name);
			fprintf(outfile, "                    DBERROR((\"Expected struct\"));\n");
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
			break;
		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "                if(!lex->GetFileAssignment(m_%s)) {\n", m_bitPairDatum->m_name);
			fprintf(outfile, "                    DBERROR((\"Expected string\"));\n");
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
			break;
		case DATUM_STRINGID:
			fprintf(outfile, "                if(!lex->GetStringIdAssignment(m_%s)) {\n", m_bitPairDatum->m_name);
			fprintf(outfile, "                    DBERROR((\"Expected string ID\"));\n");
			fprintf(outfile, "                    done = true; break;\n");
			fprintf(outfile, "                }\n");
			break;
		default:
			Assert(0);
			break;
	}
}

void Datum::ExportInitialization(FILE *outfile)
{
	if(m_maxSize == k_MAX_SIZE_VARIABLE) {
		fprintf(outfile, "    m_%s = NULL;\n", m_name);
		fprintf(outfile, "    m_num%s = 0;\n", m_name);
	} else if(m_maxSize > 0) {
		if(m_type == DATUM_STRINGID){
			fprintf(outfile, "    memset(m_%s, 0, sizeof(m_%s));\n", m_name, m_name);
			fprintf(outfile, "    m_num%s = 0;\n", m_name);
		}
		else{
			fprintf(outfile, "    memset(m_%s, 0, sizeof(m_%s));\n", m_name, m_name);
			fprintf(outfile, "    m_num%s = 0;\n", m_name);
		}
	} else {
		switch(m_type) {
			case DATUM_INT:
			case DATUM_STRINGID:
				fprintf(outfile, "    m_%s = %d;\n", m_name, m_hasValue ? val.intValue : 0);
				break;
			case DATUM_FLOAT:
				fprintf(outfile, "    m_%s = %lf;\n", m_name, m_hasValue ? val.floatValue : 0);
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "    memset((uint8*)&m_%s, 0, sizeof(m_%s));\n",
						m_name, m_name);
				break;
			case DATUM_FILE:
			case DATUM_STRING:
				if(!m_hasValue) {
					fprintf(outfile, "    m_%s = NULL;\n", m_name);
				} else {
					fprintf(outfile, "    m_%s = new char[%d];\n", m_name, strlen(val.textValue) + 1);
					fprintf(outfile, "    strcpy(m_%s, \"%s\");\n", m_name, val.textValue);
				}
				break;
			case DATUM_RECORD:
				fprintf(outfile, "    m_%s = 0x7fffffff;\n", m_name);
				break;
			case DATUM_BIT_GROUP:
				fprintf(outfile, "    m_%s = 0;\n", m_name);
				break;
			case DATUM_BIT:
				break;
			case DATUM_BIT_PAIR:
				ExportBitPairInitialization(outfile);
				break;
			default:
				Assert(0);
				break;
		}
	}
}

void Datum::ExportSerializationStoring(FILE *outfile){

	const char *typestring = "";
	switch(m_type) {
		case DATUM_INT:
		case DATUM_STRINGID:
			typestring = "sint32";
			break;
		case DATUM_BIT:
			typestring = "uint8";
			break;
		case DATUM_FLOAT:
			typestring = "double";
			break;
		case DATUM_STRING:
		case DATUM_FILE:
			typestring = "char";
			break;
		case DATUM_RECORD:
			typestring = "sint32";
			break;
		case DATUM_STRUCT:
			typestring = m_subType;
			break;
		case DATUM_BIT_GROUP:
			typestring = "uint32";
			break;
	}

	if(m_maxSize == k_MAX_SIZE_VARIABLE) {
		if(m_type == DATUM_STRINGID){
			fprintf(outfile, "        // Free stringID not implemented\n", m_name);
		}
		else if(m_type == DATUM_STRUCT){
			fprintf(outfile, "\n        {\n");
			fprintf(outfile, "            archive << m_num%s;\n", m_name);
			fprintf(outfile, "            for(sint32 i = 0; i < m_num%s; ++i){\n", m_name);
			fprintf(outfile, "                m_%s[i].Serialize(archive);\n", m_name);
			fprintf(outfile, "            }\n");
			fprintf(outfile, "        }\n\n");
		}
		else{
			fprintf(outfile, "        archive << m_num%s;\n", m_name);
			fprintf(outfile, "        archive.Store((uint8*)m_%s, m_num%s * sizeof(%s));\n\n", m_name, m_name, typestring);
		}
	} else if(m_maxSize > 0) {
		if(m_type == DATUM_STRINGID){
			fprintf(outfile, "\n        {\n");
			fprintf(outfile, "            MBCHAR* tmpString;\n");
			fprintf(outfile, "            archive << m_num%s;\n", m_name);
			fprintf(outfile, "            for(sint32 i = 0; i < m_num%s; ++i){\n", m_name);
			fprintf(outfile, "                tmpString = g_theStringDB->GetIdStr(m_%s[i]);\n", m_name);
			fprintf(outfile, "                archive << tmpString;\n");
			fprintf(outfile, "            }\n");
			fprintf(outfile, "        }\n\n");
		}
		else{
			fprintf(outfile, "        archive << m_num%s;\n", m_name);
			fprintf(outfile, "        archive.Store((uint8*)&m_%s, sizeof(m_%s));\n", m_name, m_name);
		}
	} else {
		switch(m_type) {
			case DATUM_STRINGID:
				fprintf(outfile, "        {\n");
				fprintf(outfile, "            MBCHAR* tmpString = g_theStringDB->GetIdStr(m_%s);\n", m_name);
				fprintf(outfile, "            archive << tmpString;\n");
				fprintf(outfile, "        }\n\n");
				break;
			case DATUM_INT:
			case DATUM_FLOAT:
			case DATUM_RECORD:
			case DATUM_BIT_GROUP:
				fprintf(outfile, "        archive << m_%s;\n", m_name);
				break;
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "        m_%s.Serialize(archive);\n", m_name);
				break;
			case DATUM_FILE:
			case DATUM_STRING:
				fprintf(outfile, "        archive << m_%s;\n", m_name);
				break;
			case DATUM_BIT:
				// Does not need to be implemented
				break;
			case DATUM_BIT_PAIR:
				ExportBitPairSerializationStoring(outfile);
				break;
			default:
				Assert(0);
				break;
		}
	}
}

void Datum::ExportSerializationLoading(FILE *outfile)
{
	const char *typestring = "";
	switch(m_type) {
		case DATUM_INT:
		case DATUM_STRINGID:
			typestring = "sint32";
			break;
		case DATUM_BIT:
			typestring = "uint8";
			break;
		case DATUM_FLOAT:
			typestring = "double";
			break;
		case DATUM_STRING:
		case DATUM_FILE:
			typestring = "char";
			break;
		case DATUM_RECORD:
			typestring = "sint32";
			break;
		case DATUM_STRUCT:
			typestring = m_subType;
			break;
		case DATUM_BIT_GROUP:
			typestring = "uint32";
			break;
	}

	if(m_maxSize == k_MAX_SIZE_VARIABLE) {
		if(m_type == DATUM_STRINGID){
			fprintf(outfile, "        // Free stringID not implemented\n", m_name);
		}
		else if(m_type == DATUM_STRUCT){
			fprintf(outfile, "\n        {\n");
			fprintf(outfile, "            archive >> m_num%s;\n", m_name);
			fprintf(outfile, "            m_%s = new %s[m_num%s];\n", m_name, typestring, m_name);
			fprintf(outfile, "            for(sint32 i = 0; i < m_num%s; ++i){\n", m_name);
			fprintf(outfile, "                m_%s[i].Serialize(archive);\n", m_name);
			fprintf(outfile, "            }\n");
			fprintf(outfile, "        }\n\n");
		}
		else{
			fprintf(outfile, "        archive >> m_num%s;\n", m_name);
			fprintf(outfile, "        archive.Load((uint8*)m_%s, m_num%s * sizeof(%s));\n\n", m_name, m_name, typestring);
		}
	} else if(m_maxSize > 0) {
		if(m_type == DATUM_STRINGID){
			fprintf(outfile, "\n        {\n");
			fprintf(outfile, "            MBCHAR* tmpString;\n");
			fprintf(outfile, "            archive >> m_num%s;\n", m_name);
			fprintf(outfile, "            for(sint32 i = 0; i < m_num%s; ++i){\n", m_name);
			fprintf(outfile, "                tmpString = NULL;\n");
			fprintf(outfile, "                archive << tmpString;\n");
			fprintf(outfile, "                g_theStringDB->GetStringID(tmpString, m_%s[i]);\n", m_name);
			fprintf(outfile, "                delete[] tmpString;\n");
			fprintf(outfile, "            }\n");
			fprintf(outfile, "        }\n\n");
		}
		else{
			fprintf(outfile, "        archive >> m_num%s;\n", m_name);
			fprintf(outfile, "        archive.Load((uint8*)&m_%s, sizeof(m_%s));\n", m_name, m_name);
		}
	} else {
		switch(m_type) {
			case DATUM_STRINGID:
				fprintf(outfile, "        {\n");
				fprintf(outfile, "            MBCHAR* tmpString = NULL;\n");
				fprintf(outfile, "            archive >> tmpString;\n");
				fprintf(outfile, "            g_theStringDB->GetStringID(tmpString, m_%s);\n", m_name);
				fprintf(outfile, "            delete[] tmpString;\n");
				fprintf(outfile, "        }\n\n");
				break;
			case DATUM_INT:
			case DATUM_FLOAT:
			case DATUM_RECORD:
			case DATUM_BIT_GROUP:
				fprintf(outfile, "        archive >> m_%s;\n", m_name);
				break;
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "        memset((uint8*)&m_%s, 0, sizeof(m_%s));\n", m_name, m_name);
				fprintf(outfile, "        m_%s.Serialize(archive);\n", m_name);
				break;
			case DATUM_FILE:
			case DATUM_STRING:
				fprintf(outfile, "        m_%s = NULL;\n", m_name);
				fprintf(outfile, "        archive >> m_%s;\n\n", m_name);
				break;
			case DATUM_BIT:
				// Does not need to be implemented
				break;
			case DATUM_BIT_PAIR:
				ExportBitPairSerializationLoading(outfile);
				break;
			default:
				Assert(0);
				break;
		}
	}
}

void Datum::ExportDestructor(FILE *outfile)
{
	
	if (m_maxSize == k_MAX_SIZE_VARIABLE) {
		fprintf(outfile, "    // free array %s\n", m_name);
		switch (m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_FLOAT:
		case DATUM_RECORD:
			fprintf(outfile, "    if (m_num%s > 0)\n", m_name );
			fprintf(outfile, "        delete [m_num%s] m_%s;\n", m_name, m_name );
			fprintf(outfile, "    m_%s = NULL;\n", m_name );
			fprintf(outfile, "    m_num%s = 0;\n\n", m_name );
			break;
		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        // free string elements\n",m_name);
			fprintf(outfile, "        if (m_%s[index])\n", m_name );
			fprintf(outfile, "            delete m_%s[index];\n", m_name );
			fprintf(outfile, "    }\n");
			fprintf(outfile, "    if (m_num%s > 0)\n", m_name );
			fprintf(outfile, "        delete [m_num%s] m_%s;\n", m_name, m_name );
			fprintf(outfile, "    m_%s = NULL;\n", m_name );
			fprintf(outfile, "    m_num%s = 0;\n\n", m_name );
			break;
		case DATUM_STRUCT:
			fprintf(outfile, "    // free struct elements\n",m_name);
			fprintf(outfile, "    if (m_num%s > 0)\n", m_name );
			fprintf(outfile, "        delete [m_num%s] m_%s;\n", m_name, m_name );
			fprintf(outfile, "    m_%s = NULL;\n", m_name );
			fprintf(outfile, "    m_num%s = 0;\n\n", m_name );
			break;
		}
	} else if ( m_type ==  DATUM_BIT_PAIR ) 
	{
		switch(m_bitPairDatum->m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_FLOAT:
		case DATUM_RECORD:
		case DATUM_STRUCT:
			break;
		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "    // free string attribute %s\n",m_bitPairDatum->m_name);
			fprintf(outfile, "    if (m_%s)\n", m_bitPairDatum->m_name );
			fprintf(outfile, "        delete m_%s;\n", m_bitPairDatum->m_name );
			fprintf(outfile, "    m_%s = NULL;\n\n", m_bitPairDatum->m_name );
			break;
		}
	} else if ( m_type == DATUM_FILE || m_type == DATUM_STRING )
	{
		fprintf(outfile, "    // free string attribute %s\n",m_name);
		fprintf(outfile, "    if (m_%s)\n", m_name );
		fprintf(outfile, "        delete m_%s;\n", m_name );
		fprintf(outfile, "    m_%s = NULL;\n\n", m_name );
	}
}

void Datum::ExportOperatorEqual(FILE *outfile)
{
	
	if (m_maxSize == k_MAX_SIZE_VARIABLE) {
		fprintf(outfile, "    // free array %s\n", m_name);
		switch (m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_RECORD:
			fprintf(outfile, "    if (m_num%s > 0)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        delete [m_num%s] m_%s;\n", m_name, m_name );
			fprintf(outfile, "        m_%s = NULL;\n", m_name );
			fprintf(outfile, "        m_num%s = 0;\n", m_name );
			fprintf(outfile, "    }\n");

			fprintf(outfile, "    if (rval.m_num%s > 0)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        m_%s = new sint32 [rval.m_num%s];\n", m_name, m_name );
			fprintf(outfile, "        memcpy(m_%s, rval.m_%s, sizeof(sint32)*rval.m_num%s);\n", m_name, m_name, m_name);
			fprintf(outfile, "    }\n");
			fprintf(outfile, "    m_num%s = rval.m_num%s;\n\n",m_name,m_name);
			break;
		case DATUM_FLOAT:
			fprintf(outfile, "    if (m_num%s > 0)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        delete [m_num%s] m_%s;\n\n", m_name, m_name );
			fprintf(outfile, "        m_%s = NULL;\n", m_name );
			fprintf(outfile, "        m_num%s = 0;\n", m_name );
			fprintf(outfile, "    }\n");

			fprintf(outfile, "    if (rval.m_num%s > 0)\n", m_name );
			fprintf(outfile, "    {\n", m_name );
			fprintf(outfile, "        m_%s = new double [rval.m_num%s];\n\n", m_name, m_name );
			fprintf(outfile, "        memcpy(m_%s, rval.m_%s, sizeof(double)*rval.m_num%s);\n", m_name, m_name, m_name);
			fprintf(outfile, "    }\n");
			fprintf(outfile, "    m_num%s = rval.m_num%s;\n\n",m_name,m_name);
			break;
		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "    // free string elements of %s[]\n",m_name);
			fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
			fprintf(outfile, "        {\n");
			fprintf(outfile, "            // free string elements\n",m_name);
			fprintf(outfile, "            delete m_%s[index];\n", m_name );
			fprintf(outfile, "        }\n\n");
			
			fprintf(outfile, "    if (m_num%s > 0)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        delete [m_num%s] m_%s;\n", m_name, m_name );
			fprintf(outfile, "        m_%s = NULL;\n", m_name );
			fprintf(outfile, "    }\n\n");

			fprintf(outfile, "    if (rval.m_num%s > 0)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        m_%s = new char* [rval.m_num%s];\n", m_name, m_name );
			fprintf(outfile, "    }\n\n");

			fprintf(outfile, "    // copy string elements of %s[]\n",m_name);
			fprintf(outfile, "    for (index = 0; index < rval.m_num%s; index++)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "            m_%s[index] = new char [strlen(rval.m_%s[index])+1];\n", m_name, m_name);
			fprintf(outfile, "            strcpy(m_%s[index], rval.m_%s[index]);\n", m_name, m_name);
			fprintf(outfile, "    }\n");
			fprintf(outfile, "    m_num%s = rval.m_num%s;\n\n",m_name,m_name);
			break;

		case DATUM_STRUCT:
			fprintf(outfile, "    // free struct elements of %s[]\n",m_name);
			fprintf(outfile, "    if (m_num%s > 0)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        delete [m_num%s] m_%s;\n", m_name, m_name );
			fprintf(outfile, "        m_%s = NULL;\n", m_name );
			fprintf(outfile, "    }\n\n");

			fprintf(outfile, "    if (rval.m_num%s > 0)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        m_%s = new %s [rval.m_num%s];\n", m_name, m_subType, m_name );
			fprintf(outfile, "    }\n\n");

			fprintf(outfile, "    // copy struct elements of %s[]\n",m_name);
			fprintf(outfile, "    for (index = 0; index < rval.m_num%s; index++)\n", m_name );
			fprintf(outfile, "        {\n");
			fprintf(outfile, "            m_%s[index] = rval.m_%s[index];\n", m_name, m_name);
			fprintf(outfile, "    }\n");
			fprintf(outfile, "    m_num%s = rval.m_num%s;\n\n",m_name,m_name);

			break;
		}
	}
	else if ( m_maxSize > 0 )
	{
		fprintf(outfile, "    // copy fixed sized array %s[]\n", m_name);
		switch (m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_RECORD:
			fprintf(outfile, "    memcpy(m_%s, rval.m_%s, sizeof(sint32)*rval.m_num%s);\n\n", m_name, m_name, m_name);
			break;
		case DATUM_FLOAT:
			fprintf(outfile, "    memcpy(m_%s, rval.m_%s, sizeof(double)*rval.m_num%s);\n\n", m_name, m_name, m_name);
			break;
		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "    // free string elements of %s[]\n",m_name);
			fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        // free string elements\n",m_name);
			fprintf(outfile, "        delete m_%s[index];\n", m_name );
			fprintf(outfile, "    }\n\n");
			
			fprintf(outfile, "    // copy string elements of %s[]\n",m_name);
			fprintf(outfile, "    for (index = 0; index < rval.m_num%s; index++)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        m_%s[index] = new char [strlen(rval.m_%s[index])+1];\n", m_name, m_name);
			fprintf(outfile, "        strcpy(m_%s[index], rval.m_%s[index]);\n", m_name, m_name);
			fprintf(outfile, "    }\n\n");
			break;

		case DATUM_STRUCT:
			fprintf(outfile, "    // copy struct elements of %s[]\n",m_name);
			fprintf(outfile, "    for (index = 0; index < rval.m_num%s; index++)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        m_%s[index] = rval.m_%s[index];\n", m_name, m_name);
			fprintf(outfile, "    }\n\n");

			break;
		}
	} 
	else if ( m_type ==  DATUM_BIT_PAIR ) 
	{
		fprintf(outfile, "    m_flags%d = rval.m_flags%d;\n",
			m_bitNum / 32, m_bitNum / 32);
		switch(m_bitPairDatum->m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_FLOAT:
		case DATUM_RECORD:
		case DATUM_STRUCT:
			fprintf(outfile, "    m_%s = rval.m_%s;\n\n", 
				m_bitPairDatum->m_name, m_bitPairDatum->m_name );
			break;
		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        delete m_%s;\n", m_bitPairDatum->m_name );
			fprintf(outfile, "        m_%s = new char[strlen(rval.m_%s)+1];\n", 
				m_bitPairDatum->m_name, m_bitPairDatum->m_name );
			fprintf(outfile, "        strcpy(m_%s, rval.m_%s);\n",
				m_bitPairDatum->m_name, m_bitPairDatum->m_name );
			fprintf(outfile, "    }\n\n");
			break;
		}
	} else if ( m_type == DATUM_FILE || m_type == DATUM_STRING )
	{
			fprintf(outfile, "    if (m_%s)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "    delete m_%s;\n\n", m_name );
			fprintf(outfile, "    }\n\n");

			fprintf(outfile, "    if (rval.m_%s)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        m_%s = new char [strlen(rval.m_%s)+1];\n", m_name, m_name);
			fprintf(outfile, "        strcpy(m_%s, rval.m_%s);\n", m_name, m_name);
			fprintf(outfile, "    }\n\n");
	}
	else {
		switch(m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_FLOAT:
		case DATUM_RECORD:
		case DATUM_STRUCT:
		case DATUM_BIT_GROUP:
			fprintf(outfile, "    m_%s = rval.m_%s;\n\n", m_name, m_name);
			break;
		case DATUM_BIT:
			fprintf(outfile, "    m_flags%d = rval.m_flags%d;\n\n",m_bitNum / 32, m_bitNum / 32);
			break;
		default:
			Assert(0);
			break;
		}
	}
}

void Datum::ExportResolver(FILE  *outfile)
{
	if(m_type == DATUM_RECORD) {
		if(m_maxSize >= 0) {
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        sint32 i;\n");
			fprintf(outfile, "        for(i = 0; i < m_num%s; i++) {\n", m_name);
			fprintf(outfile, "            if(m_%s[i] & 0x80000000) {\n", m_name);		
			fprintf(outfile, "                sint32 id = m_%s[i] & 0x7fffffff;\n", m_name);
			fprintf(outfile, "                if(!g_the%sDB->GetNamedItem(id, m_%s[i])) {\n", m_subType, m_name);
			fprintf(outfile, "                    c3errors_ErrorDialog(\"DB\", \"%%s not found in %s database\", g_theStringDB->GetNameStr(id));\n",
					m_subType);
			fprintf(outfile, "                }\n");
			fprintf(outfile, "            } else if(m_%s[i] == 0x7fffffff){\n", m_name);
			fprintf(outfile, "                m_%s[i] = -1;\n", m_name);
			fprintf(outfile, "            }\n");
			fprintf(outfile, "        }\n");
			fprintf(outfile, "    }\n");
		} else {
			fprintf(outfile, "    if(m_%s & 0x80000000) {\n", m_name);
			fprintf(outfile, "        sint32 id = m_%s & 0x7fffffff;\n", m_name);
			fprintf(outfile, "        if(!g_the%sDB->GetNamedItem(id, m_%s)) {\n", m_subType, m_name);
			fprintf(outfile, "            c3errors_ErrorDialog(\"DB\", \"%%s not found in %s database\", g_theStringDB->GetNameStr(id));\n",
					m_subType);
			fprintf(outfile, "        }\n");
			fprintf(outfile, "    } else if(m_%s == 0x7fffffff) {\n", m_name);
			fprintf(outfile, "        m_%s = -1;\n", m_name);
			fprintf(outfile, "    }\n");
		}
	} else if(m_type == DATUM_STRUCT) {
		if(m_maxSize >= 0) {
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        sint32 i;\n");
			fprintf(outfile, "        for(i = 0; i < m_num%s; i++) {\n", m_name);
			fprintf(outfile, "            m_%s[i].ResolveDBReferences();\n", m_name);
			fprintf(outfile, "        }\n");
			fprintf(outfile, "    }\n");
		} else {
			fprintf(outfile, "    m_%s.ResolveDBReferences();\n", m_name);
		}
	} else if(m_type == DATUM_BIT_PAIR) {
		m_bitPairDatum->ExportResolver(outfile);
	}
}

void Datum::ExportMerge(FILE *outfile, char *recordName) 
{
	
	if (m_maxSize > 0) {
		switch (m_type) 
			{
			case DATUM_INT:
			case DATUM_STRINGID:
				
				if (m_maxSize == k_MAX_SIZE_VARIABLE)
				{
					fprintf(outfile, "    // replace array m_%s\n", m_name);
					fprintf(outfile, "    if (m_num%s != rval.m_num%s)\n", m_name, m_name);
					fprintf(outfile, "    {\n");
					fprintf(outfile, "        delete m_%s;\n", m_name, m_name);
					fprintf(outfile, "        m_%s = new sint32 [rval.m_num%s];\n", m_name, m_name);
					fprintf(outfile, "    }\n\n");
				}
				fprintf(outfile, "        m_num%s = rval.m_num%s;\n", m_name, m_name);
				fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        // replace atomic element %s\n", m_name);
				fprintf(outfile, "        m_%s[index] = rval.m_%s[index];\n", m_name, m_name);
				fprintf(outfile, "    }\n\n");
				break;
			case DATUM_FLOAT:
				
				if (m_maxSize == k_MAX_SIZE_VARIABLE)
				{
					fprintf(outfile, "    // replace array m_%s\n", m_name);
					fprintf(outfile, "    if (m_num%s != rval.m_num%s)\n", m_name, m_name);
					fprintf(outfile, "    {\n");
					fprintf(outfile, "        delete m_%s;\n", m_name, m_name);
					fprintf(outfile, "        m_%s = new double [rval.m_num%s];\n", m_name, m_name);
					fprintf(outfile, "    }\n\n");
				}
				fprintf(outfile, "        m_num%s = rval.m_num%s;\n", m_name, m_name);
				fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        // replace atomic element %s\n", m_name);
				fprintf(outfile, "        m_%s[index] = rval.m_%s[index];\n", m_name, m_name);
				fprintf(outfile, "    }\n\n");
				break;
			case DATUM_RECORD:
				
				if (m_maxSize == k_MAX_SIZE_VARIABLE)
				{
					fprintf(outfile, "    // resize m_%s if necessary\n",m_name);
					fprintf(outfile, "    if (rval.m_num%s > m_num%s)\n", m_name, m_name);
					fprintf(outfile, "    {\n");
					fprintf(outfile, "        delete m_%s;\n", m_name);
					fprintf(outfile, "        m_%s = NULL;\n", m_name);
					fprintf(outfile, "        if (rval.m_num%s > 0)\n", m_name);
					fprintf(outfile, "            m_%s = new sint32 [rval.m_num%s];\n", m_name, m_name );
					fprintf(outfile, "        m_num%s = rval.m_num%s;\n", m_name, m_name);
					fprintf(outfile, "    }\n\n");
				}
				fprintf(outfile, "    // copy values of rval.m_%s to m_%s\n",m_name,m_name);
				fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        if (index < rval.m_num%s)\n", m_name);
				fprintf(outfile, "            m_%s[index] = rval.m_%s[index];\n", m_name, m_name );
				fprintf(outfile, "    }\n");
				break;
			case DATUM_FILE:
			case DATUM_STRING:
				fprintf(outfile, "    // replace string elements of m_%s\n",m_name);
				fprintf(outfile, "    // first remove old elements of m_%s\n",m_name);
				fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        if (m_%s[index])\n", m_name);
				fprintf(outfile, "            delete m_%s[index];\n", m_name);
				fprintf(outfile, "    }\n");
				
				if (m_maxSize == k_MAX_SIZE_VARIABLE)
				{
					fprintf(outfile, "    delete m_%s;\n", m_name);
					fprintf(outfile, "    m_%s = new char * [m_num%s];\n", m_name, m_name);
				}
				fprintf(outfile, "    // copy values of rval.m_%s to m_%s\n",m_name,m_name);
				fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        m_%s[index] = new char[strlen(rval.m_%s[index])+1];\n", m_name, m_name );
				fprintf(outfile, "        strcpy(m_%s[index], rval.m_%s[index]);\n", m_name, m_name );
				fprintf(outfile, "    }\n");
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "    // replace struct elements of m_%s\n",m_name);
				fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        // find matching struct element\n" );
				
				fprintf(outfile, "        for (int obj_index = 0; obj_index < rval.m_num%s; obj_index++)\n", m_name );
				fprintf(outfile, "            if (m_%s[index] == rval.m_%s[obj_index]){\n", m_name, m_name );
				fprintf(outfile, "                // replace struct element\n");
				fprintf(outfile, "                m_%s[index] = rval.m_%s[obj_index];\n", m_name, m_name);
				
				fprintf(outfile, "                }\n");
				
				
				fprintf(outfile, "    }\n");
			}

	} else if ( m_type ==  DATUM_BIT_PAIR )
		{
			fprintf(outfile, "    // only replace values that have been set\n");
			fprintf(outfile, "    if((rval.m_flags%d & k_%s_%s_Bit) != 0)\n",
					m_bitNum / 32, recordName, m_name);
			switch(m_bitPairDatum->m_type)
				{
				case DATUM_INT:
				case DATUM_STRINGID:
				case DATUM_FLOAT:
				case DATUM_RECORD:
				case DATUM_STRUCT:
					fprintf(outfile, "    m_%s = rval.m_%s;\n\n", 
							m_bitPairDatum->m_name, m_bitPairDatum->m_name );
					break;
				case DATUM_FILE:
				case DATUM_STRING:
					fprintf(outfile, "    {\n");
					fprintf(outfile, "        delete m_%s;\n", m_bitPairDatum->m_name);
					fprintf(outfile, "        m_%s = new char[strlen(rval.m_%s)+1];\n", 
							m_bitPairDatum->m_name, m_bitPairDatum->m_name);
					fprintf(outfile, "        strcpy(m_%s, rval.m_%s);\n",
							m_bitPairDatum->m_name, m_bitPairDatum->m_name);
					fprintf(outfile, "    }\n\n");
					break;
				}
	} else if ( m_type == DATUM_BIT_GROUP ) {
		fprintf(outfile, "    // only replace values that have been set\n");
		fprintf(outfile, "    if(rval.m_%s != 0x0)\n", m_name);

		
		fprintf(outfile, "        m_%s = rval.m_%s;\n\n", m_name, m_name);
	}
}
