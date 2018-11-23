#ifndef LDLIF_H__
#define LDLIF_H__

#define LDL_ERROR_OK 0
#define LDL_ERROR_NOTOK 1
#define LDL_ERROR_CANT_OPEN_FILE 2
#define LDL_ERROR_INCLUDE 3

#define MAX_INCLUDE_DEPTH 64

#if defined(__cplusplus)
class ldl_datablock;

ldl_datablock *ldlif_find_block(const char *name);
void ldlif_add_block_to_tree(ldl_datablock *block);
void ldlif_remove_block_from_tree(ldl_datablock *block);

extern "C" {
#endif

extern int g_ldlLineNumber;

int ldlif_open_first_file(const char *file);
const char *ldlif_current_file();

int ldlif_find_file(const char *filename, char *buf);

const char *ldlif_getnameptr(const char *name);
const char *ldlif_getstringptr(const char *text);

int ldlif_parse(const char *filename);

void ldlif_add_name(void **newnames, const char *name, void *oldnames);
void ldlif_start_block(void *names);
void *ldlif_end_block(void *names);
void *ldlif_add_empty_block(void *names);

void ldlif_add_bool_attribute(const char *name, int val);
void ldlif_add_int_attribute(const char *name, int val);
void ldlif_add_float_attribute(const char *name, double val);
void ldlif_add_string_attribute(const char *name, const char *val);

void ldlif_indent_log(int indent);
void ldlif_init_log();
void ldlif_log(const char *fmt, ...)
#ifdef __GNUC__
	__attribute__((__format__ (__printf__, 1, 2)))
#endif
	;

void ldlif_allocate_stuff();
void ldlif_deallocate_stuff();

#if defined(__cplusplus)
}
#endif

#endif
