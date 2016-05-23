#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SLICCMD_H__
#define __SLICCMD_H__

#define k_MAX_SLICCMD_NAME 256

#if defined(__cplusplus)
extern "C" {
#endif

#define EXP_VAL_INT 0
#define EXP_VAL_SYM 1
#define EXP_VAL_TEMP_SYM 2

struct sliccmdExpValue {
	int type;
	char name[k_MAX_SLICCMD_NAME];
	int value;
	void *sym;
	void *dbptr;
};

extern int sliccmd_use_dialogs;
extern char *sliccmd_output;
extern int sliccmd_output_len;
extern int sliccmd_int_result;
extern const char *sliccmd_cat_string;

void sliccmd_arg_exp(int value);
void sliccmd_arg_id(const char *id);
void sliccmd_arg_stringid(int id);
void sliccmd_arg_string(const char *name);
int sliccmd_get_int_value(const char *symName);
int sliccmd_call(const char *funcName);
int sliccmd_ref_has_int_value(const char *structName, const char *memberName);
int sliccmd_get_ref_value(const char *structName, const char *memberName);
void *sliccmd_get_ref_sym(const char *structName, const char *memberName);

int sliccmd_array_lookup(const char *arrayName, int index);
void *sliccmd_array_lookup_reference(const char *arrayName, int index, const char *memberName);

int sliccmd_has_int_value(const char *symName);
int sliccmd_sym_has_int_value(const void *vsym, int *value);
int sliccmd_array_has_int_value(const char *symName, int index);

void *sliccmd_lookup_sym(const char *name);
void *sliccmd_maybe_lookup_sym(const char *name);
void *sliccmd_array_lookup_sym(const char *arrayName, int index);

void sliccmd_parse_done(struct sliccmdExpValue *v, int action);
void sliccmd_type_error();

void sliccmd_error(const char *s);

void sliccmd_clear_symbols();

void sliccmd_assign(void *vsym, struct sliccmdExpValue *v);

void *sliccmd_get_db_name_sym(void *dbptr, const char *name);
void *sliccmd_get_db_name_sym_by_index(void *dbptr, int index);

int sliccmd_parse(int action, const char *cmd, char *output, int outputlen, int useDialogs, const char *catString);
void yyscerror(const char *s);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
class SlicSymbolWatchCallback;
class SlicSymbolData;

void sliccmd_add_watch(SlicSymbolWatchCallback *watch);
void sliccmd_add_symbol_used(SlicSymbolData *sym);
SlicSymbolData *sliccmd_get_symbol(const char *name);

#endif

#endif
