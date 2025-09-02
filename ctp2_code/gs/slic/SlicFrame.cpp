//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic code interpreter
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
// - Added slic database access, by Martin G�hmann.
// - Added a way to find out the size of a slic database, by Martin G�hmann.
// - Repaired memory leak caused by one of the contructors, by Martin G�hmann.
// - Replaced Debug Assertion for slic division by 0 by slic error message
//   the problem must be solved by the slicer not by a c++ coder,
//   by Martin G�hmann
// - Treat modulo (%) 0 errors in the same way as errors with division by 0.
// - Fixed slic database access after a reload by Martin G�hmann.
// - Added '**' operator handling
// - Added bitwise operator handling
// - Repaired memory leaks.
// - Repaired crash with invalid input.
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
// - Added database array access. (Sep 16th 2005 Martin G�hmann)
// - Repaired crashes with game saved with original Activision executable.
// - Standardized code. (May 29th 2006 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"
#include "SlicSegment.h"
#include "SlicError.h"
#include "slicif.h"
#include "SlicStack.h"
#include "SlicFrame.h"
#include "SlicFunc.h"
#include "SlicEngine.h"
#include "SlicSymbol.h"
#include "MessageData.h"
#include "TurnCnt.h"            // g_turn
#include "UnitDynArr.h"
#include "SlicObject.h"
#include "SlicButton.h"
#include "MapPoint.h"
#include "SlicStruct.h"
#include "SlicNamedSymbol.h"
#include "SlicConditional.h"
#include "GameEventManager.h"
#include "SlicArray.h"
#include "profileDB.h"

//Added by Martin G�hmann for database access
#include "SlicDBConduit.h"
#include <math.h>

extern "C" FILE *debuglog;

namespace
{
    SlicDBInterface * GetDatabase(unsigned char * & instructionPointer)
    {
        SlicDBInterface *   namedDatabase =
            g_slicEngine->GetDBConduit(reinterpret_cast<char *>(instructionPointer));

        if (namedDatabase)
        {
		    // Skip the name string
            for ( ; *instructionPointer++; )
            {
                // No action: just skip
            }

            return namedDatabase;
        }
        else
        {
            // Compatibility mode: DB is stored by index with the Activision 1.1 patch
            int     dbIndex     = *(reinterpret_cast<int *>(instructionPointer));
            instructionPointer += sizeof(int);

            switch (dbIndex)
            {
            default:    return NULL;
            case 0:     return g_slicEngine->GetDBConduit("UnitDB");
            // Probably 1, etc. are other databases. TODO: confirm and add here.
            }
        }
    }

} // namespace

SlicFrame::SlicFrame(SlicSegment *segment, sint32 offset)
:
    m_offset       (offset),
    m_error        (SLIC_RT_OK),
    m_argStackPtr  (-1),
    m_currentLine  (-1),
    m_segment      (segment),
    m_stack        (new SlicStack),
  //m_argListArray
    m_argList      (NULL),
    m_resultObject (NULL),
    m_messageData  (NULL)
{
}

SlicFrame::SlicFrame(SlicSegment *segment, sint32 offset, SlicStack *stack)
:
    m_offset       (offset),
    m_error        (SLIC_RT_OK),
    m_argStackPtr  (-1),
    m_currentLine  (-1),
    m_segment      (segment),
    m_stack        (stack),
  //m_argListArray
    m_argList      (NULL),
    m_resultObject (NULL),
    m_messageData  (NULL)
{
}

//----------------------------------------------------------------------------
//
// Name       : ~SlicFrame
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
SlicFrame::~SlicFrame()
{
	delete m_stack;
	delete m_messageData;

	if (m_resultObject)
	{
		m_resultObject->Release();
	}
}

BOOL SlicFrame::ArrayLookup(SS_TYPE arrayType, SlicStackValue array,
							SS_TYPE indexType, SlicStackValue indexValue,
							SS_TYPE &retType, SlicStackValue &retValue)
{
	SlicSymbolData * arraySym = SlicStack::GetSymbol(arrayType, array);

	if (arraySym)
    {
	    sint32 index = Eval(indexType, indexValue);
	    return arraySym->ArrayLookup(index, retType, retValue);
    }
    else
    {
		c3errors_ErrorDialog("Slic", "Array is not a variable");
		return FALSE;
	}
}

sint32 SlicFrame::Eval(SS_TYPE type, SlicStackValue value)
{
	return SlicStack::Eval(type, value);
}

