/**
 * $Id$
 */
#ifndef __CTPDB_H__
#define __CTPDB_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define Assert(x) { if(!(x)) { fprintf(stderr, "Assert %s:%d: %s\n", __FILE__, __LINE__, #x); } }

typedef enum {
	DATUM_INT,
	DATUM_BIT,
	DATUM_FLOAT,
	DATUM_FILE,
	DATUM_RECORD,
	DATUM_STRUCT,
	DATUM_BIT_GROUP,
	DATUM_STRING,
	DATUM_STRINGID,
	DATUM_BIT_PAIR,
	DATUM_NONE,
} DATUM_TYPE;

union dbvalue {
	sint32 intValue;
	double floatValue;
	char *textValue;
};

struct namelist {
	char *name;
	uint8 flags;
	union dbvalue v;
	struct namelist *next;
};

struct fieldsize {
	int minSize, maxSize;
};

struct bitpairtype {
	uint32 type;
	void *extraData; 
};

#define k_NAMEVALUE_HAS_VALUE 0x01
#define k_NAMEVALUE_INT 0x02
#define k_NAMEVALUE_FLOAT 0x04
#define k_NAMEVALUE_STRING 0x08

struct namevalueflags {
	char *name;
	uint8 flags;
	union dbvalue v;
};

#define k_MAX_SIZE_VARIABLE (0x7fffffff)

void db_start_record(char *name);
void db_end_record(char *name);

void db_make_int_db(char *name);
void db_make_float_db(char *name);
void db_make_string_db(char *name);
void db_make_string_id_db(char *name);

void db_start_member_class(char *name);
void db_end_member_class(char *name);

void db_add_bits(struct namelist *list, struct fieldsize *size);
void db_add_ints(struct namelist *list, struct fieldsize *size);
void db_add_floats(struct namelist *list, struct fieldsize *size);
void db_add_records(char *recType, struct namelist *list, struct fieldsize *size);
void db_add_structs(char *structType, struct namelist *list, struct fieldsize *size);
void db_add_filenames(struct namelist *list, struct fieldsize *size);
void db_add_strings(struct namelist *list, struct fieldsize *size);
void db_add_string_ids(struct namelist *list, struct fieldsize *size);
void db_add_bit_pair(struct namelist *list, struct fieldsize *size, struct bitpairtype *pairtype);
void db_add_grouped_bits(char *groupName, struct namelist *list);

void db_set_output_dir(char *dir);

#if defined(__cplusplus)
}
#endif

#endif
