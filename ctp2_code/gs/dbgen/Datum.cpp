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
// - Modernised destructor code.
// - Fixed operator equal generation for non-integer arrays. (Jan 3rd 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "ctp2_config.h"
#include "Datum.h"

#if defined(WIN32)
#include <windows.h>
#else
#include "windows.h"
#endif

#include "ctpdb.h"
#include "RecordDescription.h"


char const * Datum::BitPairTypeString(void)
{
	switch (m_bitPairDatum->m_type)
	{
	default:
		return "void";

	case DATUM_FILE:
	case DATUM_STRING:
		return "char const *";

	case DATUM_FLOAT:
		return "double";

	case DATUM_INT:
	case DATUM_RECORD:
	case DATUM_STRINGID:
		return "sint32";
	}
}

char const * Datum::ReturnTypeString(void)
{
	switch (m_type)
	{
	default:
		return "void";

	case DATUM_BIT:
		return "uint8";

	case DATUM_BIT_GROUP:
		return "uint32";

	case DATUM_FILE:
	case DATUM_STRING:
		return "char const *";

	case DATUM_FLOAT:
		return "double";

	case DATUM_INT:
	case DATUM_RECORD:
	case DATUM_STRINGID:
		return "sint32";

	case DATUM_STRUCT:
		return m_subType;
	}
}

char const * Datum::VarTypeString(void)
{
	switch (m_type)
	{
	default:
		return "void";

	case DATUM_BIT:
		return "uint8";

	case DATUM_BIT_GROUP:
		return "uint32";

	case DATUM_FILE:
	case DATUM_STRING:
		return "char *";

	case DATUM_FLOAT:
		return "double";

	case DATUM_INT:
	case DATUM_RECORD:
	case DATUM_STRINGID:
		return "sint32";

	case DATUM_STRUCT:
		return m_subType;
	}
}

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
	if (m_type == DATUM_BIT_PAIR) 
	{
		m_bitPairDatum->ExportVariable(outfile, indent);
		return;
	}

	char sizestring[50];
	char comment[k_MAX_STRING];
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

	switch (m_type) 
	{
	default:
		break;

	case DATUM_BIT:
		Assert(0);
		break;			

	case DATUM_RECORD:
		sprintf(comment, " // Index into %s database", m_subType);
		break;
	}

	fprintf(outfile, "%s", indent ? "        " : "    ");
	fprintf(outfile, "%-15s %c m_%s%s;%s\n", VarTypeString(), notFixedStar, m_name, sizestring, comment);

	if ((m_maxSize > 0) || (m_maxSize == k_MAX_SIZE_VARIABLE)) 
	{
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
	char const *    ind         = (indent) ? "        " : "    ";
	int const       wordIndex   = m_bitNum / 32;    // sint32 as base type

	// Renamed parameterless Get-function to prevent confusion with the 
	// function that has a parameter.
	fprintf(outfile, "%sbool Has%s() const { return m_flags%d & k_%s_%s_Bit; }\n", 
	        ind, m_name, wordIndex, recordName, m_name
	       );

	if(!m_hasValue){
	/// @todo Remove the parameterless Get-function after modifiying the code to
	///       use the correct function.
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	fprintf(outfile, "__declspec(deprecated(\"Use Has%s or add parameter\"))\n", m_name);
#endif
	fprintf(outfile, "%sbool Get%s() const { return Has%s(); }\n", 
	        ind, m_name, m_name
	       );
	}

	switch (m_bitPairDatum->m_type) 
	{
	default:
		break;

	case DATUM_FILE:
	case DATUM_FLOAT:
	case DATUM_INT:
	case DATUM_STRING:
	case DATUM_STRINGID:
		if(!m_hasValue){
			fprintf(outfile, "%sbool Get%s(%s & value) const\n", ind, m_name, BitPairTypeString());
			fprintf(outfile, "%s{\n", ind);
			fprintf(outfile, "%s    if (m_flags%d & k_%s_%s_Bit)\n", ind, wordIndex, recordName, m_name);
			fprintf(outfile, "%s    {\n", ind);
			fprintf(outfile, "%s        value = m_%s;\n", ind, m_bitPairDatum->m_name);
			fprintf(outfile, "%s    }\n", ind);
			fprintf(outfile, "%s    return m_flags%d & k_%s_%s_Bit;\n", ind, wordIndex, recordName, m_name);
			fprintf(outfile, "%s}\n", ind);
		}
		else{
			fprintf(outfile, "%s%s Get%s() const { return m_%s; }\n", ind, BitPairTypeString(), m_name, m_bitPairDatum->m_name);
		}
		break;

	case DATUM_RECORD:
		fprintf(outfile, "%sbool Get%sIndex(sint32 & index) const\n", ind, m_name);
		fprintf(outfile, "%s{\n", ind);
		fprintf(outfile, "%s    index = (m_flags%d & k_%s_%s_Bit) ? m_%s : -1;\n", 
		        ind, wordIndex, recordName, m_name, m_bitPairDatum->m_name
		       );
		fprintf(outfile, "%s    return m_flags%d & k_%s_%s_Bit;\n", 
		        ind, wordIndex, recordName, m_name
		       );
		fprintf(outfile, "%s}\n", ind);

		fprintf(outfile, "%sbool Get%s(const %sRecord * & rec) const;\n", 
		        ind, m_name, m_bitPairDatum->m_subType
		       );
		fprintf(outfile, "%sconst %sRecord * Get%sPtr() const;\n", 
		        ind, m_bitPairDatum->m_subType, m_name
		       );
		break;

	case DATUM_STRUCT:
		fprintf(outfile, "%sbool Get%s(const %s *&ptr) const\n", 
		        ind, m_name, m_bitPairDatum->m_subType
		       );
		fprintf(outfile, "%s{\n", ind);
		fprintf(outfile, "%s    ptr = (m_flags%d & k_%s_%s_Bit) ? &m_%s : NULL;\n", 
		        ind, wordIndex, recordName, m_name, m_bitPairDatum->m_name);
		fprintf(outfile, "%s    return m_flags%d & k_%s_%s_Bit;\n", 
		        ind, wordIndex, recordName, m_name
		       );
		fprintf(outfile, "%s}\n", ind);

		fprintf(outfile, "%sconst %s * Get%sPtr() const\n", 
		        ind, m_bitPairDatum->m_subType, m_name
		       );
		fprintf(outfile, "%s{\n", ind);
		fprintf(outfile, "%s    return (m_flags%d & k_%s_%s_Bit) ? &m_%s : NULL;\n", 
				ind, wordIndex, recordName, m_name, m_bitPairDatum->m_name
		       );
		fprintf(outfile, "%s}\n", ind);
		break;
	}
}