sint32 SlicFrame::IsEqual(SS_TYPE type1, SlicStackValue value1,
						  SS_TYPE type2, SlicStackValue value2)
{
	SlicSymbolData *sym1, *sym2;
	if(type1 == SS_TYPE_INT || type2 == SS_TYPE_INT) {
		return Eval(type1, value1) == Eval(type2, value2);
	}

	switch(type1) {
		case SS_TYPE_SYM:
		case SS_TYPE_VAR:
		{
			if(type1 == SS_TYPE_SYM) {
				sym1 = value1.m_sym;
			} else {
				sym1 = g_slicEngine->GetSymbol(value1.m_int);
			}

			if(type2 == SS_TYPE_SYM) {
				sym2 = value2.m_sym;
			} else if(type2 == SS_TYPE_VAR) {
				sym2 = g_slicEngine->GetSymbol(value2.m_int);
			} else {
				Assert(FALSE);
				return 0;
			}

			MapPoint pos1, pos2;
			Unit u1, u2;
			if(sym1->GetPos(pos1)) {
				if(sym2->GetPos(pos2)) {
					return pos1 == pos2;
				}
			}
			if(sym1->GetUnit(u1)) {
				if(sym2->GetUnit(u2)) {
					return u1 == u2;
				}
			}
			if(sym1->GetCity(u1)) {
				if(sym2->GetCity(u2)) {
					return u1 == u2;
				}
			}

			if(sym1->GetType() != SLIC_SYM_IVAR)
				return 0;

			if(sym2->GetType() != SLIC_SYM_IVAR)
				return 0;

			return Eval(type1, value1) == Eval(type2, value2);
		}
		default:
			Assert(FALSE);
			return 0;
	}
}

void SlicFrame::SetValue(SlicSymbolData *sym, SS_TYPE type, SlicStackValue value)
{
	SlicSymbolData *getsym;
	switch(type) {
		case SS_TYPE_INT:
			sym->SetIntValue(Eval(type, value));
			break;
		case SS_TYPE_VAR:
			getsym = g_slicEngine->GetSymbol(value.m_int);
			if(!sym->SetValueFrom(getsym)) {
				char buf[1024];
				sprintf(buf, "In object %s, variables '%s' and '%s' are of different types", m_segment->GetName(), sym->GetName(), getsym->GetName());
				if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
					c3errors_ErrorDialog("Slic", buf);
				}
			}
			break;
		case SS_TYPE_SYM:
			getsym = value.m_sym;
			if(!sym->SetValueFrom(getsym)) {
				char buf[1024];
				sprintf(buf, "In object %s, variables '%s' and '%s' are of different types", m_segment->GetName(), sym->GetName(), getsym->GetName());
				if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
					c3errors_ErrorDialog("Slic", buf);
				}
			}
			break;
		default:
		{
			BOOL BadTypeInAssignment = FALSE;
			Assert(BadTypeInAssignment);
			break;
		}
	}
}

BOOL SlicFrame::SetArrayValue(SlicSymbolData *sym,
							  SS_TYPE insType, SlicStackValue insValue,
							  SS_TYPE indexType, SlicStackValue indexValue)
{
	sint32 index = Eval(indexType, indexValue);
	return sym->SetArrayValue(index, insType, insValue);
}

void SlicFrame::AddArg(SS_TYPE type, SlicStackValue value)
{
	Assert(m_argList);
	if(!m_argList)
		return;

	SlicSymbolData *sym;
	switch(type) {
		case SS_TYPE_INT:
			m_argList->AddArg(SA_TYPE_INT, value.m_int);
			break;
		case SS_TYPE_VAR:
			sym = g_slicEngine->GetSymbol(value.m_int);
			if(sym->GetType() == SLIC_SYM_REGION) {
				m_argList->AddArg(SA_TYPE_REGION, sym);
			} else if(sym->GetType() == SLIC_SYM_COMPLEX_REGION) {
				m_argList->AddArg(SA_TYPE_COMPLEX_REGION, sym);
			} else {
				m_argList->AddArg(SA_TYPE_INT_VAR, sym);
			}
			break;
		case SS_TYPE_SYM:
			sym = value.m_sym;
			Assert(sym);
			if(sym) {
				if(sym->GetType() == SLIC_SYM_REGION) {
					m_argList->AddArg(SA_TYPE_REGION, sym);
				} else if(sym->GetType() == SLIC_SYM_COMPLEX_REGION) {
					m_argList->AddArg(SA_TYPE_COMPLEX_REGION, sym);
				} else {
					m_argList->AddArg(SA_TYPE_INT_VAR, sym);
				}
			}
			break;

		default:
			Assert(FALSE);
			break;
	}
}

