//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database record generator header
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
// - None
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
//   * Added bool m_hasGovernmentsModified to the RecordDescription class
// - Modified AddBitPair function to allow bit pairs to have default values
//   so that when two records are merged, only the bit is merged
//   in that is set. - Sep. 28th 2004 Martin Gühmann
//
//----------------------------------------------------------------------------

#ifndef __RECORD_DESCRIPTION_H__
#define __RECORD_DESCRIPTION_H__

#define k_MAX_RECORD_NAME 256
#define k_MAX_STRING 256

#include "pointerlist.h"

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

// Removed by Martin Gühmann
	void AddBitPair(struct namelist *nameInfo, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype);

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

	bool m_hasGovernmentsModified;

	sint32 m_numBits;
	bool m_addingToMemberClass;
	DATUM_TYPE m_baseType;
};

#endif