void Datum::ExportAccessor(FILE *outfile, sint32 indent, char *recordName)
{
	char const *    ind         = (indent) ? "        " : "    ";
	int const       wordIndex   = m_bitNum / 32;    // sint32 as base type

	if (m_maxSize <= 0) 
	{
		switch (m_type) 
		{
		default:
			Assert(0);
			break;

		case DATUM_FILE:
		case DATUM_FLOAT:
		case DATUM_INT:
		case DATUM_STRING:
		case DATUM_STRINGID:
			fprintf(outfile, "%s%s Get%s() const { return m_%s; }\n", ind, ReturnTypeString(), m_name, m_name);
			break;

		case DATUM_BIT:
			fprintf(outfile, "%sbool Get%s() const { return m_flags%d & k_%s_%s_Bit; }\n",
					ind, m_name, wordIndex, recordName, m_name);
			break;
		
		case DATUM_BIT_PAIR:
			ExportBitPairAccessorProto(outfile, indent, recordName);
			break;
				
		case DATUM_RECORD:
			fprintf(outfile, "%ssint32           Get%sIndex() const { return m_%s; }\n", ind, m_name, m_name);
			fprintf(outfile, "%s%sRecord const * Get%s() const;\n", ind, m_subType, m_name);
			break;

		case DATUM_BIT_GROUP:
			{
				fprintf(outfile, "%suint32           Get%s() const { return m_%s; }\n", ind, m_name, m_name);
				for (struct namelist * node = m_groupList; node; node = node->next)
				{
					fprintf(outfile, "%sbool             Get%s%s() const { return m_%s & k_%s_%s_%s_Bit; }\n",
					        ind, m_name, node->name, m_name, recordName, m_name, node->name
					       );
				}
			}
			break;

		case DATUM_STRUCT:
			fprintf(outfile, "%s%s const * Get%s() const { return &m_%s; }\n", 
			        ind, ReturnTypeString(), m_name, m_name
			       );
			break;
		}
	}
	else
	{
		switch (m_type) 
		{
		default:
			break;

		case DATUM_FILE:
		case DATUM_FLOAT:
		case DATUM_INT:
		case DATUM_STRING:
		case DATUM_STRINGID:
			fprintf(outfile, "%s%s Get%s(sint32 index) const;\n", ReturnTypeString(), ind, m_name);
			break;

		case DATUM_RECORD:
			fprintf(outfile, "%ssint32 Get%sIndex(sint32 index) const;\n", ind, m_name);
			fprintf(outfile, "%sconst %sRecord * Get%s(sint32 index) const;\n", ind, m_subType, m_name);
			break;

		case DATUM_STRUCT:
			fprintf(outfile, "%sconst %s * Get%s(sint32 index) const;\n", ind, m_subType, m_name);
			break;
		}
		fprintf(outfile, "%ssint32 GetNum%s() const { return m_num%s;}\n", ind, m_name, m_name);
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
				fprintf(outfile, "%sRecord const * %sRecord::Get%s() const\n", m_subType, recordName, m_name);
				fprintf(outfile, "{\n");
				fprintf(outfile, "    return g_the%sDB->Get(m_%s);\n", m_subType, m_name);
				fprintf(outfile, "}\n\n");
				break;
			case DATUM_BIT_PAIR:
				if(m_bitPairDatum->m_type == DATUM_RECORD) {
					fprintf(outfile, "%sRecord const * %sRecord::Get%sPtr() const\n", m_bitPairDatum->m_subType, recordName, m_name);
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
			case DATUM_FILE:
			case DATUM_FLOAT:
			case DATUM_INT:
			case DATUM_STRING:
			case DATUM_STRINGID:
				fprintf(outfile, "%s %sRecord::Get%s(sint32 index) const\n", ReturnTypeString(), recordName, m_name);
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
				
				fprintf(outfile, "%sRecord const * %sRecord::Get%s(sint32 index) const\n", m_subType, recordName, m_name);
				fprintf(outfile, "{\n");
				ExportRangeCheck(outfile);
				fprintf(outfile, "    return g_the%sDB->Get(m_%s[index]);\n", m_subType, m_name);
				fprintf(outfile, "}\n\n");
				break;

			case DATUM_STRUCT:
				fprintf(outfile, "%sRecord::%s const * %sRecord::Get%s(sint32 index) const\n", recordName, m_subType, recordName, m_name);
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

	switch (m_bitPairDatum->m_type) 
	{
		case DATUM_STRINGID:
			fprintf(outfile, "        if (Has%s()){\n", m_name);
			fprintf(outfile, "            MBCHAR* tmpString = g_theStringDB->GetIdStr(m_%s);\n", m_bitPairDatum->m_name);
			fprintf(outfile, "            archive << tmpString;\n");
			fprintf(outfile, "        }\n\n");
			break;

		case DATUM_INT:
		case DATUM_FILE:
		case DATUM_FLOAT:
		case DATUM_RECORD:
		case DATUM_BIT_GROUP:
		case DATUM_STRING:
			fprintf(outfile, "        archive << m_%s;\n", m_bitPairDatum->m_name);
			break;

		case DATUM_STRUCT:
			fprintf(outfile, "        m_%s.Serialize(archive);\n", m_bitPairDatum->m_name);
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
				fprintf(outfile, "            if (Has%s()){\n", m_name);
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
			if (m_hasValue) 
			{
				fprintf(outfile, "    m_%s = new char[%d];\n", m_bitPairDatum->m_name, strlen(val.textValue) + 1);
				fprintf(outfile, "    strcpy(m_%s, \"%s\");\n", m_bitPairDatum->m_name, val.textValue);
			}
			else
			{
				fprintf(outfile, "    m_%s = NULL;\n", m_bitPairDatum->m_name);
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
	/// @todo Find out if this function is needed.

#if 0   // Useless or unreachable
	Assert(m_type == DATUM_BIT_PAIR);
	Assert(m_bitPairDatum);

	return;
	fprintf(outfile, "            case k_Token_%s_%s_Value:\n", recordName, m_name);
	ExportBitPairDirectParse(outfile, recordName);
	fprintf(outfile, "                break;\n");
#endif
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
			fprintf(outfile, "                if(!g_the%sDB->GetRecordFromLexer(lex, m_%s)) {\n", m_bitPairDatum->m_subType, m_bitPairDatum->m_name);
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
			fprintf(outfile, "    memset(m_%s, 0, sizeof(m_%s));\n", m_name, m_name);
			fprintf(outfile, "    m_num%s = 0;\n", m_name);
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
				if (m_hasValue)
				{
					fprintf(outfile, "    m_%s = new char[%d];\n", m_name, strlen(val.textValue) + 1);
					fprintf(outfile, "    strcpy(m_%s, \"%s\");\n", m_name, val.textValue);
				}
				else
				{
					fprintf(outfile, "    m_%s = NULL;\n", m_name);
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
			fprintf(outfile, "        archive.Store((uint8*)m_%s, m_num%s * sizeof(%s));\n\n", 
			        m_name, m_name, VarTypeString()
			       );
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
			case DATUM_FILE:
			case DATUM_FLOAT:
			case DATUM_RECORD:
			case DATUM_BIT_GROUP:
			case DATUM_STRING:
				fprintf(outfile, "        archive << m_%s;\n", m_name);
				break;
				break;
			case DATUM_STRUCT:
				fprintf(outfile, "        m_%s.Serialize(archive);\n", m_name);
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
	if(m_maxSize == k_MAX_SIZE_VARIABLE) {
		if(m_type == DATUM_STRINGID){
			fprintf(outfile, "        // Free stringID not implemented\n", m_name);
		}
		else if(m_type == DATUM_STRUCT){
			fprintf(outfile, "\n        {\n");
			fprintf(outfile, "            archive >> m_num%s;\n", m_name);
			fprintf(outfile, "            m_%s = new %s[m_num%s];\n", m_name, VarTypeString(), m_name);
			fprintf(outfile, "            for(sint32 i = 0; i < m_num%s; ++i){\n", m_name);
			fprintf(outfile, "                m_%s[i].Serialize(archive);\n", m_name);
			fprintf(outfile, "            }\n");
			fprintf(outfile, "        }\n\n");
		}
		else{
			fprintf(outfile, "        archive >> m_num%s;\n", m_name);
			fprintf(outfile, "        archive.Load((uint8*)m_%s, m_num%s * sizeof(%s));\n\n", 
			        m_name, m_name, VarTypeString()
			       );
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
	if (m_maxSize == k_MAX_SIZE_VARIABLE) 
	{
		switch (m_type) 
		{
		default:    // Plain array
			fprintf(outfile, "    delete [] m_%s;\n", m_name );
			break;

		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "    for (index = 0; index < m_num%s; ++index)\n", m_name );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        delete m_%s[index];\n", m_name );
			fprintf(outfile, "    }\n");
			fprintf(outfile, "    delete [] m_%s;\n", m_name );
			break;
		}
	}
	else if ( m_type ==  DATUM_BIT_PAIR )
	{
		switch (m_bitPairDatum->m_type)
		{
		default:    // No action
			break;

		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "    delete m_%s;\n", m_bitPairDatum->m_name );
			break;
		}
	} 
	else if ( m_type == DATUM_FILE || m_type == DATUM_STRING )
	{
		fprintf(outfile, "    delete m_%s;\n", m_name );
	}
}

#if 0
void Datum::ExportOperatorEqual(FILE *outfile) // == not implemented
#endif

void Datum::ExportOperatorAssignment(FILE *outfile)
{
	char const ind []   = "        ";
	
	if (m_maxSize == k_MAX_SIZE_VARIABLE) 
	{
		switch (m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_RECORD:
		case DATUM_FLOAT:
        case DATUM_STRUCT:
			fprintf(outfile, "%sdelete [] m_%s;\n", ind, m_name);
			fprintf(outfile, "%sm_%s = NULL;\n", ind, m_name);

			fprintf(outfile, "%sif (rval.m_num%s > 0)\n", ind, m_name);
			fprintf(outfile, "%s{\n", ind);
			fprintf(outfile, "%s    m_%s = new %s [rval.m_num%s];\n", 
			        ind, m_name, VarTypeString(), m_name
			       );
			fprintf(outfile, "%s    std::copy(rval.m_%s, rval.m_%s + rval.m_num%s, m_%s);\n", 
			        ind, m_name, m_name, m_name, m_name
			       );
			fprintf(outfile, "%s}\n", ind);

			fprintf(outfile, "%sm_num%s = rval.m_num%s;\n\n", ind, m_name, m_name);
			break;

		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "%sfor (index = 0; index < m_num%s; ++index)\n", ind, m_name);
			fprintf(outfile, "%s{\n");
			fprintf(outfile, "%s    delete m_%s[index];\n", ind, m_name);
			fprintf(outfile, "%s}\n");
			fprintf(outfile, "%sdelete [] m_%s;\n", ind, m_name);
			fprintf(outfile, "%sm_%s = NULL;\n", ind, m_name);
			
			fprintf(outfile, "%sif (rval.m_num%s > 0)\n", ind, m_name);
			fprintf(outfile, "%s{\n", ind);
			fprintf(outfile, "%s    m_%s = new char * [rval.m_num%s];\n", ind, m_name, m_name);
			fprintf(outfile, "%s}\n\n");

			fprintf(outfile, "%s// copy string elements of %s[]\n", ind, m_name);
			fprintf(outfile, "%sfor (index = 0; index < rval.m_num%s; index++)\n", ind, m_name);
			fprintf(outfile, "%s{\n");
			fprintf(outfile, "%s    m_%s[index] = new char [strlen(rval.m_%s[index])+1];\n", ind, m_name, m_name);
			fprintf(outfile, "%s    strcpy(m_%s[index], rval.m_%s[index]);\n", ind, m_name, m_name);
			fprintf(outfile, "%s}\n");
			fprintf(outfile, "%sm_num%s = rval.m_num%s;\n\n", ind, m_name, m_name);
			break;
		}
	}
	else if ( m_maxSize > 0 )
	{
		switch (m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_RECORD:
		case DATUM_FLOAT:
			fprintf(outfile, "%sstd::copy(rval.m_%s, rval.m_%s + rval.m_num%s, m_%s);\n\n", 
			        ind, m_name, m_name, m_name, m_name
			       );
			break;

		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "%s// free string elements of %s[]\n", ind, m_name);
			fprintf(outfile, "%sfor (index = 0; index < m_num%s; ++index)\n", ind, m_name);
			fprintf(outfile, "%s{\n", ind);
			fprintf(outfile, "%s    delete m_%s[index];\n", ind, m_name);
			fprintf(outfile, "%s}\n\n", ind);
			
			fprintf(outfile, "%s// copy string elements of %s[]\n", ind, m_name);
			fprintf(outfile, "%sfor (index = 0; index < rval.m_num%s; index++)\n", ind, m_name);
			fprintf(outfile, "%s{\n", ind);
			fprintf(outfile, "%s    m_%s[index] = new char [strlen(rval.m_%s[index])+1];\n", ind, m_name, m_name);
			fprintf(outfile, "%s    strcpy(m_%s[index], rval.m_%s[index]);\n", ind, m_name, m_name);
			fprintf(outfile, "%s}\n\n", ind);
			break;

		case DATUM_STRUCT:
			fprintf(outfile, "%s// copy struct elements of %s[]\n", ind, m_name);
			fprintf(outfile, "%sfor (index = 0; index < rval.m_num%s; index++)\n", ind, m_name);
			fprintf(outfile, "%s{\n", ind);
			fprintf(outfile, "%s    m_%s[index] = rval.m_%s[index];\n", ind, m_name, m_name);
			fprintf(outfile, "%s}\n\n", ind);

			break;
		}
	} 
	else if ( m_type ==  DATUM_BIT_PAIR ) 
	{
		switch (m_bitPairDatum->m_type) 
		{
		case DATUM_INT:
		case DATUM_STRINGID:
		case DATUM_FLOAT:
		case DATUM_RECORD:
		case DATUM_STRUCT:
			fprintf(outfile, "%sm_%s = rval.m_%s;\n\n", 
			        ind, m_bitPairDatum->m_name, m_bitPairDatum->m_name 
			       );
			break;

		case DATUM_FILE:
		case DATUM_STRING:
			fprintf(outfile, "%s{\n", ind);
			fprintf(outfile, "%s    delete [] m_%s;\n", ind, m_bitPairDatum->m_name );
			fprintf(outfile, "%s    m_%s = new char[strlen(rval.m_%s)+1];\n", 
			        ind, m_bitPairDatum->m_name, m_bitPairDatum->m_name 
			       );
			fprintf(outfile, "%s    strcpy(m_%s, rval.m_%s);\n",
			        ind, m_bitPairDatum->m_name, m_bitPairDatum->m_name 
			       );
			fprintf(outfile, "%s}\n\n", ind);
			break;
		}
	}
	else if (m_type == DATUM_FILE || m_type == DATUM_STRING)
	{
		fprintf(outfile, "%sdelete [] m_%s;\n", ind, m_name);
		fprintf(outfile, "%sm_%s = NULL;\n\n", ind, m_name);

		fprintf(outfile, "%sif (rval.m_%s)\n", ind, m_name);
		fprintf(outfile, "%s{\n", ind);
		fprintf(outfile, "%s    m_%s = new char [strlen(rval.m_%s)+1];\n", ind, m_name, m_name);
		fprintf(outfile, "%s    strcpy(m_%s, rval.m_%s);\n", ind, m_name, m_name);
		fprintf(outfile, "%s}\n\n", ind);
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
			fprintf(outfile, "%sm_%s = rval.m_%s;\n\n", ind, m_name, m_name);
			break;
		case DATUM_BIT:
			// No action: part of m_flags
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
			case DATUM_FLOAT:
			case DATUM_INT:
			case DATUM_STRINGID:
				
				if (m_maxSize == k_MAX_SIZE_VARIABLE)
				{
					fprintf(outfile, "    // replace array m_%s\n", m_name);
					fprintf(outfile, "    if (m_num%s != rval.m_num%s)\n", m_name, m_name);
					fprintf(outfile, "    {\n");
					fprintf(outfile, "        delete [] m_%s;\n", m_name, m_name);
					fprintf(outfile, "        m_%s = new %s [rval.m_num%s];\n", m_name, VarTypeString(), m_name);
					fprintf(outfile, "    }\n");
				}
				fprintf(outfile, "    m_num%s = rval.m_num%s;\n", m_name, m_name);
				fprintf(outfile, "    std::copy(rval.m_%s, rval.m_%s + rval.m_num%s, m_%s);\n\n",
				                      m_name, m_name, m_name, m_name
				       );
				break;
				
			case DATUM_RECORD:
				if (m_maxSize == k_MAX_SIZE_VARIABLE)
				{
					fprintf(outfile, "    // resize m_%s if necessary\n",m_name);
					fprintf(outfile, "    if (rval.m_num%s > m_num%s)\n", m_name, m_name);
					fprintf(outfile, "    {\n");
					fprintf(outfile, "        delete [] m_%s;\n", m_name);
					fprintf(outfile, "        m_%s = NULL;\n", m_name);
					fprintf(outfile, "        if (rval.m_num%s > 0)\n", m_name);
					fprintf(outfile, "            m_%s = new sint32 [rval.m_num%s];\n", m_name, m_name );
					fprintf(outfile, "        m_num%s = rval.m_num%s;\n", m_name, m_name);
					fprintf(outfile, "    }\n");
				}

 				fprintf(outfile, "    if (rval.m_num%s > 0)\n", m_name);
				fprintf(outfile, "        std::copy(rval.m_%s, rval.m_%s + rval.m_num%s, m_%s);\n\n",
				                      m_name, m_name, m_name, m_name
				       );
				break;

			case DATUM_FILE:
			case DATUM_STRING:
				fprintf(outfile, "    // replace string elements of m_%s\n",m_name);
				fprintf(outfile, "    // first remove old elements of m_%s\n",m_name);
				fprintf(outfile, "    for (index = 0; index < m_num%s; index++)\n", m_name );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        delete m_%s[index];\n", m_name);
				fprintf(outfile, "    }\n");
				
				if (m_maxSize == k_MAX_SIZE_VARIABLE)
				{
					fprintf(outfile, "    delete [] m_%s;\n", m_name);
					fprintf(outfile, "    m_%s = new char * [m_num%s];\n", m_name, m_name);
				}
				fprintf(outfile, "    // copy values of rval.m_%s to m_%s\n",m_name,m_name);
				fprintf(outfile, "    for (sint32 index_%s = 0; index_%s < m_num%s; ++index_%s)\n", 
				                        m_name, m_name, m_name, m_name
				       );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        m_%s[index_%s] = new char[strlen(rval.m_%s[index_%s])+1];\n", 
				                        m_name, m_name, m_name, m_name
				       );
				fprintf(outfile, "        strcpy(m_%s[index_%s], rval.m_%s[index_%s]);\n", 
				                        m_name, m_name, m_name, m_name
				       );
				fprintf(outfile, "    }\n");
				break;

			case DATUM_STRUCT:
				fprintf(outfile, "    // replace struct elements of m_%s\n",m_name);
				fprintf(outfile, "    for (sint32 index_%s = 0; index_%s < m_num%s; ++index_%s)\n", 
				                        m_name, m_name, m_name, m_name
				       );
				fprintf(outfile, "    {\n");
				fprintf(outfile, "        // find matching struct element and replace it\n" );
				fprintf(outfile, "        for (sint32 obj_index = 0; obj_index < rval.m_num%s; ++obj_index)\n", m_name );
				fprintf(outfile, "            if (m_%s[index_%s] == rval.m_%s[obj_index]){\n", 
				                        m_name, m_name, m_name
				       );
				fprintf(outfile, "                m_%s[index_%s] = rval.m_%s[obj_index];\n", 
				                        m_name, m_name, m_name
				       );
				fprintf(outfile, "                }\n");
				fprintf(outfile, "    }\n");
			}

	}
	else if (m_type == DATUM_BIT_PAIR)
		{
			switch(m_bitPairDatum->m_type)
				{
				case DATUM_INT:
				case DATUM_STRINGID:
				case DATUM_FLOAT:
				case DATUM_RECORD:
				case DATUM_STRUCT:
			fprintf(outfile, "    // only replace values that have been set\n");
			fprintf(outfile, "    if (rval.m_flags%d & k_%s_%s_Bit)\n",
			        m_bitNum / 32, recordName, m_name
			       );
			fprintf(outfile, "    {\n");
			fprintf(outfile, "        m_%s = rval.m_%s;\n",
			        m_bitPairDatum->m_name, m_bitPairDatum->m_name
			       );
			fprintf(outfile, "    }\n\n");
					break;

				case DATUM_FILE:
				case DATUM_STRING:
			fprintf(outfile, "    // only replace values that have been set\n");
			fprintf(outfile, "    if (rval.m_flags%d & k_%s_%s_Bit)\n",
					m_bitNum / 32, recordName, m_name
			       );
					fprintf(outfile, "    {\n");
					fprintf(outfile, "        delete m_%s;\n", m_bitPairDatum->m_name);
					fprintf(outfile, "        m_%s = new char[strlen(rval.m_%s)+1];\n",
							m_bitPairDatum->m_name, m_bitPairDatum->m_name);
					fprintf(outfile, "        strcpy(m_%s, rval.m_%s);\n",
							m_bitPairDatum->m_name, m_bitPairDatum->m_name);
					fprintf(outfile, "    }\n\n");
					break;
				}
	}
	else if (m_type == DATUM_BIT_GROUP) 
	{
		fprintf(outfile, "    // only replace values that have been set\n");
		fprintf(outfile, "    if (rval.m_%s)\n", m_name);
		fprintf(outfile, "    {\n");
		fprintf(outfile, "        m_%s = rval.m_%s;\n", m_name, m_name);
		fprintf(outfile, "    }\n\n");
	}
}