//----------------------------------------------------------------------------
//
// Name       : DoInstruction
//
// Description: Executes slic intructions that were previously
//              "compiled" by slicif_add_op.
//
// Parameters : SOP op
//
// Globals    : This function is so big no idea.
//
// Returns    : -
//
// Remark(s)  : The behaviour depends on the given SOP type
//              if you define new operatiors you have of course modify
//              or implement the according behaviour of that function.
//
//              This function is called at slic run time.
//
//----------------------------------------------------------------------------
BOOL SlicFrame::DoInstruction(SOP op)
{
	unsigned char* codePtr = &m_segment->m_code[m_offset];
	unsigned char* origCodePtr = &m_segment->m_code[m_offset];
#if defined(SLIC_DOUBLES)
	double dval;
#endif

	sint32 ival, ival2;

	SlicStackValue sval1, sval2, sval3;

	SS_TYPE type1, type2, type3;
	SlicSymbolData *symval;
	BOOL stopped = FALSE;
	sint32 sp;
	sint32 res;
	BOOL calcOffset = TRUE;

	//Added by Martin G�hmann for database access
	SlicDBInterface *conduit;
	char* name;

	switch(op) {
		case SOP_PUSHI:
			sval1.m_int = *(reinterpret_cast<sint32 *>(codePtr));
			codePtr += sizeof(sint32);
			m_stack->Push(SS_TYPE_INT, sval1);
			break;

		case SOP_PUSHD:
#if defined(SLIC_DOUBLES)
			dval = *((double*)codePtr);
            // Probably some handling missing here. Now, we just skip some bytes.
#endif
			codePtr += sizeof(sint32);
			break;

		case SOP_PUSHV:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(int);
			sval1.m_sym = g_slicEngine->GetSymbol(ival);
			if (sval1.m_sym)
            {
                m_stack->Push(SS_TYPE_SYM, sval1);
            }
            else
            {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL symbol %d\n", ival));
				stopped = TRUE;
			}
			break;

		case SOP_PUSHM:
		{
			ival = *((sint32*)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			if(!symval) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL struct symbol %d\n", ival));
				stopped = TRUE;
				break;
			}

			if(symval->GetType() != SLIC_SYM_STRUCT) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, %s is not a struct\n", symval->GetName()));
				stopped = TRUE;
				break;
			}

			SlicStructInstance *theStruct = symval->GetStruct();
			Assert(theStruct);

			ival2 = *((sint32*)codePtr);
			codePtr += sizeof(sint32);

			sval1.m_sym = theStruct->GetMemberSymbol(ival2);
			m_stack->Push(SS_TYPE_SYM, sval1);
			break;
		}

		case SOP_PUSHAM:
		{

			ival = *((sint32*)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			if(!symval) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL struct symbol %d\n", ival));
				stopped = TRUE;
				break;
			}

			if(symval->GetType() != SLIC_SYM_ARRAY) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, %s is not an array\n", symval->GetName()));
				stopped = TRUE;
				break;
			}

			sp = m_stack->Pop(type3, sval3);
			Assert(sp >= 1);

			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);

			if(!symval->ArrayLookup(Eval(type3, sval3), type1, sval1)) {
				DPRINTF(k_DBG_SLIC, ("Couldn't perform array lookup in SOP_PUSHAM\n"));
				stopped = TRUE;
				break;
			}

			SlicSymbolData * structSym = SlicStack::GetSymbol(type1, sval1);
			if (!structSym)
            {
				DPRINTF(k_DBG_SLIC, ("Couldn't find struct symbol in SOP_PUSHAM\n"));
				stopped = TRUE;
				break;
			}

			if(structSym->GetType() != SLIC_SYM_STRUCT) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, array symbol id not a struct in SOP_PUSHAM\n"));
				stopped = TRUE;
				break;
			}

			SlicStructInstance *theStruct = structSym->GetStruct();
			Assert(theStruct);

			ival2 = *((sint32*)codePtr);
			codePtr += sizeof(sint32);

			sval1.m_sym = theStruct->GetMemberSymbol(ival2);
			m_stack->Push(SS_TYPE_SYM, sval1);
			break;
		}

		case SOP_PUSHA:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(int);
			symval = g_slicEngine->GetSymbol(ival);
			if(!symval) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL symbol %d\n", ival));
				stopped = TRUE;
				break;
			}

			sval1.m_sym = symval;
			m_stack->Push(SS_TYPE_SYM, sval1);
			break;

		case SOP_AINDX:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 1);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			res = ArrayLookup(type2, sval2, type1, sval1,
							  type3, sval3);
			if(res)

				m_stack->Push(type3, sval3);
			else {
				sval3.m_int = 0;

				m_stack->Push(SS_TYPE_INT, sval3);
			}
			break;
		case SOP_ADD:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) + Eval(type1, sval1);

			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_SUB:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) - Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_MULT:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) * Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_EXP:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = static_cast<int>
				(pow(static_cast<double>(Eval(type2, sval2)), Eval(type1, sval1)));
			m_stack->Push(SS_TYPE_INT, sval3);
			break;
		// Bitwise operators:
		case SOP_BAND:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) & Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);
			break;
		case SOP_BOR:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) | Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);
			break;
		case SOP_BXOR:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) ^ Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);
			break;
		case SOP_BNOT:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sval3.m_int = ~Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);
			break;
		case SOP_DIV:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			if(Eval(type1, sval1) == 0) {

				//Added by Martin G�hmann
				//It is a problem of slic code and not of the ctp2.exe,
				//the slicer has to solve the problem.
				if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
					c3errors_ErrorDialog("Slic", "In object %s: Division by 0.", m_segment->GetName());
				}
				sval3.m_int = 0;
				m_stack->Push(SS_TYPE_INT, sval3);
				break;
			}
			sval3.m_int = Eval(type2, sval2) / Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_MOD:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
            {
                sint32 const    divisor = Eval(type1, sval1);
                if (0 == divisor)
                {
                    // Handle error as for SOP_DIV.
                    if (g_theProfileDB && g_theProfileDB->IsDebugSlic())
                    {
                        c3errors_ErrorDialog("Slic", "In object %s: modulo 0.",
                                             m_segment->GetName()
                                            );
                    }
                    sval3.m_int = 0;
                }
                else
                {
                    sval3.m_int = Eval(type2, sval2) % divisor;
                }
            }
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_EQ:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = IsEqual(type2, sval2, type1, sval1);

			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_GT:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) > Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_LT:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) < Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_GTE:

			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) >= Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_LTE:

			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) <= Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_NEQ:

			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = !IsEqual(type2, sval2, type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_AND:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) && Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_OR:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);
			sval3.m_int = Eval(type2, sval2) || Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);

			break;
		case SOP_NOT:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sval3.m_int = !Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);
			break;
		case SOP_POP:

			Assert(FALSE);
			break;
		case SOP_TRIG:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sval3.m_int = Eval(type1, sval1);
			if(!sval3.m_int)
				stopped = TRUE;
			break;
		case SOP_ARGE:
			sp = m_stack->Pop(type1, sval1);
			AddArg(type1, sval1);
			break;
		case SOP_ARGID:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			if(!symval) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, NULL symbol %d\n", ival));
				return FALSE;
			}
			if(symval->GetType() != SLIC_SYM_ID) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, symbol %s is not of type ID\n",
						symval->GetName()));
				return FALSE;
			}
			m_argList->AddArg(symval->GetSegment(), symval);
			break;
		case SOP_ARGS:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			if(symval->GetType() != SLIC_SYM_SVAR) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, string arg doesn't have string type\n"));
				return FALSE;
			}
			m_argList->AddArg(SA_TYPE_STRING, sint32(symval->GetStringId()));
			break;
		case SOP_ARGST:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			if(symval->GetType() != SLIC_SYM_STRING) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, hard string doesn't have hard string type\n"));
				return FALSE;
			}
			m_argList->AddArg(SA_TYPE_HARD_STRING, symval);
			break;
		case SOP_CALL:
		case SOP_CALLR:
		{

			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			if(!symval) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, NULL symbol %d\n", ival));
				return FALSE;
			}
			if(symval->GetType() != SLIC_SYM_FUNC && symval->GetType() != SLIC_SYM_UFUNC) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, symbol %s is not a function\n",
						symval->GetName()));
				return FALSE;
			}
			SlicFunc *funcObj = symval->GetFunction();
			SFN_ERROR err;

			if(!funcObj) {

				SlicSegment *segment = g_slicEngine->GetSegment(symval->GetName());
				if(!segment || segment->GetType() != SLIC_OBJECT_FUNCTION) {
					DPRINTF(k_DBG_SLIC, ("Undefined function %s\n", symval->GetName()));
					if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
						c3errors_ErrorDialog("Slic", "%s is not a known function", symval->GetName());
					}
					return FALSE;
				}

				SlicObject * obj = NULL;
				err = segment->Call(m_argList, obj);
				if (obj)
                {
                    if (op == SOP_CALLR)
                    {
					    if (g_slicEngine->AtBreak())
                        {
                            Assert(!m_resultObject); // need stack when failing?
                            if (m_resultObject)
                            {
                                m_resultObject->Release();
                            }
						    m_resultObject  = obj;
					    }
                        else
                        {
						    sval1.m_int = obj->GetResult();
						    m_stack->Push(SS_TYPE_INT, sval1);
						    obj->Release();
					    }
                    }
                    else
                    {
                        // SOP_CALL: result not used
                        obj->Release();
                    }
				}
			} else {
				err = funcObj->Call(m_argList);
				if(op == SOP_CALLR) {
					switch(funcObj->GetReturnType()) {
						case SFR_INT:
							sval1.m_int = funcObj->GetResult().m_int;
							m_stack->Push(SS_TYPE_INT, sval1);
							break;
						default:
							sval1.m_int = 0;
							m_stack->Push(SS_TYPE_INT, sval1);
							Assert(FALSE);
					}
				}
			}
			if(err != SFN_ERROR_OK) {
				ReportSFError(err, symval);
				stopped = TRUE;
			}

			m_argStackPtr--;
			Assert(m_argStackPtr >= -1);
			if(m_argStackPtr >= 0) {
				m_argList = &m_argListArray[m_argStackPtr];
			} else if(m_argStackPtr == -1) {
				m_argList = NULL;
			}

			if(g_slicEngine->AtBreak())
				stopped = TRUE;

			break;
		}
		case SOP_EVENT:
		{
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			if(g_gevManager->IsProcessing()) {
				EVENTLOG(("    "));
			}

			EVENTLOG(("Event:%s(", g_gevManager->GetEventName((GAME_EVENT)ival)));

			GameEventArgList *args = m_argList->CreateGameEventArgs((GAME_EVENT)ival);
			EVENTLOG((") : Serial %d\n", g_gevManager->GetNextSerial()));

			g_gevManager->ArglistAddEvent(GEV_INSERT_Tail,
										  (GAME_EVENT)ival,
										  args);

			m_argStackPtr--;
			Assert(m_argStackPtr >= -1);
			if(m_argStackPtr >= 0) {
				m_argList = &m_argListArray[m_argStackPtr];
			} else if(m_argStackPtr == -1) {
				m_argList = NULL;
			}

			if(g_slicEngine->AtBreak())
				stopped = TRUE;

			break;
		}
		case SOP_SBLK:
			DPRINTF(k_DBG_SLIC, ("dangling SBLK\n"));
			return FALSE;
		case SOP_END:
			stopped = TRUE;
			codePtr += sizeof(sint32);
			break;
		case SOP_JMP:
			m_offset    = *(reinterpret_cast<sint32 *>(codePtr));
			calcOffset  = FALSE;
			break;
		case SOP_BNT:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			sp = m_stack->Pop(type2, sval2);
			if(!Eval(type2, sval2)) {
				m_offset = ival;
				calcOffset = FALSE;
			}
			break;
		case SOP_BNEV:


			codePtr += sizeof(sint32);

			break;
		case SOP_BUTN:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			ival2 = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);

			symval = g_slicEngine->GetSymbol(ival2);
			if(symval->GetType() != SLIC_SYM_SVAR) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, button string arg doesn't have string type\n"));
				return FALSE;
			}
			g_slicEngine->GetContext()->AddButton(new SlicButton(
				(StringId)symval->GetStringId(), m_segment,
				ival, g_slicEngine->GetContext()));
			break;
		case SOP_OCLS:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);

			g_slicEngine->GetContext()->AddButton(new SlicButton(
					 		 -1, m_segment, ival,
							 g_slicEngine->GetContext()));
			break;
		case SOP_STOP:
			stopped = TRUE;
			break;
		case SOP_NEG:
			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			sval3.m_int = 0 - Eval(type1, sval1);
			m_stack->Push(SS_TYPE_INT, sval3);
			break;
		case SOP_ASSN:
			ival = *((sint32*)codePtr);
			codePtr += sizeof(sint32);

			symval = g_slicEngine->GetSymbol(ival);
			if(!symval) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL symbol %d\n", ival));
				stopped = TRUE;
				break;
			}

			sp = m_stack->Pop(type1, sval1);

			SetValue(symval, type1, sval1);
			break;
		case SOP_ASSNA:

			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			if(!symval) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL symbol %d in array assignment", ival));
				stopped = TRUE;
				break;
			}

			if(symval->GetType() != SLIC_SYM_ARRAY) {
				if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
					c3errors_ErrorDialog("Slic", "Symbol '%s' used in array assignment is not an array", symval->GetName());
				}
				stopped = TRUE;
				break;
			}

			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 1);

			sp = m_stack->Pop(type3, sval3);
			Assert(sp >= 0);

			SetArrayValue(symval, type2, sval2, type3, sval3);
			break;
		case SOP_ASSNM:
		{
			ival = *((sint32*)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			if(!symval) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL struct symbol %d\n", ival));
				stopped = TRUE;
				break;
			}

			if(symval->GetType() != SLIC_SYM_STRUCT) {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, %s is not a struct\n", symval->GetName()));
				stopped = TRUE;
				break;
			}

			SlicStructInstance *theStruct = symval->GetStruct();
			Assert(theStruct);

			ival2 = *((sint32*)codePtr);
			codePtr += sizeof(sint32);

			symval = theStruct->GetMemberSymbol(ival2);

			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			SetValue(symval, type1, sval1);
			break;
		}
		case SOP_ASSNAM:
		{


			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			SlicSymbolData *arraySym = g_slicEngine->GetSymbol(ival);

			ival2 = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);

			if(!arraySym) {
				DPRINTF(k_DBG_SLIC, ("Bad Mojo, symbol %d does not exist", ival));
				stopped = TRUE;
				break;
			}

			if(arraySym->GetType() != SLIC_SYM_ARRAY) {
				DPRINTF(k_DBG_SLIC, ("%s is not an array", arraySym->GetName()));
				stopped = TRUE;
				break;
			}

			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 1);

			sp = m_stack->Pop(type2, sval2);
			Assert(sp >= 0);

			if(!arraySym->ArrayLookup(Eval(type2, sval2), type3, sval3)) {
				DPRINTF(k_DBG_SLIC, ("Array lookup, %s[%d], failed\n",
									 arraySym->GetName(), Eval(type2, sval2)));
				stopped = TRUE;
				break;
			}

			SlicSymbolData *structSym = SlicStack::GetSymbol(type3, sval3);
			if (!structSym)
			{
				DPRINTF(k_DBG_SLIC, ("Couldn't find symbol for %s[%d]\n",
									 arraySym->GetName(), Eval(type2, sval2)));
				stopped = TRUE;
				break;
			}

			if(structSym->GetType() != SLIC_SYM_STRUCT) {
				DPRINTF(k_DBG_SLIC, ("%s[%d] is not a struct\n",
									 arraySym->GetName(), Eval(type2, sval2)));
				stopped = TRUE;
				break;
			}

			SlicStructInstance *theStruct = structSym->GetStruct();
			Assert(theStruct);

			if(!theStruct) {
				stopped = TRUE;
				break;
			}
			SlicSymbolData *assnSym = theStruct->GetMemberSymbol(ival2);
			assnSym->SetValueFromStackValue(type1, sval1);
			break;
		}
		case SOP_SARGS:

			m_argStackPtr++;
			Assert(m_argStackPtr < k_ARGLIST_STACK_SIZE);
			if(m_argStackPtr < k_ARGLIST_STACK_SIZE) {
				m_argList = &m_argListArray[m_argStackPtr];
				m_argList->Clear();
			}
			break;
		case SOP_RET:

			sp = m_stack->Pop(type1, sval1);
			Assert(sp >= 0);
			g_slicEngine->GetContext()->SetResult(Eval(type1, sval1));
			stopped = TRUE;
			break;
		case SOP_LINE:
		case SOP_LBRK:
		{

			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			m_currentLine = ival;

			ival2 = *((sint32 *)codePtr);
			if(m_segment->GetFilename()) {
				if(ival2 < 0) {

					*((sint32 *)codePtr) = FindFileOffset(m_segment->GetFilename(), ival);
					ival2 = *((sint32 *)codePtr);
				}
			}

			codePtr += sizeof(sint32);

			SlicConditional *cond = *((SlicConditional **)codePtr);
			codePtr += sizeof(SlicConditional *);

			if(op == SOP_LBRK || g_slicEngine->BreakRequested()) {
				if(!cond || (cond->Eval() != 0)) {

					g_slicEngine->Break(m_segment, codePtr - m_segment->m_code,
										g_slicEngine->GetContext(), m_stack,
										m_messageData);
					stopped = TRUE;
				}
			}

			break;
		}
		case SOP_ASIZE:
			ival = *((sint32 *)codePtr);
			codePtr += sizeof(sint32);
			symval = g_slicEngine->GetSymbol(ival);
			Assert(symval);
			if(symval) {
				sval1.m_int = symval->GetArray()->GetSize();
			}
			else{
				sval1.m_int = 0;
			}
			m_stack->Push(SS_TYPE_INT, sval1);
			break;

		//Added by Martin G�hmann for database support
		case SOP_DBNAME:
		{
			conduit     = GetDatabase(codePtr);
			ival        = *((sint32 *)codePtr);
			codePtr    += sizeof(sint32);
			symval      = g_slicEngine->GetSymbol(ival);

			if (conduit && symval)
            {
			    sval1.m_sym = symval;
			    sval3.m_int = Eval(SS_TYPE_SYM, sval1);

			    if ((sval3.m_int >= 0) && sval3.m_int < conduit->GetNumRecords())
                {
                    // No action: value is OK
			    }
			    else
                {
				    sval3.m_int = -1;
                }

                m_stack->Push(SS_TYPE_INT, sval3);
			}
            else
            {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL symbol %d\n", ival));
				stopped = TRUE;
			}
			break;
		}
		case SOP_DBNAMEREF:
		{
			conduit     = GetDatabase(codePtr);
			ival        = *((sint32 *)codePtr);
			codePtr    += sizeof(sint32);
			symval      = g_slicEngine->GetSymbol(ival);

			if (conduit && symval)
            {
                //Get the member:
                name = reinterpret_cast<char *>(codePtr);
                Assert(name);
                for ( ; *codePtr++; )
                {
	                // No action: just skip
                }

                sval1.m_sym = symval;
                sval3.m_int = Eval(SS_TYPE_SYM, sval1);

                if(sval3.m_int > -1 && sval3.m_int < conduit->GetNumRecords()){
	                sval3.m_int = conduit->GetValue(sval3.m_int, name);
	                m_stack->Push(SS_TYPE_INT, sval3);
                }
                else{
	                if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
		                c3errors_ErrorDialog("Slic", "In object %s no entry found with index %i in %s.", m_segment->GetName(), sval3.m_int, conduit->GetName());
	                }
	                sval3.m_int = 0;
	                m_stack->Push(SS_TYPE_INT, sval3);
                }
            }
            else
            {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL symbol %d\n", ival));
				stopped = TRUE;
            }
			break;
		}
		case SOP_DBNAMEARRAY:
		{
			conduit     = GetDatabase(codePtr);
			ival        = *((int*)codePtr);
			codePtr    += sizeof(int);
			symval      = g_slicEngine->GetSymbol(ival);

			if (conduit && symval)
            {
			    //Get the member:
			    name    = reinterpret_cast<char *>(codePtr);
			    Assert(name);
			    for ( ; *codePtr++ ; )
                {
				    // No action: just skip
			    }

			    sval1.m_sym = symval;
			    sval3.m_int = Eval(SS_TYPE_SYM, sval1);

			    sp = m_stack->Pop(type2, sval2);
			    Assert(sp >= 0);

			    sval2.m_int = Eval(type2, sval2);

			    if(sval3.m_int > -1 && sval3.m_int < conduit->GetNumRecords()){
				    sval3.m_int = conduit->GetValue(sval3.m_int, name, sval2.m_int);
				    m_stack->Push(SS_TYPE_INT, sval3);
			    }
			    else{
				    if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
					    c3errors_ErrorDialog("Slic", "In object %s no entry found with index %i in %s.", m_segment->GetName(), sval3.m_int, conduit->GetName());
				    }
				    sval3.m_int = 0;
				    m_stack->Push(SS_TYPE_INT, sval3);
			    }
            }
            else
            {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, NULL symbol %d\n", ival));
				stopped = TRUE;
			}

			break;
		}
		case SOP_DBNAMECONSTARRAY:
		{
			conduit     = GetDatabase(codePtr);

            if (conduit)
            {
			    sval3.m_int = *((sint32 *)codePtr);
			    codePtr    += sizeof(sint32);
			    //Get the member:
			    name = reinterpret_cast<char *>(codePtr);
			    Assert(name);
			    for ( ; *codePtr++; )
                {
				    // No action: just skip
			    }

			    sp = m_stack->Pop(type2, sval2);
			    Assert(sp >= 0);

			    sval2.m_int = Eval(type2, sval2);

			    if(sval3.m_int > -1 && sval3.m_int < conduit->GetNumRecords()){
				    sval3.m_int = conduit->GetValue(sval3.m_int, name, sval2.m_int);
				    m_stack->Push(SS_TYPE_INT, sval3);
			    }
			    else{
				    if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
					    c3errors_ErrorDialog("Slic", "In object %s no entry found with index %i in %s.", m_segment->GetName(), sval3.m_int, conduit->GetName());
				    }
				    sval3.m_int = 0;
				    m_stack->Push(SS_TYPE_INT, sval3);
			    }
            }
            else
            {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, incorrect database symbol\n"));
				stopped = TRUE;
            }

			break;
		}
		case SOP_DB:
		{
			conduit = GetDatabase(codePtr);

			if (conduit)
            {
			    sp = m_stack->Pop(type1, sval1);
			    Assert(sp >= 0);

			    sval3.m_int = Eval(type1, sval1);

			    if(sval3.m_int > -1 && sval3.m_int < conduit->GetNumRecords()){
				    m_stack->Push(SS_TYPE_INT, sval3);
			    }
			    else{
				    sval3.m_int = -1;
				    m_stack->Push(SS_TYPE_INT, sval3);
			    }
            }
            else
            {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, incorrect database symbol\n"));
				stopped = TRUE;
            }

			break;
		}
		case SOP_DBREF:
		{
			conduit = GetDatabase(codePtr);

			if (conduit)
            {
			    //Get the member:
			    name = reinterpret_cast<char *>(codePtr);
			    Assert(name);
			    for ( ; *codePtr++ ; )
                {
				    // No action: just skip
			    }

			    sp = m_stack->Pop(type1, sval1);
			    Assert(sp >= 0);

			    sval2.m_int = Eval(type1, sval1);

			    if(sval2.m_int > -1 && sval2.m_int < conduit->GetNumRecords()){
				    sval3.m_int = conduit->GetValue(sval2.m_int, name);
				    m_stack->Push(SS_TYPE_INT, sval3);
			    }
			    else{
				    if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
					    c3errors_ErrorDialog("Slic", "In object %s no entry found with index %i in %s.", m_segment->GetName(), sval2.m_int, conduit->GetName());
				    }
				    sval2.m_int = 0;
				    m_stack->Push(SS_TYPE_INT, sval2);
			    }
            }
            else
            {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, incorrect database symbol\n"));
				stopped = TRUE;
            }

			break;
		}
		case SOP_DBARRAY:
		{
			conduit = GetDatabase(codePtr);

			if (conduit)
            {
			    //Get the member:
			    name = reinterpret_cast<char *>(codePtr);
			    Assert(name);
			    for ( ; *codePtr++; )
                {
				    // Just skip
			    }

			    sp = m_stack->Pop(type1, sval1);
			    Assert(sp >= 0);

			    sval1.m_int = Eval(type1, sval1);

			    sp = m_stack->Pop(type2, sval2);
			    Assert(sp >= 0);

			    sval2.m_int = Eval(type2, sval2);

			    if(sval2.m_int > -1 && sval2.m_int < conduit->GetNumRecords()){
				    sval3.m_int = conduit->GetValue(sval2.m_int, name, sval1.m_int);
				    m_stack->Push(SS_TYPE_INT, sval3);
			    }
			    else{
				    if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
					    c3errors_ErrorDialog("Slic", "In object %s no entry found with index %i in %s.", m_segment->GetName(), sval2.m_int, conduit->GetName());
				    }
				    sval2.m_int = 0;
				    m_stack->Push(SS_TYPE_INT, sval2);
			    }
            }
            else
            {
				DPRINTF(k_DBG_SLIC, ("Bad mojo, incorrect database symbol\n"));
				stopped = TRUE;
            }

			break;
		}
		case SOP_DBSIZE:
		{
			//Added by Martin G�hmann to figure out via
			//slic how many records the database contains
			//Get the database:
			conduit = GetDatabase(codePtr);
			Assert(conduit);
			if (conduit)
            {
				sval3.m_int = conduit->GetNumRecords();
			}
			else
            {
				sval3.m_int = 0;
			}
			m_stack->Push(SS_TYPE_INT, sval3);
			break;
		}

		default:
			DPRINTF(k_DBG_SLIC, ("???\n"));
			break;
	}
	if(calcOffset)
		m_offset += static_cast<sint32>(codePtr - origCodePtr); // m_offset should be size_t, unless it is stored in the savegame. And of course it is legacy code.
	return !stopped;
}

