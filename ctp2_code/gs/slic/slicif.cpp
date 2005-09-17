//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : SLIC interpreter functions
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
// _DEBUG
// - Generate debug version
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added slic database access by Martin Gühmann
// - Added a way to find out the size of a slic database by Martin Gühmann
// - slicif_cleanup() added.
// - Fixed slic database access after a reload by Martin Gühmann.
// - Types corrected.
// - Added debugging code for '**' operator
// - Replaced slicif_is_sym by slicif_is_name function. This function is 
//   modelled slicif_find_db_index but without error message if this 
//   function fails to retrieve the database index. - Feb. 24th 2005 Martin Gühmann
// - Added debugging code for bitwise operator
// - Prevented crash with invalid Slic input.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added database array access. (Sep 16th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "slicif.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "SlicEngine.h"
#include "SlicSymbol.h"
#include "GameEventManager.h"
#include "SlicStruct.h"
#include "SlicNamedSymbol.h"
#include "SlicArray.h"
#include "profileDB.h"

#include "SlicDBConduit.h"

#define k_MAX_PAREN_LEVEL 128

struct PSlicObject **g_slicObjectArray = NULL;
int g_slicArraySize = 0;
int g_slicNumEntries = 0;

namespace
{
    char const NAME_STRUCT_INVALID[]    = "*invalid struct*";
} // namespace

static unsigned char * s_code = NULL;
static int s_allocated_code = 0;
static unsigned char *s_code_ptr;
static int s_trigger_symbols[k_MAX_TRIGGER_SYMBOLS];
static int s_num_trigger_symbols;
static int s_parameters[k_MAX_PARAMETERS];
static int s_num_parameters;
static int s_parameter_index = 0;
static int s_temp_name_counter = 0;

static unsigned char *s_block_ptr[k_MAX_SLIC_LEVELS];
static int s_level = 0;
static int s_found_trigger;

static int s_file_num = 0;

static int s_if_level = 0;
struct {
	int count;
	unsigned char *array[k_MAX_SLIC_LEVELS];
} s_if_stack[k_MAX_SLIC_LEVELS];

struct LoopStackDescriptor {
	int expression;
	int increment;
};

static int s_while_level = 0;
static LoopStackDescriptor s_while_stack[k_MAX_SLIC_LEVELS];

static char s_current_segment_name[1024];
static SF_RET s_function_return_type = (SF_RET)0;

int g_slicWarnUndeclared = 0;

static int s_event_checking = 0;

static int s_arg_counts[GEA_End];

static int s_inSegment = 0;

static int s_parenLevel = 0;

static GAME_EVENT s_currentEvent;
static sint32 s_currentEventArgument[k_MAX_PAREN_LEVEL];

static bool s_argValuePushed = false;
static SlicSymbolData *s_argSymbol;
static int s_argMemberIndex;

#define k_MAX_FUNCTION_NAME 256

extern "C" void yyerror(char *s);

void slicif_init()
{
	int i;

	if(g_slicObjectArray) {
		for(i = 0; i < g_slicNumEntries; i++) {
			if(g_slicObjectArray[i]) {
				if(g_slicObjectArray[i]->m_id) {
					free(g_slicObjectArray[i]->m_id);
				}
				if(g_slicObjectArray[i]->m_trigger_symbols) {
					free(g_slicObjectArray[i]->m_trigger_symbols);
				}
				if(g_slicObjectArray[i]->m_parameters) {
					free(g_slicObjectArray[i]->m_parameters);
				}
				free(g_slicObjectArray[i]);
			}
		}
		free(g_slicObjectArray);
	}

	g_slicObjectArray = NULL;
	g_slicArraySize = 0;
	g_slicNumEntries = 0;
	s_temp_name_counter = 0;
}

//----------------------------------------------------------------------------
//
// Name       : slicif_cleanup
//
// Description: Clean up used (heap) memory 
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
void slicif_cleanup()
{
	slicif_init();
	delete [] s_code;
	s_code				= NULL;
	s_code_ptr			= NULL;
	s_allocated_code	= 0;
}

void slicif_start()
{
	s_level = 0;
	s_num_trigger_symbols = 0;
	s_found_trigger = 0;
	s_if_level = 0;
	s_while_level = 0;
	s_num_parameters = 0;
	s_parameter_index = 0;
	s_event_checking = 0;
	s_inSegment = 0;
	s_currentEvent = GEV_MAX;
	s_argValuePushed = false;
	s_argSymbol = NULL;
	s_argMemberIndex = -1;
	s_parenLevel = 0;
	if(!s_code) {
		s_code = new unsigned char[1000];
		s_allocated_code = 1000;
	}
	s_code_ptr = s_code;
}

void slicif_set_start(int symStart)
{
}

void slicif_add_object(struct PSlicObject *obj)
{
	if(!g_slicObjectArray) {
		g_slicObjectArray = (PSlicObject**)malloc(sizeof(struct PSlicObject *) * k_INITIAL_SLIC_SIZE);
		g_slicArraySize = k_INITIAL_SLIC_SIZE;
	}

	if(g_slicNumEntries >= g_slicArraySize) {
		struct PSlicObject **newArray = (PSlicObject **)malloc(sizeof(struct PSlicObject *) * g_slicArraySize * 2);
		memcpy(newArray, g_slicObjectArray, sizeof(struct PSlicObject *) * g_slicArraySize);
		free(g_slicObjectArray);
		g_slicArraySize *= 2;
		g_slicObjectArray = newArray;
	}
	slicif_add_op(SOP_STOP);
	obj->m_code = (unsigned char *)malloc(s_code_ptr - s_code);
	memcpy(obj->m_code, s_code, s_code_ptr - s_code);
	obj->m_codeSize = s_code_ptr - s_code;
	obj->m_from_file = s_file_num;
	if(g_theProfileDB->IsDebugSlic()) {
		const char *filename = slicif_get_filename();
		obj->m_filename = (char *)malloc(strlen(filename) + 1);
		strcpy(obj->m_filename, filename);
	} else {
		obj->m_filename = NULL;
	}

	g_slicObjectArray[g_slicNumEntries] = obj;
	g_slicNumEntries++;
	
	if(obj->m_type == SLIC_OBJECT_TRIGGER) {
		obj->m_trigger_symbols = (int *)malloc(s_num_trigger_symbols * sizeof(int));
		obj->m_num_trigger_symbols = s_num_trigger_symbols;
		memcpy(obj->m_trigger_symbols, s_trigger_symbols, s_num_trigger_symbols * sizeof(int));
	} else {
		obj->m_trigger_symbols = NULL;
		obj->m_num_trigger_symbols = 0;
		obj->m_ui_component = NULL;
	}

	if(obj->m_type == SLIC_OBJECT_FUNCTION) {
		SlicSymbolData *funcSym;

		obj->m_parameters = (int *)malloc(s_num_parameters * sizeof(int));
		obj->m_num_parameters = s_num_parameters;
		memcpy(obj->m_parameters, s_parameters, s_num_parameters * sizeof(int));
		obj->m_return_type = s_function_return_type;

		
		funcSym = g_slicEngine->GetOrMakeSymbol(obj->m_id);
		Assert(funcSym->GetType() == SLIC_SYM_FUNC || funcSym->GetType() == SLIC_SYM_UFUNC || funcSym->GetType() == SLIC_SYM_UNDEFINED);
		if(funcSym->GetType() == SLIC_SYM_UNDEFINED)
			funcSym->SetType(SLIC_SYM_UFUNC);

	} else {
		obj->m_parameters = NULL;
		obj->m_num_parameters = 0;
		obj->m_return_type = SF_RET_VOID;
	}

	if(obj->m_type != SLIC_OBJECT_HANDLEEVENT) {
		obj->m_event_name = NULL;
	} else {
		Assert(obj->m_priority == SLIC_PRI_PRE || obj->m_priority == SLIC_PRI_POST);
	}

#ifdef _DEBUG
	slicif_dump_code(s_code, s_code_ptr - s_code);
#endif

	slicif_start();
}

void slicif_declare_sym(char *name, SLIC_SYM type)
{
	char buf[1024];
	char realname[1024];

	SlicNamedSymbol *sym;

	if(s_inSegment) {
		slicif_get_local_name(realname, name);
		sym = g_slicEngine->GetOrMakeSymbol(realname);
	} else {
		sym = g_slicEngine->GetOrMakeSymbol(name);
	}

	if(sym) {
		if(sym->GetType() != SLIC_SYM_UNDEFINED) {
			sprintf(buf, "Symbol '%s' already has a type", name);			
			yyerror(buf);
		} else {
			SlicStructDescription *desc = g_slicEngine->GetStructDescription(type);
			if(desc) {
				sym->SetType(SLIC_SYM_STRUCT);
				sym->SetStruct(new SlicStructInstance(desc));
			} else {
				sym->SetType(type);
			}
		}
	} else {
		sprintf(buf, "Couldn't create symbol '%s'", name);
		yyerror(buf);
	}
}

