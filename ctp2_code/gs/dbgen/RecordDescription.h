
#ifndef __RECORD_DESCRIPTION_H__
#define __RECORD_DESCRIPTION_H__

#define k_MAX_RECORD_NAME 256
#define k_MAX_STRING 256

#include "PointerList.h"

#include "Datum.h"
#include "MemberClass.h"

class RecordDescription
{
public:
	RecordDescription(char *name);
	~RecordDescription();

	void SetBaseType(DATUM_TYPE type);

	void AddDatum(DATUM_TYPE type, struct namelist *nameInfo, sint32 minSize, sint32 maxSize,
				  char *subType = NULL);
	void AddGroupedBits(char *name, struct namelist *list);
	void AddBitPair(char *name, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype);

	void StartMemberClass(char *name);
	void EndMemberClass(char *name);

	void ExportHeader(FILE *outfile);
	void ExportBits(FILE *outfile);
	void ExportRanges(FILE *outfile);
	void ExportForwardDeclarations(FILE *outfile);
	void ExportData(FILE *outfile);
	void ExportMethods(FILE *outfile);
	void ExportMemberClasses(FILE *outfile);

	void ExportCode(FILE *outfile);
	void ExportManagement(FILE *outfile);
	void ExportParser(FILE *outfile);
	void ExportMerger(FILE *outfile);
	void ExportTokenCases(FILE *outfile);
	void ExportDataParsers(FILE *outfile);
	void ExportDataCode(FILE *outfile);
	void ExportMemberClassParsers(FILE *outfile);
	void ExportOtherRecordIncludes(FILE *outfile);
	void ExportMemberClassDataCode(FILE *outfile);
	void ExportResolver(FILE *outfile);

#if 0
	void ExportDBHeader(FILE *outfile);
	
	void ExportDBCode(FILE *outfile);
#endif

private:
	char m_name[k_MAX_RECORD_NAME];
	PointerList<Datum> m_datumList;
	PointerList<MemberClass> m_memberClasses;

	sint32 m_numBits;
	bool m_addingToMemberClass;
	DATUM_TYPE m_baseType;
};

#endif
