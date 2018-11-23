//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handels slic database access
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
// HAVE_PRAGMA_ONCE
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added two new functions for better slic error
//   messages and for access on the number of entries
//   in a database, addion by Martin Gühmann.
// - Added database array access. (Sep 16th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SLIC_DB_CONDUIT_H__
#define __SLIC_DB_CONDUIT_H__

class SlicDBInterface {
public:
	virtual sint32 GetIndex(const char *name) = 0;
	virtual const char *GetName() = 0;
	virtual sint32 GetValue(sint32 index, const char *valname) = 0;
	virtual sint32 GetValue(sint32 index, const char *valname, const sint32 val) = 0;
	virtual const MBCHAR *GetRecordName(const char *id) = 0;
	virtual const MBCHAR *GetRecordNameByIndex(sint32 index) = 0;
	virtual sint32 GetRecordNameID(const char *id) = 0;
	virtual sint32 GetRecordNameIDByIndex(sint32 index) = 0;
//Added by Martin Gühmann to get the number of records in a database via slic
	virtual sint32 GetNumRecords() = 0;
	virtual bool IsTokenInDB(const char *valname) { return false; }
	virtual ~SlicDBInterface() {}
};

template <class T> class CTPDatabase;

template <class T, class AccessorInfo> class SlicDBConduit : public SlicDBInterface
{
public:
	SlicDBConduit(const char *slicname, CTPDatabase<T> *db, AccessorInfo *acc, const char **tokens, sint32 numTokens) {
		m_slicname = new char[strlen(slicname) + 1];
		strcpy(m_slicname, slicname);
		m_db = db;
		m_accessors = acc;
		m_tokens = tokens;
		m_numTokens = numTokens;
	}

	virtual ~SlicDBConduit() {
		delete [] m_slicname;
	}

	sint32 GetIndex(const char *name) {
		sint32 index;
		if(m_db->GetNamedItem(name, index))
			return index;
		else
			return -1;
	}
	const char *GetName() { return m_slicname; }

	sint32 GetValue(sint32 index, const char *valname) {
		const T *rec = m_db->Get(index);
		Assert(rec);
		if(!rec)return 0; //Added by Martin Gühmann to avoid an access violation
		sint32 i;
		for(i = 0; i < m_numTokens; i++) {
			if(stricmp(valname, m_tokens[i]) == 0) {
				if(m_accessors[i].m_intAccessor) {
					return (rec->*m_accessors[i].m_intAccessor)();
				} else if(m_accessors[i].m_floatAccessor) {
					return (sint32)((rec->*m_accessors[i].m_floatAccessor)() * 100);
				} else if(m_accessors[i].m_boolAccessor) {
					return (sint32)(rec->*m_accessors[i].m_boolAccessor)();
				} else if(m_accessors[i].m_bitIntAccessor) {
					sint32 val;
					if(!(rec->*m_accessors[i].m_bitIntAccessor)(val))
						return 0;
					return val;
				} else if(m_accessors[i].m_bitFloatAccessor) {
					double val;
					if(!(rec->*m_accessors[i].m_bitFloatAccessor)(val))
						return 0;
					return (sint32)(val * 100);
				}
			}
		}
		return 0;
	}

	sint32 GetValue(sint32 index, const char *valname, const sint32 val){
		const T *rec = m_db->Get(index);
		Assert(rec);
		if(!rec)return 0;
		sint32 i;
		for(i = 0; i < m_numTokens; i++) {
			if(stricmp(valname, m_tokens[i]) == 0) {
				if(m_accessors[i].m_intArrayAccessor) {
					return (rec->*m_accessors[i].m_intArrayAccessor)(val);
				}
				else if(m_accessors[i].m_floatArrayAccessor) {
					return (sint32)((rec->*m_accessors[i].m_floatArrayAccessor)(val) * 100);
				}
			}
		}
		return 0;
	}

	const MBCHAR *GetRecordName(const char *id) {
		sint32 index;
		if(m_db->GetNamedItem(id, index)) {
			return m_db->Get(index)->GetNameText();
		} else {
			return "<err>";
		}
	}

	const MBCHAR *GetRecordNameByIndex(sint32 index) {
		if(m_db->Get(index)) {
			return m_db->Get(index)->GetNameText();
		} else {
			return "<err>";
		}
	}

	sint32 GetRecordNameIDByIndex(sint32 index) {
		if(m_db->Get(index)) {
			return m_db->Get(index)->GetName();
		} else {
			return -1;
		}
	}

	sint32 GetRecordNameID(const char *id) {
		sint32 index;
		if(m_db->GetNamedItem(id, index)) {
			return m_db->Get(index)->GetName();
		} else {
			return -1;
		}
	}

//Added by Martin Gühmann to get the number of records in a database via slic

//----------------------------------------------------------------------------
//
// Name       : GetNumRecords
//
// Description: Returns the number in the according database.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : Number of entries in the according database.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
	sint32 GetNumRecords(){
		return m_db->NumRecords();
	}

//----------------------------------------------------------------------------
//
// Name       : IsTokenInDB
//
// Description: Checks wheather a token is in the according database.
//
// Parameters : const char *valname
//
// Globals    : -
//
// Fields     : m_numTokens
//              m_tokens
//
// Returns    : Whether the token specified by valname is in the according database.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
	bool IsTokenInDB(const char *valname){
		sint32 i;
		for(i = 0; i < m_numTokens; i++) {
			if(stricmp(valname, m_tokens[i]) == 0) {
				return true;
			}
		}
		return false;
	}

private:
	CTPDatabase<T> *m_db;
	char *m_slicname;
	AccessorInfo *m_accessors;
	const char **m_tokens;
	sint32 m_numTokens;
};

#endif
