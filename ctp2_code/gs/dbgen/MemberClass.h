
#ifndef __MEMBER_CLASS_H__
#define __MEMBER_CLASS_H__

class MemberClass
{
public:
	MemberClass(char *name);
	~MemberClass();

	void AddDatum(DATUM_TYPE type, struct namelist *nameInfo, sint32 minSize, sint32 maxSize,
				  char *subType);
	void AddGroupedBits(char *name, struct namelist *list);
	void AddBitPair(char *name, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype);
	
	void ExportHeader(FILE *outfile);
	void ExportBits(FILE *outfile);
	void ExportData(FILE *outfile);
	void ExportMethods(FILE *outfile);
	void ExportForwardDeclarations(FILE *outfile);
	
	void ExportInitialization(FILE *outfile, char *recordName);
	void ExportParser(FILE *outfile, char *recordName);
	void ExportTokenCases(FILE *outfile, char *recordName);
	void ExportOtherRecordIncludes(FILE *outfile);
	void ExportDataCode(FILE *outfile, char *recordName);
	void ExportResolver(FILE *outfile, const char *recordName);

	const char *GetName() { return m_name; }

private:
	char m_name[k_MAX_RECORD_NAME];
	PointerList<Datum> m_datumList;

	sint32 m_numBits;
};

#endif