BOOL SlicFrame::Run()
{
	if (m_resultObject)
    {
		SlicStackValue value;
		value.m_int = m_resultObject->GetResult();
		m_resultObject->Release();
        m_resultObject = NULL;
		m_stack->Push(SS_TYPE_INT, value);
	}

	SOP op;

	do {
		op = (SOP)m_segment->m_code[m_offset++];
	} while(DoInstruction(op));

	return TRUE;
}

BOOL SlicFrame::RunAt(sint32 startOffset)
{
	m_offset = startOffset;
	return Run();
}

void SlicFrame::ClearMessageData()
{
	if(!m_messageData) {
		m_messageData = new MessageData(ID());
	}

	m_messageData->m_owner = PLAYER_INDEX_INVALID;
	m_messageData->m_sender = PLAYER_INDEX_INVALID;
	m_messageData->m_msgType = 0;
	m_messageData->m_cityList->Clear();
	delete [] m_messageData->m_text;
	m_messageData->m_text = NULL;
	m_messageData->m_request = ID();
	m_messageData->m_timestamp = g_turn ? g_turn->GetYear() : 0;
}

void SlicFrame::DeleteMessageData()
{
	delete m_messageData;
	m_messageData = NULL;
}

void SlicFrame::SetMessageData(MessageData *data)
{
	m_messageData = data;
}

