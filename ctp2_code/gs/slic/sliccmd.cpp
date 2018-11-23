//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : SLIC command handling
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
// - Types corrected.
// - Modified int checker functions so that they return false if there is
//   also an alternative string representation. Nov. 5th 2004 Martin G�hmann
// - Fixed broken government message with white text. April 14th 2005 Martin G�hmann
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"

#include "sliccmd.h"
#include "SlicEngine.h"
#include "SlicSymbol.h"
#include "SlicNamedSymbol.h"
#include "SlicStruct.h"

#include "pointerlist.h"

#include "ysc.tab.h"
#include "StrDB.h"
#include "SlicDBConduit.h"
#include "profileDB.h"


PointerList<SlicSymbolData> *s_symbolList = NULL;

int sliccmd_int_result;

void sliccmd_arg_exp(int value)
{
	yyscerror("arguments not implemented");
}

void sliccmd_arg_id(const char *id)
{
	yyscerror("arguments not implemented");
}

void sliccmd_arg_stringid(int id)
{
	yyscerror("arguments not implemented");
}

void sliccmd_arg_string(const char *string)
{
	yyscerror("arguments not implemented");
}

int sliccmd_get_int_value(const char *symName)
{
	char errbuf[1024];

	SlicSymbolData *sym = sliccmd_get_symbol(symName);
	if(!sym) {
		sprintf(errbuf, "Symbol %s not found", symName);
		yyscerror(errbuf);
		return 0;
	}

	sint32 value;
	if(!sym->GetIntValue(value)) {
		sprintf(errbuf, "Symbol %s is not an integer variable", symName);
		yyscerror(errbuf);
		return 0;
	}

	return value;
}

int sliccmd_call(const char *funcName)
{
	yyscerror("Function calls not implemented");
	return 0;
}

int sliccmd_ref_has_int_value(const char *structName, const char *memberName)
{

	SlicSymbolData *sym = sliccmd_get_symbol(structName);
	if(!sym) {
		return 0;
	}

	if(sym->GetType() != SLIC_SYM_STRUCT) {
		return 0;
	}

	SlicStructInstance *inst = sym->GetStruct();

	Assert(inst);

	if(!inst)
		return 0;

	SlicSymbolData *member = inst->GetMemberSymbolByName(memberName);
	if(!member) {
		return 0;
	}

	sliccmd_add_symbol_used(member);

	sint32 value;
	// Added by Martin G�hmann
	if(!member->GetIntValue(value)
	||(sym->GetText(sliccmd_output, sliccmd_output_len)
	&& sym->GetType() != SLIC_SYM_IVAR)
	){
		return 0;
	}
	return 1;
}

int sliccmd_get_ref_value(const char *structName, const char *memberName)
{
	char errbuf[1024];

	SlicSymbolData *sym = sliccmd_get_symbol(structName);
	if(!sym) {
		sprintf(errbuf, "Symbol %s not found", structName);
		yyscerror(errbuf);
		return 0;
	}

	if(sym->GetType() != SLIC_SYM_STRUCT) {
		sprintf(errbuf, "%s is not a structure", structName);
		yyscerror(errbuf);
		return 0;
	}

	SlicStructInstance *inst = sym->GetStruct();

	Assert(inst);

	if(!inst)
		return 0;

	SlicSymbolData *member = inst->GetMemberSymbolByName(memberName);
	if(!member) {
		sprintf(errbuf, "Struct %s has no member named %s", structName, memberName);
		yyscerror(errbuf);
		return 0;
	}

	sliccmd_add_symbol_used(member);

	sint32 value;
	if(!member->GetIntValue(value)) {
		sprintf(errbuf, "%s.%s is not an integer", structName, memberName);
		yyscerror(errbuf);
		return 0;
	}
	return value;
}

