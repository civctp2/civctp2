
#pragma once
#ifndef __SLIC_DB_CONDUIT_H__
#define __SLIC_DB_CONDUIT_H__

class SlicDBInterface {
public:
	virtual sint32 GetIndex(const char *name) = 0;
	virtual const char *GetName() = 0;
	virtual sint32 GetValue(sint32 index, const char *valname) = 0;
	virtual const MBCHAR *GetRecordName(const char *id) = 0;
	virtual const MBCHAR *GetRecordNameByIndex(sint32 index) = 0;
	virtual sint32 GetRecordNameID(const char *id) = 0;
	virtual sint32 GetRecordNameIDByIndex(sint32 index) = 0;
	virtual ~SlicDBInterface() {};
};

template <class T> class CTPDatabase;

template <class T, class AccessorInfo> class SlicDBConduit : public SlicDBInterface
{
public:
	SlicDBConduit(const char *slicname, CTPDatabase<T> *db, AccessorInfo *acc, char **tokens, sint32 numTokens) {
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

private:
	CTPDatabase<T> *m_db;
	char *m_slicname;
	AccessorInfo *m_accessors;
	char **m_tokens;
	sint32 m_numTokens;
};

#endif
