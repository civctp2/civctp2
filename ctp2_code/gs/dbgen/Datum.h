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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added serilization method export. (Aug 24th 2005 Martin Gühmann)
// - Added alias names and the possibility to have default values from 
//   other entries. (Aug 26th 2005 Martin Gühmann)
// - Added support for default values taken from other databases like the 
//   Const database. (9-Dec-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifndef DATUM_H__
#define DATUM_H__

#include <memory.h>         // memset
#include <stdio.h>          // FILE
#include <string>           // std::string

class Datum;

#include "ctp2_inttypes.h"  // sint32
#include "ctpdb.h"          // DATUM_TYPE, dbvalue, etc.

class Datum
{
private:
	std::string           m_Name;

public:
	DATUM_TYPE            m_type;
	char const *          m_name;
	char *                m_akaName;
	char *                m_defaultName;
	sint32                m_minSize;
	sint32                m_maxSize;
	char *                m_subType;
	sint32                m_bitNum;
	struct namelist *     m_groupList;
	Datum *               m_bitPairDatum;
	bool                  m_required;
	bool                  m_hasValue;
	bool                  m_hasDBRefValue;
	bool                  m_isPreBody;
	union dbvalue         val;
	struct defaultDBField drefval;

	Datum
	(
	    std::string const & a_Name, 
	    DATUM_TYPE const &  a_Type  = DATUM_NONE
	) 
	:
	    m_Name          (a_Name),
	    m_type          (a_Type),
	    m_name          (m_Name.c_str()),
	    m_akaName       (NULL),
	    m_defaultName   (NULL),
	    m_minSize       (-1),
	    m_maxSize       (-1),
	    m_subType       (NULL),
	    m_bitNum        (-1),
	    m_groupList     (NULL),
	    m_bitPairDatum  (NULL),
	    m_required      (false),
	    m_hasValue      (false),
	    m_hasDBRefValue (false),
	    m_isPreBody     (false)
	{
		memset(&val, 0, sizeof(val));
	}

	virtual ~Datum(void) {};

	void SetValue(union dbvalue &v);
	void SetDBRefValue(struct defaultDBField &d);

	void ExportVariable(FILE *outfile, sint32 indent);
	void ExportRangeDefines(FILE *outfile);
	void ExportAccessor(FILE *outfile, sint32 indent, char *recordName);

	void ExportBitGroupParser(FILE *outfile, char *recordName);
	void ExportRangeCheck(FILE *outfile);
	void ExportDataCode(FILE *outfile, char *recordName);

	void ExportInitialization(FILE *outfile);
	void ExportSerializationStoring(FILE *outfile);
	void ExportSerializationLoading(FILE *outfile);
	void ExportDestructor(FILE *outfile);
	void ExportOperatorAssignment(FILE *outfile);
	void ExportOperatorEqual(FILE *outfile);
	void ExportBitPairSerializationStoring(FILE *outfile);
	void ExportBitPairSerializationLoading(FILE *outfile);
	void ExportBitPairInitialization(FILE *outfile);
	void ExportParseBitPairCase(FILE *outfile, char *recordName);
	void ExportBitPairAccessorProto(FILE *outfile, sint32 indent, char *recordName);
	void ExportBitPairDirectParse(FILE *outfile, char *recordName);

	void ExportResolver(FILE *outfile);
	void ExportMerge(FILE *outfile, char *recordName);

private:
    char const *    BitPairTypeString(void);
    char const *    ReturnTypeString(void);
    char const *    VarTypeString(void);
};

#endif