void *sliccmd_get_ref_sym(const char *structName, const char *memberName)
{
	char errbuf[1024];

	SlicSymbolData *sym = sliccmd_get_symbol(structName);
	if(!sym) {
		sprintf(errbuf, "Symbol %s not found", structName);
		yyscerror(errbuf);
		return 0;
	}

	if(sym->GetType() != SLIC_SYM_STRUCT) {
		sprintf(errbuf, "%s is not a structure", structName);
		yyscerror(errbuf);
		return 0;
	}

	SlicStructInstance *inst = sym->GetStruct();

	Assert(inst);

	if(!inst)
		return 0;

	SlicSymbolData *member = inst->GetMemberSymbolByName(memberName);
	if(!member) {
		sprintf(errbuf, "Struct %s has no member named %s", structName, memberName);
		yyscerror(errbuf);
		return 0;
	}

	sliccmd_add_symbol_used(member);

	return member;
}

int sliccmd_array_lookup(const char *arrayName, int index)
{
	char errbuf[1024];

	SlicSymbolData *sym = sliccmd_get_symbol(arrayName);
	if(!sym) {
		sprintf(errbuf, "Symbol %s not found", arrayName);
		yyscerror(errbuf);
		return 0;
	}

	if(sym->GetType() != SLIC_SYM_ARRAY) {
		sprintf(errbuf, "%s is not an array", arrayName);
		yyscerror(errbuf);
		return 0;
	}

	SS_TYPE type;
	SlicStackValue value;
	if(!sym->ArrayLookup(index, type, value)) {
		sprintf(errbuf, "Array lookup %s[%d] failed", arrayName, index);
		yyscerror(errbuf);
		return 0;
	}

	if(type == SS_TYPE_INT) {
		return value.m_int;
	} else if(type == SS_TYPE_SYM) {
		sint32 intVal;
		if(!value.m_sym->GetIntValue(intVal)) {
			sprintf(errbuf, "%s[%d] is not an integer", arrayName, index);
			yyscerror(errbuf);
			return 0;
		}
		return intVal;
	}

	Assert(FALSE);
	return 0;
}

void *sliccmd_array_lookup_reference(const char *arrayName, int index, const char *memberName)
{
	char errbuf[1024];

	SlicSymbolData *sym = sliccmd_get_symbol(arrayName);
	if(!sym) {
		sprintf(errbuf, "Symbol %s not found", arrayName);
		yyscerror(errbuf);
		return 0;
	}

	if(sym->GetType() != SLIC_SYM_ARRAY) {
		sprintf(errbuf, "%s is not an array", arrayName);
		yyscerror(errbuf);
		return 0;
	}

	SS_TYPE type;
	SlicStackValue value;
	if(!sym->ArrayLookup(index, type, value)) {
		sprintf(errbuf, "Array lookup %s[%d] failed", arrayName, index);
		yyscerror(errbuf);
		return 0;
	}

	if((type != SS_TYPE_SYM) || (value.m_sym->GetType() != SLIC_SYM_STRUCT)) {
		sprintf(errbuf, "%s[%d] is not a struct", arrayName, index);
		yyscerror(errbuf);
		return 0;
	}

	sliccmd_add_symbol_used(value.m_sym);

	SlicStructInstance *inst = value.m_sym->GetStruct();
	Assert(inst);
	if(!inst)
		return 0;

	SlicSymbolData *member = inst->GetMemberSymbolByName(memberName);
	if(!member) {
		sprintf(errbuf, "%s[%d] has no member named %s", arrayName, index, memberName);
		yyscerror(errbuf);
		return 0;
	}

	sliccmd_add_symbol_used(member);

	return member;
}

void sliccmd_error(const char *s)
{
	if(sliccmd_use_dialogs) {
		if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
			c3errors_ErrorDialog("SLIC Command", s);
		}
	} else {
		DPRINTF(k_DBG_SLIC, ("SLIC Command Error: %s\n", s));
	}
}

