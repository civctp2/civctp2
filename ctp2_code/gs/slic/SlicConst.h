#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SLIC_CONST_H__
#define __SLIC_CONST_H__

class CivArchive;

class SlicConst {
public:
	SlicConst(const MBCHAR *name, sint32 value) {
		m_name = new MBCHAR[strlen(name) * sizeof(MBCHAR) + 1];
		strcpy(m_name, name);
		m_value = value;
	}
	SlicConst(CivArchive &archive) { Serialize(archive); }

	~SlicConst() {
		if(m_name)
			delete [] m_name;
	}

	void Serialize(CivArchive &archive);

	const MBCHAR *GetName() { return m_name; }
	const sint32 GetValue() { return m_value; }

private:
	char *m_name;
	sint32 m_value;
};

enum SLIC_CONSTANTS {
	SLIC_CONST_CONTINUE,
	SLIC_CONST_GETINPUT,
	SLIC_CONST_STOP,

	SLIC_CONST_MAX
};

void slicconst_Initialize();

#endif