extern char g_missingSegment[256];

void SlicFrame::ReportSFError(SFN_ERROR err, SlicSymbolData *sym)
{
	MBCHAR buf[1024];

	sprintf(buf, "In object %s, function %s: ",
			m_segment->GetName(), sym->GetFunction()->GetName());
	switch(err) {
		case SFN_ERROR_NUM_ARGS:
			strcat(buf, "Wrong number of arguments");
			break;
		case SFN_ERROR_TYPE_ARGS:
			strcat(buf, "Wrong type of argument");
			break;
		case SFN_ERROR_BAD_FUNCTION:
			strcat(buf, "Bad function, bad!");
			break;
		case SFN_ERROR_INTERNAL:
			strcat(buf, "Internal failure (not your fault, don't feel bad)");
			break;
		case SFN_ERROR_TYPE_BUILTIN:
			strcat(buf, "Wrong kind of builtin variable");
			break;
		case SFN_ERROR_NOT_ADVANCE:
			strcat(buf, "Argument is not an advance");
			break;
		case SFN_ERROR_NOT_UNIT_FLAG:
			strcat(buf, "Argument is not a unit flag");
			break;
		case SFN_ERROR_NOT_IN_BUTTON:
			strcat(buf, "This function can only be used inside buttons");
			break;
		case SFN_ERROR_NOT_DIP_MESSAGE:
			strcat(buf, "This function can only be used in diplomatic messages");
			break;
		case SFN_ERROR_NOT_MESSAGE_TYPE:
			strcat(buf, "Invalid message type");
			break;
		case SFN_ERROR_NOT_SEGMENT:
			sprintf(buf + strlen(buf), "There is no object named %s", g_missingSegment);
			break;
		case SFN_ERROR_NOT_TRADE_BID:
			strcat(buf, "Not in a trade bid message");
			break;
		case SFN_ERROR_NOT_SINGLE_RECIPIENT:
			strcat(buf, "Single recipient function called from multiple recipient message");
			break;
		case SFN_ERROR_NOT_ADJACENT:
			strcat(buf, "This function only works with adjacent locations");
			break;
		case SFN_ERROR_UNKNOWN_UNIT_TYPE:
			strcat(buf, "Unknown unit type");
			break;
		case SFN_ERROR_UNKNOWN_BUILDING:
			strcat(buf, "Unknown building");
			break;
		case SFN_ERROR_OUT_OF_RANGE:
			strcat(buf, "Value out of bounds");
			break;
		case SFN_ERROR_DEAD_PLAYER:
			strcat(buf, "Referenced player is not in the game");
			break;
		case SFN_ERROR_EFFECT_NOT_FOUND:
			strcat(buf, "The named special effect does not exist");
			break;
		case SFN_ERROR_SOUND_NOT_FOUND:
			strcat(buf, "The named sound does not exist");
			break;
		case SFN_ERROR_CIV_NOT_FOUND:
			strcat(buf, "The named civilization does not exist");
			break;

		default:
			strcat(buf, "Unknown error");
			break;
	}
	if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
		c3errors_ErrorDialog("Slic", buf);
	}
}

sint32 SlicFrame::FindFileOffset(const char *file, sint32 line)
{
	FILE *f = fopen(file, "r");
	if(!f)
		return -1;

	sint32 findLine = 1;
	char lineStr[1024];
	while(findLine < line) {
		fgets(lineStr, 1024, f);
		findLine++;
	}
	sint32 offset = ftell(f);
	fclose(f);
	return offset;
}