int sliccmd_has_int_value(const char *symName)
{
	SlicSymbolData *sym = sliccmd_get_symbol(symName);
	if(!sym)
		return 0;

	sint32 value;
	// Added by Martin G�hmann
	if( sym->GetIntValue(value)
	&&(!sym->GetText(sliccmd_output, sliccmd_output_len)
	||  sym->GetType() == SLIC_SYM_IVAR))
		return 1;
	else
		return 0;
}

int sliccmd_sym_has_int_value(const void *vsym, int *value)
{
	const SlicSymbolData *sym = (SlicSymbolData *)vsym;

	// Added by Martin G�hmann
	if( sym->GetIntValue((sint32 &)*value)
	&&(!sym->GetText(sliccmd_output, sliccmd_output_len)
	||  sym->GetType() == SLIC_SYM_IVAR))
		return 1;
	return 0;
}

int sliccmd_array_has_int_value(const char *symName, int index)
{
	SlicSymbolData *sym = sliccmd_get_symbol(symName);
	if(!sym)
		return 0;

	SS_TYPE type;
	SlicStackValue value;
	if(!sym->ArrayLookup(index, type, value)) {
		return 0;
	}

	if(type == SS_TYPE_INT)
		return 1;
	else if(type == SS_TYPE_SYM) {
		sint32 intval;
		return (int)value.m_sym->GetIntValue(intval);
	}

	Assert(FALSE);
	return 0;
}

void *sliccmd_lookup_sym(const char *name)
{
	char errbuf[1024];
	SlicSymbolData *sym = sliccmd_get_symbol(name);
	if(!sym) {
		sprintf(errbuf, "Symbol %s not found", name);
		yyscerror(errbuf);
		return sym;
	}
	return sym;
}

void *sliccmd_maybe_lookup_sym(const char *name)
{
	SlicSymbolData *sym = sliccmd_get_symbol(name);
	return sym;
}

void *sliccmd_array_lookup_sym(const char *arrayName, int index)
{
	char errbuf[1024];

	SlicSymbolData *sym = sliccmd_get_symbol(arrayName);
	if(!sym) {
		sprintf(errbuf, "Symbol %s not found", arrayName);
		yyscerror(errbuf);
		return NULL;
	}

	if(sym->GetType() != SLIC_SYM_ARRAY) {
		sprintf(errbuf, "%s is not an array", arrayName);
		yyscerror(errbuf);
		return NULL;
	}

	SS_TYPE type;
	SlicStackValue value;
	if(!sym->ArrayLookup(index, type, value)) {
		sprintf(errbuf, "Array lookup %s[%d] failed", arrayName, index);
		yyscerror(errbuf);
		return NULL;
	}

	if(type == SS_TYPE_SYM) {
		return value.m_sym;
	} else {
		sprintf(errbuf, "No symbol for %s[%d]", arrayName, index);
		yyscerror(errbuf);
		return NULL;
	}
}

void sliccmd_parse_done(struct sliccmdExpValue *v, int action)
{
	if(v->type == EXP_VAL_INT) {
		sliccmd_int_result = v->value;
		sprintf(sliccmd_output, "%d", v->value);
		return;
	}

	Assert(v->type == EXP_VAL_SYM || v->type == EXP_VAL_TEMP_SYM);
	SlicSymbolData *sym = (SlicSymbolData *)v->sym;

	Assert(sym);
	if(!sym) {
		strcpy(sliccmd_output, "<Err>");
		return;
	}

	if(action == SLICCMD_REPLACE) {
		bool expanded = false;
		if(sliccmd_cat_string[0] != 0) {
			StringId id = sym->GetStringId();
			if(id >= 0) {
				char buf[k_MAX_MSG_LEN];
				sprintf(buf, "%s_%s", g_theStringDB->GetIdStr(id), sliccmd_cat_string);
				MBCHAR *rval;
				if(g_theStringDB->GetText(buf, &rval)) {
					strcpy(sliccmd_output, rval);
					expanded = true;
				}
			}
		}
		if(!expanded) {
			sym->GetText(sliccmd_output, sliccmd_output_len);
		}
	} else {
		sym->GetDebugText(sliccmd_output, sliccmd_output_len);
	}
	sym->GetIntValue((sint32 &)sliccmd_int_result);

	if(v->type == EXP_VAL_TEMP_SYM) {
		delete sym;
	}
}

