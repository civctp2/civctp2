/**
 * $Id$
 */
#ifndef __DATUM_H__
#define __DATUM_H__

class Datum
{
public:
	DATUM_TYPE m_type;
	char *m_name;
	sint32 m_minSize, m_maxSize;
	char *m_subType;
	sint32 m_bitNum;
	struct namelist *m_groupList;
	Datum *m_bitPairDatum;
	bool m_required;
	bool m_hasValue;
	union dbvalue val;

	~Datum();
	Datum() {
		m_type = DATUM_NONE;
		m_name = NULL;
		m_minSize = m_maxSize = -1;
		m_subType = NULL;
		m_bitNum = -1;
		m_groupList = NULL;
		m_bitPairDatum = NULL;
		m_required = false;
		m_hasValue = false;
		memset(&val, 0, sizeof(val));
	}

	void SetValue(union dbvalue &v);

	void ExportVariable(FILE *outfile, sint32 indent);
	void ExportRangeDefines(FILE *outfile);
	void ExportAccessor(FILE *outfile, sint32 indent, char *recordName);

	void ExportBitGroupParser(FILE *outfile, char *recordName);
	void ExportRangeCheck(FILE *outfile);
	void ExportDataCode(FILE *outfile, char *recordName);

	void ExportInitialization(FILE *outfile);
	void ExportDestructor(FILE *outfile);
	void ExportOperatorEqual(FILE *outfile);
	void ExportBitPairInitialization(FILE *outfile);
	void ExportParseBitPairCase(FILE *outfile, char *recordName);
	void ExportBitPairAccessorProto(FILE *outfile, sint32 indent, char *recordName);
	void ExportBitPairDirectParse(FILE *outfile, char *recordName);

	void ExportResolver(FILE *outfile);
	void ExportMerge(FILE *outfile, char *recordName);
};

#endif
