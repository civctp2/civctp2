//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database substructs record generator header
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Modified AddBitPair function to allow bit pairs to have default values
//   so that when two records are merged, only the bit is merged 
//   in that is set. - Sep. 28th 2004 Martin Gühmann
//
//----------------------------------------------------------------------------

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
#if defined(ACTIVISION_ORIGINAL)
	void AddBitPair(char *name, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype);
#else
	void AddBitPair(struct namelist *nameInfo, sint32 minSize, sint32 maxSize, struct bitpairtype *pairtype);
#endif
	
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
