
#pragma once
#ifndef __SLIC_NAMED_SYMBOL_H__
#define __SLIC_NAMED_SYMBOL_H__

#include "SlicSymbol.h"
#include "SlicBuiltinEnum.h"

class SlicNamedSymbol : public SlicSymbolData 
{
protected:
	
	sint32 m_index;

	
	uint8 m_fromFile;
	

	
	char *m_name;
	

	
	

public:
	SlicNamedSymbol(const char *name, SLIC_SYM type);
	SlicNamedSymbol(const char *name);
	SlicNamedSymbol(const char *name, SlicArray *array);
	SlicNamedSymbol(const char *name, SlicStructDescription *structDesc);
	SlicNamedSymbol() { m_name = NULL; }
	virtual ~SlicNamedSymbol();

	virtual bool IsParameter() const { return false; }
	virtual bool IsBuiltin() const { return false; }

	void Init(const char *name);


	void Serialize(CivArchive &archive);
	void PostSerialize();
	virtual SLIC_SYM_SERIAL_TYPE GetSerializeType() { return SLIC_SYM_SERIAL_NAMED; }

	const char *GetName() const;
	void DelName();

	sint32 GetIndex() const { return m_index; }
	void SetIndex(sint32 index) { m_index = index; }

};

class SlicParameterSymbol : public SlicNamedSymbol 
{
private:
	sint32 m_parameterIndex;

public:
	SlicParameterSymbol(const char *name, sint32 index);
	SlicParameterSymbol() {};

	void Serialize(CivArchive &archive);
	virtual SLIC_SYM_SERIAL_TYPE GetSerializeType() { return SLIC_SYM_SERIAL_PARAMETER; }

	BOOL GetIntValue(sint32 &value) const;
	BOOL GetPlayer(sint32 &value) const;
	BOOL GetPos(MapPoint &pos) const;
	BOOL GetUnit(Unit &u) const;
	BOOL GetArmy(Army &a) const;
	BOOL GetCity(Unit &c) const;

	bool IsParameter() const { return TRUE; }
};

class SlicBuiltinNamedSymbol : public SlicNamedSymbol 
{
	SLIC_BUILTIN m_builtin;
public:
	SlicBuiltinNamedSymbol(SLIC_BUILTIN which, const char *name, SlicArray *array) :
		m_builtin(which),
		SlicNamedSymbol(name, array)
	{}
	SlicBuiltinNamedSymbol(SLIC_BUILTIN which, const char *name, SlicStructDescription *structDesc) :
		m_builtin(which),
		SlicNamedSymbol(name, structDesc)
	{}
	SlicBuiltinNamedSymbol() {}

	void Serialize(CivArchive &archive);
	virtual SLIC_SYM_SERIAL_TYPE GetSerializeType() { return SLIC_SYM_SERIAL_BUILTIN; }

	bool IsBuiltin() const { return true; }
	SLIC_BUILTIN GetBuiltin() { return m_builtin; }
};
 
#endif