void sliccmd_assign(void *vsym, struct sliccmdExpValue *v)
{
	SlicSymbolData *sym = (SlicSymbolData *)vsym;
	if(!sym)
		return;

	SS_TYPE type;
	SlicStackValue sval;

	if(v->type == EXP_VAL_INT) {
		type = SS_TYPE_INT;
		sval.m_int = v->value;
	} else {
		type = SS_TYPE_SYM;
		sval.m_sym = (SlicSymbolData *)v->sym;
	}

	sym->SetValueFromStackValue(type, sval);
}

void sliccmd_type_error()
{
	char errbuf[1024];
	sprintf(errbuf, "Type error");
	yyscerror(errbuf);
}

void sliccmd_clear_symbols()
{
	if (s_symbolList)
    {
    	while (s_symbolList->GetHead())
        {
		    s_symbolList->RemoveHead();
        }

        delete s_symbolList;
        s_symbolList = NULL;
	}
}

void sliccmd_add_symbol_used(SlicSymbolData *sym)
{
	if(!s_symbolList)
		s_symbolList = new PointerList<SlicSymbolData>;

	PointerList<SlicSymbolData>::Walker walk(s_symbolList);
	while(walk.IsValid()) {

		if(walk.GetObj() == sym)
			return;
		walk.Next();
	}

	s_symbolList->AddTail(sym);
}

void sliccmd_add_watch(SlicSymbolWatchCallback *watch)
{
	if(!s_symbolList)
		s_symbolList = new PointerList<SlicSymbolData>;

	PointerList<SlicSymbolData>::Walker walk(s_symbolList);
	while(walk.IsValid()) {
		walk.GetObj()->AddWatch(watch);
		watch->WatchCallback(walk.GetObj(), true);
		walk.Next();
	}
}


SlicSymbolData *sliccmd_get_symbol(const char *name)
{
	SlicSymbolData *sym = g_slicEngine->GetSymbol(name);
	if(sym) {
		sliccmd_add_symbol_used(sym);
	}
	return sym;
}

void *sliccmd_get_db_name_sym(void *dbptr, const char *name)
{
	SlicDBInterface * conduit = reinterpret_cast<SlicDBInterface *>(dbptr);
	Assert(conduit);
	if(!conduit)
		return 0;

	sint32 stringId = conduit->GetRecordNameID(name);
	if(stringId >= 0) {
		SlicSymbolData *sym = new SlicSymbolData(SLIC_SYM_SVAR);
		sym->SetStringId(stringId);
		return sym;
	} else {

		SlicSymbolData *sym = new SlicSymbolData(SLIC_SYM_STRING);

		const MBCHAR *str = conduit->GetRecordName(name);
		sym->SetString((const char *)str);
		return sym;
	}
}

void *sliccmd_get_db_name_sym_by_index(void *dbptr, int index)
{
	SlicDBInterface * conduit = reinterpret_cast<SlicDBInterface *>(dbptr);
	Assert(conduit);
	if(!conduit)
		return 0;

	sint32 stringId = conduit->GetRecordNameIDByIndex(index);
	if(stringId >= 0) {
		SlicSymbolData *sym = new SlicSymbolData(SLIC_SYM_SVAR);
		sym->SetStringId(stringId);
		return sym;
	} else {
		SlicSymbolData *sym = new SlicSymbolData(SLIC_SYM_STRING);
		const MBCHAR *str = conduit->GetRecordNameByIndex((sint32)index);
		sym->SetString((const char *)str);
		return sym;
	}
}