void slicif_declare_array(char *name, SLIC_SYM type)
{
	char buf[1024];
	SlicNamedSymbol *sym = g_slicEngine->GetOrMakeSymbol(name);
	if(sym) {
		if(sym->GetType() != SLIC_SYM_UNDEFINED) {
			sprintf(buf, "Symbol '%s' in array declaration already has a type", name);
			yyerror(buf);
		} else {
			sym->SetType(SLIC_SYM_ARRAY);
			SlicStructDescription *desc = g_slicEngine->GetStructDescription(type);
			if(desc) {
				sym->SetArrayType(SLIC_SYM_STRUCT);
				sym->GetArray()->SetStructTemplate(desc);
			} else {
				sym->SetArrayType(type);
			}
		}
	} else {
		sprintf(buf, "Couldn't create array symbol '%s'", name);
		yyerror(buf);
	}
}

void slicif_declare_fixed_array(char *name, SLIC_SYM type, int size)
{
	char buf[1024];
	SlicNamedSymbol *sym = g_slicEngine->GetOrMakeSymbol(name);
	if(sym) {
		if(sym->GetType() != SLIC_SYM_UNDEFINED) {
			sprintf("Symbol '%s' in array declaration already has a type", name);
			yyerror(buf);
		} else {
			sym->SetType(SLIC_SYM_ARRAY);
			sym->GetArray()->FixSize(size);

			SlicStructDescription *desc = g_slicEngine->GetStructDescription(type);
			if(desc) {
				sym->SetArrayType(SLIC_SYM_STRUCT);
				sym->GetArray()->SetStructTemplate(desc);
			} else {
				sym->SetArrayType(type);
			}
		}
	} else {
		sprintf(buf, "Couldn't create array symbol '%s'", name);
		yyerror(buf);
	}
}

//----------------------------------------------------------------------------
//
// Name       : slicif_add_op
//
// Description: This function is used to compile slic code as far as it is 
//              known.
//
// Parameters : SOP op and as many as you want.
//
// Globals    : This function is so big no idea.
//
// Returns    : -
//
// Remark(s)  : The number of arguments depends on the given SOP type
//              if you define new operatiors you have of course modify
//              or implement the according behaviour of that function.
//
//----------------------------------------------------------------------------
void slicif_add_op(SOP op, ...)
{
	va_list vl;
	double dval;
	int ival;
	char *name;
	SlicNamedSymbol *symval;
	int offset;
	char *sptr;
	char internalName[k_MAX_FUNCTION_NAME];

	//Added by Martin Gühmann for database access
	SlicDBInterface *conduit;
	char *dbName;
//	string dbName;

	char errbuf[1024];

	va_start(vl, op);

	if(s_code_ptr - s_code > (s_allocated_code - 40)) {
		
		unsigned char *newcode = new unsigned char[s_allocated_code * 2];
		s_allocated_code *= 2;
		memcpy(newcode, s_code, s_code_ptr - s_code);
		s_code_ptr = newcode + (s_code_ptr - s_code);

		
		sint32 i, j;
		for(i = 0; i <= s_level; i++) {
			s_block_ptr[i] = newcode + (s_block_ptr[i] - s_code);
		}

		for(i = 0; i <= s_if_level; i++) {
			for(j = 0; j < s_if_stack[i].count; j++) {
				s_if_stack[i].array[j] = newcode + (s_if_stack[i].array[j] - s_code);
			}
		}

		delete [] s_code;
		s_code = newcode;
	}

	*s_code_ptr++ = (unsigned char)op;
	switch(op) {
		case SOP_PUSHD:
			
			dval = va_arg(vl, double);
			*((double*)s_code_ptr) = dval;
			s_code_ptr += sizeof(double);
			break;
		case SOP_PUSHI:
			
			ival = va_arg(vl, int);
			*((int*)s_code_ptr) = ival;
			s_code_ptr += sizeof(int);
			if(!s_argValuePushed && s_parenLevel > 0) {
				s_argValuePushed = true;
			}
			s_argSymbol = NULL;
			break;
		case SOP_PUSHV:
			
			name = va_arg(vl, char*);
			symval = slicif_get_symbol(name);
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", name);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(name);
			}
			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);

			if(!s_argValuePushed && (s_parenLevel > 0)) {
				
				s_argValuePushed = true;
				s_argSymbol = symval;
				s_argMemberIndex = -1;
			} else {
				
				s_argSymbol = NULL;
			}

			break;

		case SOP_PUSHA:
			
			name = va_arg(vl, char*);
			symval = slicif_get_symbol(name);
			if(!symval) {
				sprintf(errbuf, "Symbol '%s' is undefined", name);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(name);
			}
			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);
			if(!s_argValuePushed && (s_parenLevel > 0)) {
				
				s_argValuePushed = true;
				s_argSymbol = symval;
				s_argMemberIndex = -1;
			} else {
				
				s_argSymbol = NULL;
			}
			break;			
		case SOP_PUSHM:
		{
			
			char *structname = va_arg(vl, char *);
			name = va_arg(vl, char *);
			symval = slicif_get_symbol(structname);
			sint32 member = 0;
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", structname);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(structname);
			} else if(symval->GetType() != SLIC_SYM_STRUCT) {
				sprintf(errbuf, "%s is not a structure", structname);
				yyerror(errbuf);
			} else {
				SlicStructDescription *theStruct = symval->GetStruct()->GetDescription();
				member = (int)theStruct->GetMemberIndex(name);
				if(member < 0) {
					sprintf(errbuf, "%s is not a member of %s", name, structname);
					yyerror(errbuf);
				}
			}
			
			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);

			
			*((int *)s_code_ptr) = member;
			s_code_ptr += sizeof(int);

			if(!s_argValuePushed && (s_parenLevel > 0)) {
				
				s_argValuePushed = true;
				s_argSymbol = symval;
				s_argMemberIndex = member;
			} else {
				
				s_argSymbol = NULL;
			}
			break;
		}
		case SOP_PUSHAM:
		{
			
			char *structname = va_arg(vl, char *);
			name = va_arg(vl, char *);
			
			symval = slicif_get_symbol(structname);
			sint32 member = 0;
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", structname);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(structname);
			} else if(symval->GetType() != SLIC_SYM_ARRAY) {
				sprintf(errbuf, "%s is not an array", structname);
				yyerror(errbuf);
			} else {
				SlicStructDescription *theStruct = symval->GetArray()->GetStructTemplate();
				member = (int)theStruct->GetMemberIndex(name);
				if(member < 0) {
					sprintf(errbuf, "%s is not a member of %s", name, structname);
					yyerror(errbuf);
				}
			}
			
			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);

			
			*((int *)s_code_ptr) = member;
			s_code_ptr += sizeof(int);	

			if(!s_argValuePushed && (s_parenLevel > 0)) {
				
				s_argValuePushed = true;
				s_argSymbol = symval;
				s_argMemberIndex = member;
			} else {
				
				s_argSymbol = NULL;
			}
			break;
		}			
		case SOP_SARGS:
			
			s_currentEventArgument[++s_parenLevel] = 0;
			s_argValuePushed = false;
			break;
		case SOP_ARGE:
			
			slicif_check_argument();
			s_argValuePushed = false;
			s_argSymbol = NULL;
			break;
		case SOP_ARGID:
			name = va_arg(vl, char*);
			symval = g_slicEngine->GetOrMakeSymbol(name);
			if(symval->GetType() != SLIC_SYM_ID) {
				if(symval->GetType() == SLIC_SYM_UNDEFINED) {
					symval->SetType(SLIC_SYM_ID);
				} else {
					sprintf(errbuf, "%s already defined", name);
					yyerror(errbuf);
				}
			}
			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);

			s_argSymbol = symval;

			slicif_check_argument();
			s_argValuePushed = false;
			s_argSymbol = NULL;
			break;
		case SOP_ARGS:
			ival = va_arg(vl, int);
			*((int *)s_code_ptr) = ival;
			s_code_ptr += sizeof(int);

			slicif_check_string_argument();
			s_argValuePushed = false;
			s_argSymbol = NULL;

			break;
		case SOP_ARGST:
			name = va_arg(vl, char *);
			symval = g_slicEngine->GetOrMakeSymbol(name);
			if(symval->GetType() == SLIC_SYM_UNDEFINED) {
				symval->SetType(SLIC_SYM_STRING);
			} else {
				
				
				Assert(symval->GetType() == SLIC_SYM_STRING);
			}

			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);

			slicif_check_hard_string_argument();
			s_argValuePushed = false;
			s_argSymbol = NULL;
			break;
		case SOP_CALL:
		case SOP_CALLR:
			slicif_check_num_args();

			s_parenLevel--;
			
			
			name = va_arg(vl, char*);
			if((symval = g_slicEngine->GetSymbol(name))) {
				if(symval->GetType() != SLIC_SYM_FUNC &&
				   symval->GetType() != SLIC_SYM_UFUNC) {
					sprintf(errbuf, "%s is not a function", name);
					yyerror(errbuf);
				}
			} else {
				strcpy(internalName, "_");
				strcat(internalName, name);
				if(!g_slicEngine->GetFunction(internalName)) {
					sprintf(errbuf, "No function named %s", name);
					yyerror(errbuf);
				}
				if(!(symval = g_slicEngine->GetSymbol(internalName))) {
					symval = g_slicEngine->GetOrMakeSymbol(internalName);
					symval->SetType(SLIC_SYM_FUNC);
				}
			}
			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);
			break;
		case SOP_EVENT:
		{
			slicif_check_num_args();
			s_parenLevel--;

			s_currentEvent = GEV_MAX;

			name = va_arg(vl, char *);
			GAME_EVENT ev = g_gevManager->GetEventIndex(name);
			*((int *)s_code_ptr) = (int)ev;
			s_code_ptr += sizeof(int);
			break;
		}

		case SOP_SBLK:
			ival = va_arg(vl, int);
			s_block_ptr[ival] = s_code_ptr;
			*((int *)s_code_ptr) = ival;
			s_code_ptr += sizeof(int);
			break;
		case SOP_END:
			
			ival = va_arg(vl, int);

			
			*((int *)s_code_ptr) = -1;
			s_code_ptr += sizeof(int);

			
			offset = s_code_ptr - s_code;
			*((int *)s_block_ptr[ival]) = offset;
			s_block_ptr[ival][-1] = SOP_JMP;

			break;
		case SOP_BUTN:
			offset = s_block_ptr[s_level] - s_code;
			*((int *)s_code_ptr) = offset + sizeof(int);
			s_code_ptr += sizeof(int);

			ival = va_arg(vl, int);
			*((int *)s_code_ptr) = ival;
			s_code_ptr += sizeof(int);
			break;
		case SOP_OCLS:
			offset = s_block_ptr[s_level] - s_code;
			*((int *)s_code_ptr) = offset + sizeof(int);
			s_code_ptr += sizeof(int);
			break;
		case SOP_BNT:
		case SOP_BNEV:
			
			
			sptr = (char *)(s_block_ptr[s_level] - 1);
			*sptr = op;
			sptr++;
			*((int*)sptr) = (int)(s_code_ptr - s_code) - 1; 

			
			
			s_block_ptr[s_level] = s_code_ptr - 5;

			
			
			slicif_add_if_clause_end((char *)(s_code_ptr - 5));
			*(s_code_ptr - 6) = SOP_JMP;
			
			
			
			s_code_ptr -= 1;
			
			break;
			
		case SOP_JMP:
			ival = va_arg(vl, int);
			*((int *)s_code_ptr) = ival;
			s_code_ptr += sizeof(int);
			break;
		case SOP_ASSN:
			
			name = va_arg(vl, char*);
			symval = slicif_get_symbol(name);
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", name);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(name);
			}

			if(symval->GetType() == SLIC_SYM_UNDEFINED) {
				sprintf(errbuf, "Variable '%s' used in assignment has unknown type", name);
				yyerror(errbuf);
				symval->SetType(SLIC_SYM_IVAR);
			} else if(symval->IsParameter()) {
				sprintf(errbuf, "Function parameters are read-only");
				yyerror(errbuf);
			}

			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);
			break;			
		case SOP_ASSNA:
			
			
			
			name = va_arg(vl, char *);
			symval = slicif_get_symbol(name);
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", name);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(name);
			}

			if(symval->GetType() != SLIC_SYM_ARRAY) {
				sprintf(errbuf, "Symbol '%s' used in array assignment is not an array", name);
				yyerror(errbuf);
			}

			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);
			break;
		case SOP_ASSNM:
		case SOP_ASSNAM:
		{
			
			
			int member;
			char *structname = va_arg(vl, char*); 
			name = va_arg(vl, char*);       
			symval = slicif_get_symbol(structname);
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", structname);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(structname);
			}

			if(symval->GetType() != SLIC_SYM_STRUCT) {
				sprintf(errbuf, "Symbol %s is not a struct", structname);
				yyerror(errbuf);
				member = -1;
			} else {
				
				SlicStructDescription *theStruct = symval->GetStruct()->GetDescription();
				member = (int)theStruct->GetMemberIndex(name);
				if(member < 0) {
					sprintf(errbuf, "Struct %s has no member named %s", structname, name);
					yyerror(errbuf);
				}
			}

			
			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);

			
			*((int *)s_code_ptr) = member;
			s_code_ptr += sizeof(int);

			break;
		}
		case SOP_TRIG:
			s_found_trigger = 1;
			break;
		case SOP_LINE:
			ival = va_arg(vl, int);
			*((int *)s_code_ptr) = ival;
			s_code_ptr += sizeof(int);

			ival = va_arg(vl, int);
			*((int *)s_code_ptr) = ival;
			s_code_ptr += sizeof(int);

			*((void **)s_code_ptr) = NULL;
			s_code_ptr += sizeof(void *);

			break;
		case SOP_ASIZE:
			name = va_arg(vl, char *);
			symval = slicif_get_symbol(name);
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", name);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(name);
			}

			if(symval->GetType() != SLIC_SYM_ARRAY) {
				sprintf(errbuf, ".# operator only works on arrays");
				yyerror(errbuf);
			}

			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);
			break;
