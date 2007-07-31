#ifndef __SLIC_FRAME_H__
#define __SLIC_FRAME_H__

enum SLIC_RT;

#include "slicif.h"
#include "SlicFunc.h"
#include "SlicStack.h"

class SlicSegment;
class SlicStack;
enum SS_TYPE;
class SlicArgList;
class MessageData;
enum SFN_ERROR;

#define k_ARGLIST_STACK_SIZE 16

class SlicFrame {
private:
	
	sint32 m_offset;
	SLIC_RT m_error;
	sint32 m_argStackPtr;
	bool m_needResult;
	sint32 m_currentLine;
	

	
	SlicSegment *m_segment;
	SlicStack *m_stack;
	SlicArgList m_argListArray[k_ARGLIST_STACK_SIZE];	
	SlicArgList *m_argList;
	SlicObject *m_resultObject;
	
	

	
	MessageData *m_messageData;

public:
	SlicFrame(SlicSegment *segment, sint32 offset = 0);
	SlicFrame(SlicSegment *segment, sint32 offset, SlicStack *stack);
	~SlicFrame();
	SLIC_RT GetError() { return m_error; }

	SlicSymbolData *GetSymbol(SS_TYPE symType, SlicStackValue symVal);
	BOOL ArrayLookup(SS_TYPE arrayType, SlicStackValue array,
					 SS_TYPE indexType, SlicStackValue indexValue,
					 SS_TYPE &retType, SlicStackValue &retValue);
	BOOL SetArrayValue(SlicSymbolData *sym,
					   SS_TYPE insType, SlicStackValue insValue,
					   SS_TYPE indexType, SlicStackValue indexValue);

	sint32 Eval(SS_TYPE type, SlicStackValue value);
	sint32 IsEqual(SS_TYPE type1, SlicStackValue value, SS_TYPE type2, SlicStackValue value2);
	void SetValue(SlicSymbolData *sym, SS_TYPE type, SlicStackValue value);

	void AddArg(SS_TYPE type, SlicStackValue value);

	BOOL DoInstruction(SOP op);
	BOOL Run();
	BOOL RunAt(sint32 startOffset);
	BOOL Step();

	void ClearMessageData();
	MessageData *GetMessageData() { return m_messageData; }
	void SetMessageData(MessageData *data);

	void ReportSFError(SFN_ERROR err, SlicSymbolData *sym);

	static sint32 FindFileOffset(const char *file, sint32 line);

	SlicStack *GetStack() { return m_stack; }

	sint32 GetCurrentLine() { return m_currentLine; }
};

#endif
