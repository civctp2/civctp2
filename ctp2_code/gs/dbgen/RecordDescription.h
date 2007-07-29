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
// - Added ParseNum so that a certain number of entries can be parsed if 
//   braces are missing so that the old pollution database can be supported. (July 15th 2006 Martin Gühmann)
// - Added default tokens for database records. (July 15th 2006 Martin Gühmann)
// - Added map.txt support. (27-Mar-2007 Martin Gühmann)
// - Added Const.txt support. (29-Jul-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifndef RECORD_DESCRIPTION_H__
#define RECORD_DESCRIPTION_H__

#include <stdio.h>          // FILE

#define k_MAX_RECORD_NAME   256
#define k_MAX_STRING        256
class   RecordDescription;

#include "ctp2_inttypes.h"  // sint32, uint32
#include "ctpdb.h"          // DATUM_TYPE, namelist, etc. 
#include "Datum.h"          // Datum
#include "MemberClass.h"    // MemberClass
#include "pointerlist.h"    // PointerList

class RecordDescription
{
public:
	RecordDescription(char const * name, bool allowsSingleRecord = false);
	~RecordDescription();

	void SetBaseType(DATUM_TYPE type);

	void AddDatum(DATUM_TYPE type, struct namelist *nameInfo, sint32 minSize, sint32 maxSize,
				  char *subType = NULL, bool isPreBody = false);
	void AddGroupedBits(char *name, struct namelist *list);

	void AddBitPair(struct namelist *nameInfo, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype);

	void StartMemberClass(char const * name);
	void EndMemberClass(char const * name);

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
	void ExportPreBodyTokens(FILE *outfile);
	void ExportDefaultToken(FILE *outfile);
	void ExportDataParsers(FILE *outfile);
	void ExportDataCode(FILE *outfile);
	void ExportMemberClassParsers(FILE *outfile);
	void ExportOtherRecordIncludes(FILE *outfile);
	void ExportMemberClassDataCode(FILE *outfile);
	void ExportResolver(FILE *outfile);
	void SetParseNum(sint32 parseNum);
	void SetPreBody(bool preBody);

	sint32  FlagCount() const   { return (m_numBits + 31) / 32; };

private:
	char                        m_name[k_MAX_RECORD_NAME];
	PointerList<Datum>          m_datumList;
	PointerList<MemberClass>    m_memberClasses;
	bool                        m_hasGovernmentsModified;
	sint32                      m_numBits;
	bool                        m_addingToMemberClass;
	DATUM_TYPE                  m_baseType;

	sint32                      m_parseNum;           // Unimplemented; Only implemented in MemberClass
	bool                        m_preBody;            // To support the MapDB
	bool                        m_allowsSingleRecord; // To support the ConstDB
};

#endif