//Added by Martin Gühmann for database support
		case SOP_DBNAME:
		{
			conduit = va_arg(vl, SlicDBInterface *);
			Assert(conduit);

			name = va_arg(vl, char*);
			symval = slicif_get_symbol(name);
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", name);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(name);
			}

			//Save the database name to the code data, by saving every
			//single char including the /0 char.
			int i;
			dbName = const_cast<char*>(conduit->GetName());
			for(i = 0; dbName[i] != '\0'; ++i){
				*((char*)s_code_ptr) = dbName[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = dbName[i];
			s_code_ptr += sizeof(char);

			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);


			if(!s_argValuePushed && (s_parenLevel > 0)) {
				
				s_argValuePushed = true;
				s_argSymbol = symval;
				s_argMemberIndex = -1;
			} else {
				
				s_argSymbol = NULL;
			}

			break;
		}
		case SOP_DBNAMEREF:
		case SOP_DBNAMEARRAY:
		{
			conduit = va_arg(vl, SlicDBInterface *);
			Assert(conduit);

			//Get variable name in the argument list.
			name = va_arg(vl, char*);
			symval = slicif_get_symbol(name);
			if(!symval) {
				sprintf(errbuf, "Symbol %s is undefined", name);
				yyerror(errbuf);
				symval = g_slicEngine->GetOrMakeSymbol(name);
				//variable name is now free and can be reused. 
			}

			//Get referenced name of a flag from the according database.
			name = va_arg(vl, char*);
			if(!conduit->IsTokenInDB(name)){
				sprintf(errbuf, "Token %s not found in %s", name, conduit->GetName());
				yyerror(errbuf);
			}

			//Save the database name to the code data, by saving every
			//single char including the /0 char.
			int i;
			dbName = const_cast<char*>(conduit->GetName());
			for(i = 0; dbName[i] != '\0'; ++i){
				*((char*)s_code_ptr) = dbName[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = dbName[i];
			s_code_ptr += sizeof(char);

			*((int *)s_code_ptr) = symval->GetIndex();
			s_code_ptr += sizeof(int);

			//Do the same thing with the record member name:
			for(i = 0; name[i] != '\0'; ++i){
				*((char*)s_code_ptr) = name[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = name[i];
			s_code_ptr += sizeof(char);

			if(!s_argValuePushed && (s_parenLevel > 0)) {
				
				s_argValuePushed = true;
				s_argSymbol = symval;
				s_argMemberIndex = -1;
			} else {
				
				s_argSymbol = NULL;
			}

			break;
		}
		case SOP_DBNAMECONSTARRAY:
		{
			conduit = va_arg(vl, SlicDBInterface *);
			Assert(conduit);

			//Get variable name in the argument list.
			ival = va_arg(vl, int);

			//Get referenced name of a flag from the according database.
			name = va_arg(vl, char*);
			if(!conduit->IsTokenInDB(name)){
				sprintf(errbuf, "Token %s not found in %s", name, conduit->GetName());
				yyerror(errbuf);
			}

			//Save the database name to the code data, by saving every
			//single char including the /0 char.
			int i;
			dbName = const_cast<char*>(conduit->GetName());
			for(i = 0; dbName[i] != '\0'; ++i){
				*((char*)s_code_ptr) = dbName[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = dbName[i];
			s_code_ptr += sizeof(char);

			*((int*)s_code_ptr) = ival;
			s_code_ptr += sizeof(int);

			//Do the same thing with the record member name:
			for(i = 0; name[i] != '\0'; ++i){
				*((char*)s_code_ptr) = name[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = name[i];
			s_code_ptr += sizeof(char);

			if(!s_argValuePushed && (s_parenLevel > 0)) {
				
				s_argValuePushed = true;
				s_argSymbol = NULL;
				s_argMemberIndex = -1;
			} else {
				
				s_argSymbol = NULL;
			}

			break;
		}
		case SOP_DB:
		{
			conduit = va_arg(vl, SlicDBInterface *);
			Assert(conduit);

			//Save the database name to the code data, by saving every
			//single char including the /0 char.
			int i;
			dbName = const_cast<char*>(conduit->GetName());
			for(i = 0; dbName[i] != '\0'; ++i){
				*((char*)s_code_ptr) = dbName[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = dbName[i];
			s_code_ptr += sizeof(char);

			if(!s_argValuePushed && s_parenLevel > 0) {
				s_argValuePushed = true;
			}
			s_argSymbol = NULL;

			break;
		}
		case SOP_DBREF:
		case SOP_DBARRAY:
		{
			conduit = va_arg(vl, SlicDBInterface *);
			Assert(conduit);

			name = va_arg(vl, char *);
			if(!conduit->IsTokenInDB(name)){
				sprintf(errbuf, "Token %s not found in %s", name, conduit->GetName());
				yyerror(errbuf);
			}
			
			//Save the database name to the code data, by saving every
			//single char including the /0 char.
			int i;
			dbName = const_cast<char*>(conduit->GetName());
			for(i = 0; dbName[i] != '\0'; ++i){
				*((char*)s_code_ptr) = dbName[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = dbName[i];
			s_code_ptr += sizeof(char);

			//Do the same thing with the record member name:
			for(i = 0; name[i] != '\0'; ++i){
				*((char*)s_code_ptr) = name[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = name[i];
			s_code_ptr += sizeof(char);
			
			if(!s_argValuePushed && s_parenLevel > 0) {
				s_argValuePushed = true;
			}
			s_argSymbol = NULL;

			break;
		}
		case SOP_DBSIZE:
		{
			//Added by Martin Gühmann to figure out via 
			//slic how many records the database contains
			conduit = va_arg(vl, SlicDBInterface *);
			Assert(conduit);

			//Save the database name to the code data, by saving every
			//single char including the /0 char.
			int i;
			dbName = const_cast<char*>(conduit->GetName());
			for(i = 0; dbName[i] != '\0'; ++i){
				*((char*)s_code_ptr) = dbName[i];
				s_code_ptr += sizeof(char);
			}
			*((char*)s_code_ptr) = dbName[i];
			s_code_ptr += sizeof(char);

			if(!s_argValuePushed && s_parenLevel > 0) {
				s_argValuePushed = true;
			}
			s_argSymbol = NULL;
			break;
		}
		default:
			break;
	}
	va_end(vl);
}

void slicif_start_block()
{
	if(s_level > 0) {
		slicif_add_op(SOP_SBLK, s_level);
	}
	++s_level;
}

void slicif_end_block()
{
	--s_level;
	if(s_level > 0) {
		slicif_add_op(SOP_END, s_level);
	}
}

void slicif_start_if()
{
	++s_if_level;
	s_if_stack[s_if_level].count = 0;
}

void slicif_add_if_clause_end(char *ptr)
{
	s_if_stack[s_if_level].array[s_if_stack[s_if_level].count++] = (unsigned char *)ptr;
}

void slicif_end_if()
{
	int i;
	for(i = 0; i < s_if_stack[s_if_level].count; i++) {
		*((int *)s_if_stack[s_if_level].array[i]) = s_code_ptr - s_code;
	}
	--s_if_level;
}

void slicif_start_while()
{
	++s_while_level;
	s_while_stack[s_while_level].expression = s_code_ptr - s_code;
	s_while_stack[s_while_level].increment = -1;
}

void slicif_end_while()
{
	char *sptr;

	
	s_code_ptr -= 5;

	
	slicif_add_op(SOP_JMP, s_while_stack[s_while_level].expression);

	
	
	sptr = (char *)(s_block_ptr[s_level] - 1);
	*sptr = SOP_BNT;
	sptr++;
	*((int *)sptr) = (int)(s_code_ptr - s_code);

	
	s_while_level--;
}	
	
#ifdef _DEBUG
//----------------------------------------------------------------------------
//
// Name       : slicif_dump_code
//
// Description: This function is for debug purposes only and dumps error
//              messages to a file presumably
//
// Parameters : unsigned char* code
//              int codeSize
//
// Globals    : This function is so big no idea.
//
// Returns    : -
//
// Remark(s)  : This function is used for debbug purposes only, it should
//              also be modified if you add new SOP types.
//
//              As far as known this function is also used and slic compiling
//              time.
//
//----------------------------------------------------------------------------
void slicif_dump_code(unsigned char* code, int codeSize)
{
	unsigned char* codePtr = code;
	double dval;
	int ival, ival2;
	SlicNamedSymbol *symval;
	//Added by Martin Gühmann for database access
//	SlicDBInterface *conduit;
	char* name;
	const char* dbName;

	extern FILE *debuglog;

	while(codePtr < code + codeSize) {
		SOP op = (SOP)*codePtr;
		fprintf(debuglog, "%04lx: ", codePtr - code);
		codePtr++;
		switch(op) {
			case SOP_PUSHI:
				fprintf(debuglog, "pushi ");
				ival = *((int*)codePtr);
				fprintf(debuglog, "%d\n", ival);
				codePtr += sizeof(int);
				break;
			case SOP_PUSHD:
				fprintf(debuglog, "pushd ");
				dval = *((double*)codePtr);
				fprintf(debuglog, "%lf\n", dval);
				codePtr += sizeof(int);
				break;
			case SOP_PUSHV:
				ival = *((int*)codePtr);
				codePtr += sizeof(int);
				
				
				
				
				symval = g_slicEngine->GetSymbol(ival);
				if(!symval) {
					fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
					return;
				}
				fprintf(debuglog, "pushv %s(%d)\n", symval->GetName(), ival);
				break;
			case SOP_PUSHA:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				
				
				
				
				symval = g_slicEngine->GetSymbol(ival);
				if(!symval) {
					fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
					return;
				}
				fprintf(debuglog, "pusha %s(%d)\n", symval->GetName(), ival);
				break;
			case SOP_PUSHM:
				{
				    ival = *((int *)codePtr);
				    codePtr += sizeof(int);
				    symval = g_slicEngine->GetSymbol(ival);

				    ival2 = *((int *)codePtr);
				    codePtr += sizeof(int);

                    SlicStructInstance *    gotStruct   = symval->GetStruct();
                    char const *            memberName  = 
                        gotStruct 
                        ? gotStruct->GetDescription()->GetMemberName(ival2) 
                        : NAME_STRUCT_INVALID;
			        fprintf(debuglog, "pushm %s(%d).%s(%d)\n", 
                            symval->GetName(), symval->GetIndex(), memberName, ival2
                           );
                }
				break;
			case SOP_PUSHAM:
				
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				symval = g_slicEngine->GetSymbol(ival);
				
				if(!symval) {
					fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
					return;
				}

				
				ival2 = *((int *)codePtr);
				codePtr += sizeof(int);
				fprintf(debuglog, "pusham %s(%d)[].%s(%d)\n", symval->GetName(), symval->GetIndex(),
						symval->GetArray()->GetStructTemplate()->GetMemberName(ival2), ival2);
				break;
			case SOP_AINDX: fprintf(debuglog, "aindx\n"); break;
			case SOP_ADD:  fprintf(debuglog, "add\n"); break;
			case SOP_SUB:  fprintf(debuglog, "sub\n"); break;
			case SOP_MULT: fprintf(debuglog, "mult\n"); break;
			case SOP_EXP: fprintf(debuglog, "pow\n"); break;
			case SOP_BAND: fprintf(debuglog, "band\n"); break;
			case SOP_BOR: fprintf(debuglog, "bor\n"); break;
			case SOP_BXOR: fprintf(debuglog, "bxor\n"); break;
			case SOP_BNOT: fprintf(debuglog, "bnot\n"); break;
			case SOP_DIV:  fprintf(debuglog, "div\n"); break;
			case SOP_MOD:  fprintf(debuglog, "mod\n"); break;
			case SOP_EQ:   fprintf(debuglog, "eq\n"); break;
			case SOP_GT:   fprintf(debuglog, "gt\n"); break;
			case SOP_LT:   fprintf(debuglog, "lt\n"); break;
			case SOP_GTE:  fprintf(debuglog, "gte\n"); break;
			case SOP_LTE:  fprintf(debuglog, "lte\n"); break;
			case SOP_POP:  fprintf(debuglog, "pop\n"); break;
			case SOP_TRIG: fprintf(debuglog, "trig\n"); break;
			case SOP_ARGE: fprintf(debuglog, "arge\n"); break;
			case SOP_NEQ:  fprintf(debuglog, "neq\n"); break;
			case SOP_ARGID:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				
				
				
				
				symval = g_slicEngine->GetSymbol(ival);
				if(!symval) {
					fprintf(debuglog, "Bad Mojo, NULL symbol %d\n", ival);
					return;
				}
				if(symval->GetType() != SLIC_SYM_ID) {
					fprintf(debuglog, "Bad Mojo, symbol %s is not of type ID\n",
							symval->GetName());
					return;
				}
				fprintf(debuglog, "argid %s(%d)\n", symval->GetName(), ival);
				break;
			case SOP_ARGS: 
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				
				
				
				
				symval = g_slicEngine->GetSymbol(ival);
				if(symval->GetType() != SLIC_SYM_SVAR) {
					fprintf(debuglog, "Bad Mojo, string id arg doesn't have string id type\n");
					return;
				}
				fprintf(debuglog, "args  %d(%s)\n", ival, symval->GetName());
				break;
			case SOP_ARGST:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				
				
				
				
				symval = g_slicEngine->GetSymbol(ival);
				if(symval->GetType() != SLIC_SYM_STRING) {
					fprintf(debuglog, "Bad Mojo, string arg doesn't have string type\n");
					return;
				}
				fprintf(debuglog, "argst %d(%s)\n", ival, symval->GetName());
				break;
			case SOP_CALL:
			case SOP_CALLR:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				
				
				
				
				symval = g_slicEngine->GetSymbol(ival);
				if(!symval) {
					fprintf(debuglog, "Bad Mojo, NULL symbol %d\n", ival);
					return;
				}
				if(symval->GetType() != SLIC_SYM_FUNC &&
				   symval->GetType() != SLIC_SYM_UFUNC) {
					fprintf(debuglog, "Bad Mojo, symbol %s is not a function\n",
							symval->GetName());
					return;
				}
				fprintf(debuglog, "call%c %s(%d)\n", (op == SOP_CALL) ? ' ' : 'r', 
						symval->GetName(), ival);
				break;
			case SOP_EVENT:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);

				fprintf(debuglog, "event %s\n", g_gevManager->GetEventName((GAME_EVENT)ival));
				break;
			case SOP_SBLK:
				fprintf(debuglog, "dangling SBLK\n");
				return;
			case SOP_END:
				fprintf(debuglog, "end\n");
				codePtr += sizeof(int);
				break;
			case SOP_JMP:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				fprintf(debuglog, "jmp   0x%04lx\n", ival);
				break;
			case SOP_BNT:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				fprintf(debuglog, "bnt   0x%04lx\n", ival);
				break;
			case SOP_BNEV:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				fprintf(debuglog, "bnev  0x%04lx\n", ival);
				break;
			case SOP_BUTN:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				ival2 = *((int *)codePtr);
				codePtr += sizeof(int);
				
				
				
				

				symval = g_slicEngine->GetSymbol(ival2);
				if(symval->GetType() != SLIC_SYM_SVAR) {
					fprintf(debuglog, "Bad Mojo, button string arg doesn't have string type\n");
					return;
				}
				fprintf(debuglog, "butn  0x%04lx,%d(%s)\n", ival,
						symval->GetIndex(), symval->GetName());
				break;
			case SOP_OCLS:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				fprintf(debuglog, "ocls  0x%04lx\n", ival);
				break;
			case SOP_STOP:
				fprintf(debuglog, "stop\n");
				break;
			case SOP_NEG:
				fprintf(debuglog, "neg\n");
				break;
			case SOP_ASSN:
				ival = *((int*)codePtr);
				codePtr += sizeof(int);
				
				
				
				
				symval = g_slicEngine->GetSymbol(ival);
				if(!symval) {
					fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
					return;
				}
				fprintf(debuglog, "assn  %s(%d)\n", symval->GetName(), ival);
				break;
			case SOP_ASSNA:
				ival = *((int*)codePtr);
				codePtr += sizeof(int);
				
				
				
				
				symval = g_slicEngine->GetSymbol(ival);
				if(!symval) {
					fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
				} else {
					fprintf(debuglog, "assna %s(%d)\n", symval->GetName(), ival);
				}
				break;
			case SOP_AND: fprintf(debuglog, "and\n"); break;
			case SOP_OR:  fprintf(debuglog, "or\n"); break;
			case SOP_NOT: fprintf(debuglog, "not\n"); break;
			case SOP_SARGS: fprintf(debuglog, "sargs\n"); break;
			case SOP_RET:   fprintf(debuglog, "ret\n"); break;
			case SOP_LINE: 
				ival = *((int *)codePtr);
				codePtr += sizeof(int);

				ival2 = *((int *)codePtr);
				codePtr += sizeof(int);

				codePtr += sizeof(void *);
				fprintf(debuglog, "line  %d/%d\n", ival, ival2);
				break;
			case SOP_ASIZE:
				ival = *((int *)codePtr);
				codePtr += sizeof(int);
				symval = g_slicEngine->GetSymbol(ival);
				if(!symval) {
					fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
				} else {
					fprintf(debuglog, "asize %s(%d)\n", symval->GetName(), ival);
				}
				break;
//Added by Martin Gühmann for database support
			case SOP_DBNAME:
			{
				//Get the database name:
				int i;
				dbName = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				ival = *((int*)codePtr);
				codePtr += sizeof(int);

				if(!g_slicEngine->GetDBConduit(dbName)) {
					fprintf(debuglog, "%s\n", dbName);
					fprintf(debuglog, "Bad mojo, NULL db\n");
				} else {

					symval = g_slicEngine->GetSymbol(ival);
					if(!symval) {
						fprintf(debuglog, "%s\n", dbName);
						fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
						return;
					}
					fprintf(debuglog, "%s %s(%d)\n", dbName, symval->GetName(), ival);
				}
				break;
			}
			case SOP_DBNAMEREF:
			{
				//Get the database name:
				int i;
				dbName = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				ival = *((int*)codePtr);
				codePtr += sizeof(int);

				//Get the member name:
				name = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				if(!g_slicEngine->GetDBConduit(dbName)) {
					fprintf(debuglog, "%s\n", dbName);
					fprintf(debuglog, "Bad mojo, NULL db\n");
				} else {

					symval = g_slicEngine->GetSymbol(ival);
					if(!symval) {
						fprintf(debuglog, "%s\n", dbName);
						fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
						return;
					}
					fprintf(debuglog, "%s(%s).%s, %s == (%d)\n", dbName, symval->GetName(), name, symval->GetName(), ival);
				}
				break;
			}
			case SOP_DBNAMEARRAY:
			{
				//Get the database name:
				int i;
				dbName = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				ival = *((int*)codePtr);
				codePtr += sizeof(int);

				//Get the member name:
				name = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				if(!g_slicEngine->GetDBConduit(dbName)) {
					fprintf(debuglog, "%s\n", dbName);
					fprintf(debuglog, "Bad mojo, NULL db\n");
				} else {
					symval = g_slicEngine->GetSymbol(ival);
					if(!symval) {
						fprintf(debuglog, "%s\n", dbName);
						fprintf(debuglog, "Bad mojo, NULL symbol %d\n", ival);
						return;
					}
					fprintf(debuglog, "%s(%s).%s[], %s == (%d)\n", dbName, symval->GetName(), name, symval->GetName(), ival);
				}
				break;
			}
			case SOP_DBNAMECONSTARRAY:
			{
				//Get the database name:
				int i;
				dbName = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				ival = *((int*)codePtr);
				codePtr += sizeof(int);

				//Get the member name:
				name = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				if(!g_slicEngine->GetDBConduit(dbName)) {
					fprintf(debuglog, "%s\n", dbName);
					fprintf(debuglog, "Bad mojo, NULL db\n");
				} else {
					fprintf(debuglog, "%s(%s).%s[]\n", dbName, g_slicEngine->GetDBConduit(dbName)->GetRecordNameByIndex(ival), name);
				}
				break;
			}
			case SOP_DB:
			{
				//Get the database name:
				int i;
				dbName = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				if(!g_slicEngine->GetDBConduit(dbName)) {
					fprintf(debuglog, "%s\n", dbName);
					fprintf(debuglog, "Bad mojo, NULL db\n");
				} else {
					fprintf(debuglog, "%s\n", dbName);
				}
				break;
			}
			case SOP_DBREF:
			{
				//Get the database name:
				int i;
				dbName = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				//Get the member name:
				name = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				if(!g_slicEngine->GetDBConduit(dbName)) {
					fprintf(debuglog, "%s\n", dbName);
					fprintf(debuglog, "Bad mojo, NULL db\n");
				} else {
					fprintf(debuglog, "%s(..).%s\n", dbName, name);
				}
				break;
			}
			case SOP_DBARRAY:
			{
				//Get the database name:
				int i;
				dbName = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				//Get the member name:
				name = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				if(!g_slicEngine->GetDBConduit(dbName)) {
					fprintf(debuglog, "%s\n", dbName);
					fprintf(debuglog, "Bad mojo, NULL db\n");
				} else {
					fprintf(debuglog, "%s(..).%s[]\n", dbName, name);
				}
				break;
			}
			case SOP_DBSIZE:
			{
				//Added by Martin Gühmann to figure out via 
				//slic how many records the database contains
				//Get the database name:
				int i;
				dbName = ((char*)codePtr);
				for(i = 0; *((char*)codePtr) != '\0'; ++i){
					codePtr += sizeof(char);
				}
				codePtr += sizeof(char);

				if(!g_slicEngine->GetDBConduit(dbName)) {
					fprintf(debuglog, "%s\n", dbName);
					fprintf(debuglog, "Bad mojo, NULL db\n");
				} else {
					fprintf(debuglog, "%s\n", dbName);
				}
				break;
			}
			default:
				fprintf(debuglog, "???\n");
				break;
		}
	}
}
#endif

int slicif_find_string(char *id)
{
	char errbuf[1024];
	
	
	SlicNamedSymbol *sym = g_slicEngine->GetOrMakeSymbol(id + 3);
	if(sym->GetType() == SLIC_SYM_UNDEFINED) {
		sym->SetType(SLIC_SYM_SVAR);
	} else if(sym->GetType() != SLIC_SYM_SVAR) {
		sprintf(errbuf, "%s is not a string variable", id + 3);
		yyerror(errbuf);
	}

	if(!slicif_is_valid_string(id + 3)) {
		sprintf(errbuf, "%s not found in string databse", id + 3);
		yyerror(errbuf);
	}

	return sym->GetIndex();
}

void slicif_set_file_num(int num)
{
	s_file_num = num;
}

void slicif_add_region(char *name, int x1, int y1, int x2, int y2)
{
#if 0
	struct PSlicSymbol *rgnsym;

#ifdef _DEBUG
	extern FILE *debuglog;
	fprintf(debuglog, "Adding region %s: (%d,%d)-(%d,%d)\n", name, 
			x1,y1,x2,y2);
#endif

	rgnsym = slicif_lookup_sym(name);
	if(!rgnsym || rgnsym->m_type != SLIC_SYM_UNDEFINED) {
		slic_parse_error = SLIC_ERROR_ALREADY_USED;
		return;
	}

	rgnsym->m_type = SLIC_SYM_REGION;
	rgnsym->m_region = (PSlicRegion *)malloc(sizeof(struct PSlicRegion));
	rgnsym->m_region->x1 = x1;
	rgnsym->m_region->y1 = y1;
	rgnsym->m_region->x2 = x2;
	rgnsym->m_region->y2 = y2;
#endif
}

void slicif_start_complex_region(char *name)
{
#if 0
	struct PSlicSymbol *rgnsym;
	
#ifdef _DEBUG
	extern FILE *debuglog;
	fprintf(debuglog, "Complex region %s: ", name);
#endif
	rgnsym = slicif_lookup_sym(name);
	if(!rgnsym || rgnsym->m_type != SLIC_SYM_UNDEFINED) {
		slic_parse_error = SLIC_ERROR_ALREADY_USED;
		return;
	}

	s_current_complex_region = rgnsym;
#endif
}

void slicif_finish_complex_region()
{
#if 0
#ifdef _DEBUG
	extern FILE *debuglog;
	fprintf(debuglog, "\n");
#endif
	
	s_current_complex_region = NULL;
#endif

}

void slicif_add_region_to_complex(char *name)
{
#if 0
	struct PSlicComplexRegion *oldhead, *chk;
	struct PSlicSymbol *addregion;
#ifdef _DEBUG
	extern FILE *debuglog;
#endif

	if(s_current_complex_region == NULL) {
		slic_parse_error = SLIC_ERROR_INTERNAL;
		return;
	}
	
	addregion = slicif_find_sym(name, 1);
	if(!addregion) {
		slic_parse_error = SLIC_ERROR_UNKNOWN_REGION;
		return;
	}

	if(addregion->m_type != SLIC_SYM_REGION &&
	   addregion->m_type != SLIC_SYM_COMPLEX_REGION) {
		slic_parse_error = SLIC_ERROR_SYMBOL_NOT_REGION;
		return;
	}
		
	if(addregion->m_type == SLIC_SYM_REGION) {
		
#ifdef _DEBUG
		fprintf(debuglog, "[%d,%d - %d,%d] ", 
				addregion->m_region->x1,
				addregion->m_region->y1,
				addregion->m_region->x2,
				addregion->m_region->y2);
#endif
		oldhead = s_current_complex_region->m_complex_region;
		s_current_complex_region->m_complex_region = (PSlicComplexRegion *)malloc(sizeof(struct PSlicComplexRegion));
		*s_current_complex_region->m_complex_region = *(struct PSlicComplexRegion *)addregion->m_region;
		s_current_complex_region->m_complex_region->next = oldhead;
	} else {
		
		for(chk = addregion->m_complex_region; chk; chk = chk->next) {
			struct PSlicComplexRegion *last = s_current_complex_region->m_complex_region;
#ifdef _DEBUG
			fprintf(debuglog, "[%d,%d - %d,%d] ", 
					chk->x1, chk->y1, chk->x2, chk->y2);
#endif
			s_current_complex_region->m_complex_region = (PSlicComplexRegion *)malloc(sizeof(struct PSlicComplexRegion));
			*s_current_complex_region->m_complex_region = *chk;
			s_current_complex_region->m_complex_region->next = last;
		}
	}
#endif
}

void slicif_start_segment(char *name)
{
	s_inSegment = 1;
	strcpy(s_current_segment_name, name);
	slicif_register_line(g_slicLineNumber, -1);
}

char *slicif_get_segment_name_copy()
{
	char *name = (char *)malloc(strlen(s_current_segment_name) + 1);
	strcpy(name, s_current_segment_name);
	return name;
}

void slicif_add_parameter(SLIC_SYM type, char *name)
{
	char namebuf[1024];
	SlicSymbolData *sym;

	
	slicif_get_local_name(namebuf, name);

	if((sym = g_slicEngine->GetSymbol(namebuf))) {
		
		
		char errbuf[1024];
		sprintf(errbuf, "'%s' already has a local definition", name);
		yyerror(errbuf);
	} else {
		
		SlicParameterSymbol *psym = g_slicEngine->GetParameterSymbol(namebuf, s_parameter_index++);
		Assert(psym);
		Assert(psym->IsParameter());
		SlicStructDescription *desc = g_slicEngine->GetStructDescription(type);
		if(desc) {
			psym->SetType(SLIC_SYM_STRUCT);
			psym->SetStruct(new SlicStructInstance(desc, psym));
		} else {			
			psym->SetType(type);
		}
		s_parameters[s_num_parameters] = psym->GetIndex();
		s_num_parameters++;
	}
}

void slicif_function_return(SF_RET rettype)
{
	s_function_return_type = rettype;
}

void slicif_get_local_name(char *localName, char *name)
{
	sprintf(localName, "%s#%s", s_current_segment_name, name);
}

void slicif_add_prototype(char *name)
{
	SlicSymbolData *sym;
	sym = g_slicEngine->GetOrMakeSymbol(name);
	char errbuf[1024];
	if(sym->GetType() != SLIC_SYM_UNDEFINED) {
		sprintf(errbuf, "Symbol '%s' is already defined", name);
		yyerror(errbuf);
	} else {
		sym->SetType(SLIC_SYM_UFUNC);
	}

	if(s_num_parameters > 0) {
		
		
		sprintf(errbuf, "Prototypes should not define arguments\n");
		yyerror(errbuf);
	}
}


void slicif_start_for()
{
}


void slicif_for_expression()
{
	++s_while_level;
	s_while_stack[s_while_level].expression = s_code_ptr - s_code;
	s_while_stack[s_while_level].increment = -1;
}


void slicif_for_continue()
{
	
	
	slicif_add_op(SOP_JMP, -1);
	s_while_stack[s_while_level].increment = s_code_ptr - s_code;
}

void slicif_start_for_body()
{
	
	
	slicif_add_op(SOP_JMP, s_while_stack[s_while_level].expression);
}


void slicif_end_for()
{
	char *sptr;

	
	s_code_ptr -= 5;

	
	slicif_add_op(SOP_JMP, s_while_stack[s_while_level].increment);

	
	
	sptr = (char *)(s_block_ptr[s_level] - 1);
	*sptr = SOP_BNT;
	sptr++;
	*((int *)sptr) = (int)(s_code_ptr - s_code);

    
	sptr = (char*)(s_code +  s_while_stack[s_while_level].increment - 5);
	sptr++; 
	*((int *)sptr) = (int)(s_block_ptr[s_level] - 1 - s_code);

	
	s_while_level--;
}

int slicif_find_const(char *name, int *value)
{
	return (int)g_slicEngine->FindConst(name, (sint32*)value);
}

void slicif_add_const(char *name, int value)
{
	g_slicEngine->AddConst((MBCHAR *)name, (sint32)value);
}

void slicif_check_event_exists(char *name)
{
	GAME_EVENT ev = g_gevManager->GetEventIndex(name);
	if(ev >= GEV_MAX) {
		char errbuf[1024];
		sprintf(errbuf, "No event named %s", name);
		yyerror(errbuf);
	}
}

char *slicif_create_name(char *base)
{
	char *name = (char *)malloc(strlen(base) + 10);
	sprintf(name, "%s!%08lx", base, s_temp_name_counter++);
	return name;
}

static SLIC_PRI s_priority;
void slicif_set_priority(SLIC_PRI pri)
{
	s_priority = pri;
}

SLIC_PRI slicif_get_priority()
{
	return s_priority;
}

void slicif_set_event_checking(char *eventname)
{
	
	
	
	
	s_event_checking = 1;
	const char *argString = g_gevManager->GetArgString(g_gevManager->GetEventIndex(eventname));

	memset(s_arg_counts, 0, sizeof(s_arg_counts));

	while(*argString) {
		Assert(*argString == '%' || *argString == '&');
		argString++; 
		if(!*argString)
			break;
		
		GAME_EVENT_ARGUMENT argType = g_gevManager->ArgCharToIndex(*argString);
		s_arg_counts[argType]++;
		argString++;
	}
}

void slicif_add_valid_builtin(char *name)
{
}

void slicif_add_local_struct(char *structtype, char *name)
{
#if 0
	char errbuf[1024];
	char localname[1024];
	struct PSlicSymbol *sym;

	SlicStruct *theStruct = g_slicEngine->GetStruct(structtype);
	if(!theStruct) {
		sprintf(errbuf, "Unknown struct %s", structtype);
		yyerror(errbuf);
		return;
	}

	slicif_get_local_name(localname, name);
	if((sym == slicif_find_sym(localname, 1))) {
		sprintf(errbuf, "%s already defined locally", name);
		yyerror(errbuf);
		return;
	}

	sym = slicif_lookup_sym(localname);
	sym->m_type = SLIC_SYM_STRUCT;
	sym->m_structType = theStruct;
#endif
}

void slicif_register_line(int line, int offset)
{
	
	
	
	
	
	
	if(s_inSegment && g_theProfileDB->IsDebugSlic()) {
		slicif_add_op(SOP_LINE, line, offset);
	}
}


SlicNamedSymbol *slicif_get_symbol(char *name)
{
	char localname[1024];
	slicif_get_local_name(localname, name);

	SlicNamedSymbol *sym = g_slicEngine->GetSymbol(localname);
	if(sym)
		return sym;

	sym = g_slicEngine->GetSymbol(name);
	return sym;
}


void slicif_start_event(char *name)
{
	char errbuf[1024];

	s_currentEvent = g_gevManager->GetEventIndex(name);
	if(s_currentEvent >= GEV_MAX) {
		sprintf(errbuf, "Event %s does not exist", name);
		yyerror(errbuf);
		return;
	}

	slicif_add_op(SOP_SARGS);
}

void slicif_check_arg_symbol(SLIC_SYM type, char *typeName)
{
	char errbuf[1024];
	SLIC_SYM symType;

	if(!s_argSymbol) {
		
		
		
		if(type != SLIC_SYM_IVAR && type != SLIC_SYM_PLAYER && type != SLIC_SYM_LOCATION &&
			type != SLIC_SYM_CITY && type != SLIC_SYM_UNIT && type != SLIC_SYM_ARMY) {
			sprintf(errbuf, "Argument %d requires a symbol", s_currentEventArgument[s_parenLevel] + 1);
			yyerror(errbuf);
		}
		return;
	}

	if(s_argSymbol->GetType() == SLIC_SYM_ARRAY) {
		if(s_argSymbol->GetArray()->GetType() == SS_TYPE_INT) {
			symType = SLIC_SYM_IVAR;
		} else {
			SlicSymbolData *structDataSym = s_argSymbol->GetArray()->GetStructTemplate()->CreateDataSymbol();
			symType = structDataSym->GetType();
			delete structDataSym;
		}
	} else if(s_argSymbol->GetType() == SLIC_SYM_STRUCT) {
		if(s_argMemberIndex < 0) {
			symType = s_argSymbol->GetStruct()->GetDataSymbol()->GetType();
		} else {
			symType = s_argSymbol->GetStruct()->GetMemberSymbol(s_argMemberIndex)->GetType();
		}
	} else {
		symType = s_argSymbol->GetType();
	}

	if(type == SLIC_SYM_PLAYER && symType == SLIC_SYM_IVAR) {
		
		return;
	}
	if(symType != type) {
		sprintf(errbuf, "Type mismatch for argument %d, expected %s", s_currentEventArgument[s_parenLevel] + 1, typeName);
		yyerror(errbuf);
		return;
	}
}

void slicif_check_argument()
{
	if(s_currentEvent < GEV_MAX) {
		char argChar = g_gevManager->ArgChar(s_currentEvent, s_currentEventArgument[s_parenLevel]);
		switch(argChar) {
			case GEAC_ARMY:
				slicif_check_arg_symbol(SLIC_SYM_ARMY, "army_t");
				break;
			case GEAC_UNIT:
				slicif_check_arg_symbol(SLIC_SYM_UNIT, "unit_t");
				break;
			case GEAC_CITY:
				slicif_check_arg_symbol(SLIC_SYM_CITY, "city_t");
				break;
			case GEAC_POP:
				slicif_check_arg_symbol(SLIC_SYM_POP, "pop_t");
				break;
			case GEAC_GOLD:
				slicif_check_arg_symbol(SLIC_SYM_IVAR, "int_t");
				break;
			case GEAC_PATH:
				slicif_check_arg_symbol(SLIC_SYM_PATH, "path_t");
				break;
			case GEAC_MAPPOINT:
				slicif_check_arg_symbol(SLIC_SYM_LOCATION, "location_t");
				break;
			case GEAC_PLAYER:
				slicif_check_arg_symbol(SLIC_SYM_PLAYER, "int_t");
				break;
			case GEAC_INT:
				slicif_check_arg_symbol(SLIC_SYM_IVAR, "int_t");
				break;
			case GEAC_DIRECTION:
				slicif_check_arg_symbol(SLIC_SYM_IVAR, "int_t");
				break;
				
			case GEAC_ADVANCE:
				slicif_check_arg_symbol(SLIC_SYM_IVAR, "int_t");
				break;
			case GEAC_WONDER:
				slicif_check_arg_symbol(SLIC_SYM_IVAR, "int_t");
				break;
			case GEAC_IMPROVEMENT:
				slicif_check_arg_symbol(SLIC_SYM_IMPROVEMENT, "improvement_t");
				break;
		}
		s_currentEventArgument[s_parenLevel]++;
	}
}


void slicif_check_num_args()
{
	char errbuf[1024];

	if((s_currentEvent < GEV_MAX) && s_parenLevel == 1) {
		if((s_currentEventArgument[s_parenLevel]) != g_gevManager->GetNumArgs(s_currentEvent)) {
			sprintf(errbuf, "Wrong number of arguments for event %s, expected %d",
			        g_gevManager->GetEventName(s_currentEvent),
			        g_gevManager->GetNumArgs(s_currentEvent));
			yyerror(errbuf);
			return;
		}
	}
}

void slicif_check_string_argument()
{
}

void slicif_check_hard_string_argument()
{
}

//----------------------------------------------------------------------------
//
// Name       : slicif_find_db
//
// Description: Handels slic database access.
//
// Parameters : const char *dbname: A name of a database in slic.
//              void **dbptr:       A pointer that can be converted into
//                                  a SlicDBInterface interface object.
//
// Globals    : -
//
// Returns    : Returns whether there is a database with such a name
//              given by dbname. So finally 1 or 0.
//              
// Remark(s)  : -
//
//----------------------------------------------------------------------------
int slicif_find_db(const char *dbname, void **dbptr)
{
	SlicDBInterface *conduit = g_slicEngine->GetDBConduit(dbname);
	if(conduit) {
		*dbptr = (void *)conduit;
		return TRUE;
	} else {
		dbptr = NULL;
		return FALSE;
	}
}

//----------------------------------------------------------------------------
//
// Name       : slicif_find_db_index
//
// Description: Handels slic database access.
//
// Parameters : void *dbptr:      Represents a database for instance UnitDB.
//              const char *name: Represents name that can be found in the 
//                                given database for instance UNIT_SETTLER.
//
// Globals    : -
//
// Returns    : The database index of the given name.For instance 
//              UnitDB(UNIT_SETTLER) gives the the datbase index of the
//              unit with name UNIT_SETTLER.
//              
// Remark(s)  : This function is called at copiling time.
//
//----------------------------------------------------------------------------
int slicif_find_db_index(void *dbptr, const char *name)
{
	SlicDBInterface * conduit = reinterpret_cast<SlicDBInterface *>(dbptr);
	Assert(conduit);
	if(!conduit)
		return 0;

	sint32 index = conduit->GetIndex(name);
	if(index < 0) {
		char errbuf[1024];
		sprintf(errbuf, "%s not found in %s", name, conduit->GetName());
		yyerror(errbuf);
	}
	return index;
}

//----------------------------------------------------------------------------
//
// Name       : slicif_find_db_value
//
// Description: Handels slic database access.
//
// Parameters : void *dbptr:         Represents a database for instance UnitDB.
//              const char *recname: Represents name that can be found in the 
//                                   given database for instance UNIT_SETTLER.
//              const char *valname: Represents a flag of an entry in the given 
//                                   database for instance MaxMovePoints.
//
// Globals    : -
//
// Returns    : Gets the value of a flag of a given entry in a given 
//              database. For instance UnitDB(UNIT_SETTLER).MaxMovePoints 
//              gives the value of the MaxMovePoints flag of the entry 
//              in UnitDB with the internal name UNIT_SETTLER.
//              
// Remark(s)  : This function is called at copiling time in normal slic 
//              function but called at run time from the Great Libary.
//
//----------------------------------------------------------------------------
int slicif_find_db_value(void *dbptr, const char *recname, const char *valname)
{
	SlicDBInterface * conduit = reinterpret_cast<SlicDBInterface *>(dbptr);
	char errbuf[1024];
	Assert(conduit);
	if(!conduit)
		return 0;

	sint32 index;
	if((index = conduit->GetIndex(recname)) < 0) {
		sprintf(errbuf, "%s not found in %s", recname, conduit->GetName());
		yyerror(errbuf);
		return 0;
	}
#if 0
	//Looks like it interferes at other places
	//but I leave it in for possible later use.
	if(!conduit->IsTokenInDB(valname)){
		sprintf(errbuf, "Token %s not found in %s", valname, conduit->GetName());
		yyerror(errbuf);
	}
#endif

	return conduit->GetValue(index, valname);
}

//----------------------------------------------------------------------------
//
// Name       : slicif_find_db_value_by_index
//
// Description: Handels slic database access.
//
// Parameters : void *dbptr:         Represents a database for instance UnitDB.
//              int index:           Represents an index in the given database.
//              const char *valname: Represents a flag of an entry in the given 
//                                   database for instance MaxMovePoints.
//
// Globals    : -
//
// Returns    : Gets the value of a flag of a given entry in a given 
//              database. For instance UnitDB(0).MaxMovePoints gives
//              the value of the MaxMovePoints flag of the first entry in
//              UnitDB.
//              
// Remark(s)  : This function is called both at copiling time and run time 
//              in normal slic function but called at run time from the 
//              Great Libary.
//
//
//----------------------------------------------------------------------------
int slicif_find_db_value_by_index(void *dbptr, int index, const char *valname)
{
	SlicDBInterface * conduit = reinterpret_cast<SlicDBInterface *>(dbptr);
	Assert(conduit);
	if(!conduit)
		return 0;

#if 0
	//Looks like it interferes at other places
	//but I leave it in for possible later use.
	if(!conduit->IsTokenInDB(valname)){
		char errbuf[1024];
		sprintf(errbuf, "Token %s not found in %s", valname, conduit->GetName());
		yyerror(errbuf);
	}
#endif
	return conduit->GetValue(index, valname);
}

//----------------------------------------------------------------------------
//
// Name       : slicif_find_db_array_value
//
// Description: Handels slic database access.
//
// Parameters : void *dbptr:         Represents a database for instance UnitDB.
//              const char *recname: Represents name that can be found in the 
//                                   given database for instance UNIT_SETTLER.
//              const char *valname: Represents a flag of an entry in the given 
//                                   database for instance MaxMovePoints.
//              sint32 val:          Represents an array index.
//
// Globals    : -
//
// Returns    : Gets the value of a flag of a given entry in a given 
//              database. For instance UnitDB(UNIT_SETTLER).MaxMovePoints 
//              gives the value of the MaxMovePoints flag of the entry 
//              in UnitDB with the internal name UNIT_SETTLER.
//              
// Remark(s)  : This function is called at copiling time in normal slic 
//              function but called at run time from the Great Libary.
//
//----------------------------------------------------------------------------
int slicif_find_db_array_value(void *dbptr, const char *recname, const char *valname, int val)
{
	SlicDBInterface * conduit = reinterpret_cast<SlicDBInterface *>(dbptr);
	char errbuf[1024];
	Assert(conduit);
	if(!conduit)
		return 0;

	sint32 index;
	if((index = conduit->GetIndex(recname)) < 0) {
		sprintf(errbuf, "%s not found in %s", recname, conduit->GetName());
		yyerror(errbuf);
		return 0;
	}
#if 0
	//Looks like it interferes at other places
	//but I leave it in for possible later use.
	if(!conduit->IsTokenInDB(valname)){
		sprintf(errbuf, "Token %s not found in %s", valname, conduit->GetName());
		yyerror(errbuf);
	}
#endif

	return conduit->GetValue(index, valname, val);
}

//----------------------------------------------------------------------------
//
// Name       : slicif_find_db_array_value_by_index
//
// Description: Handels slic database access.
//
// Parameters : void *dbptr:         Represents a database for instance UnitDB.
//              int index:           Represents an index in the given database.
//              const char *valname: Represents a flag of an entry in the given 
//                                   database for instance MaxMovePoints.
//              sint32 val:          Represents an array index.
//
// Globals    : -
//
// Returns    : Gets the value of a flag of a given entry in a given 
//              database. For instance UnitDB(0).MaxMovePoints gives
//              the value of the MaxMovePoints flag of the first entry in
//              UnitDB.
//              
// Remark(s)  : This function is called both at copiling time and run time 
//              in normal slic function but called at run time from the 
//              Great Libary.
//
//
//----------------------------------------------------------------------------
int slicif_find_db_array_value_by_index(void *dbptr, int index, const char *valname, int val)
{
	SlicDBInterface * conduit = reinterpret_cast<SlicDBInterface *>(dbptr);
	Assert(conduit);
	if(!conduit)
		return 0;

#if 0
	//Looks like it interferes at other places
	//but I leave it in for possible later use.
	if(!conduit->IsTokenInDB(valname)){
		char errbuf[1024];
		sprintf(errbuf, "Token %s not found in %s", valname, conduit->GetName());
		yyerror(errbuf);
	}
#endif
	return conduit->GetValue(index, valname, val);
}



//----------------------------------------------------------------------------
//
// Name       : slicif_is_name
//
// Description: Retrieves the database index of the given, same as 
//              slicif_find_db_index but without an error message 
//              if it fails.
//
// Parameters : const char *name
//              void *dbptr
//
// Globals    : -
//
// Returns    : The database index of an given name in the given
//              database.
//
// Remark(s)  : This function is only used at compiling time, to determine
//              wheather a given name represents a name in the given 
//              database. That is double work as it is also done in the 
//              slicif_find_db_index if it is called. But as it is only
//              done at compile time, the additional time needed shouldn't
//              be a problem.
//
//----------------------------------------------------------------------------
int slicif_is_name(void *dbptr, const char *name)
{
	SlicDBInterface * conduit = reinterpret_cast<SlicDBInterface *>(dbptr);

	Assert(conduit);
	if(!conduit)
		return 0;

	return conduit->GetIndex(name);
}
