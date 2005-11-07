#ifndef __SLICSTACK_H__
#define __SLICSTACK_H__

enum SS_TYPE {
	SS_TYPE_INT,
	SS_TYPE_VAR,
	SS_TYPE_SYM,
	SS_TYPE_BAD
};

#define k_INITIAL_STACK_SIZE 512

class SlicSymbolData;
class Unit;
class MapPoint;
class Army;

typedef union {
	sint32 m_int;
	SlicSymbolData *m_sym;
} SlicStackValue;


class SlicStack {
private:
	sint32 m_sptr;
	uint8 *m_stack;
	uint32 m_stackSize;

public:
	SlicStack();
	~SlicStack();

	void Push(SS_TYPE type, SlicStackValue value);
	sint32 Pop(SS_TYPE &type, SlicStackValue &value);
#ifdef _DEBUG
	void Dump();
#endif

	
	
	static SlicSymbolData *GetSymbol(SS_TYPE symType, SlicStackValue symVal);
	static sint32 Eval(SS_TYPE type, SlicStackValue symVal);
	static BOOL GetUnit(SS_TYPE type, SlicStackValue symVal, Unit &u);
	static BOOL GetCity(SS_TYPE type, SlicStackValue symVal, Unit &city);
	static BOOL GetPos(SS_TYPE type, SlicStackValue symVal, MapPoint &pos);
	static BOOL GetArmy(SS_TYPE type, SlicStackValue symVal, Army &a);
};
#endif
