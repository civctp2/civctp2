%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctpdb.h"

#define lint

void yyerror(char *err);

int yylex();

int s_done = 0;
int g_line_number = 1;
int g_generateRequirementWarnings = 0;

%}

%union {
	int val;
	double fval;
	char *name;
	struct namevalueflags nameValue;
	struct namelist *list;
	struct fieldsize size;
	struct bitpairtype pairtype;
}

%token INT FLOAT RECORD STRUCT BIT BITS EXCLUSIVEBITS NAME NUMBER STRING STRINGID
%token FILENAME FLOATVALUE STRINGVALUE RANGE

%%
databases: records { s_done = 1; }

records: records record
       | record
       ;

record: NAME '{' { db_start_record($1.name); } data '}' { db_end_record($1.name); }
		| NAME ':' INT { db_make_int_db($1.name); }
		| NAME ':' FLOAT { db_make_float_db($1.name); }
		| NAME ':' FILENAME  { db_make_string_db($1.name); }
		| NAME ':' STRING { db_make_string_db($1.name); }
		| NAME ':' STRINGID { db_make_string_id_db($1.name); }
		;

data: data datum
	| datum
	;

datum:   structdescription
	   | bits
	   | exclusivebits
	   | BIT size names         { db_add_bits  ($3.list, &$2.size); }
	   | BIT '(' bitpairtype ')' size names { db_add_bit_pair($6.list, &$5.size, &$3.pairtype); }
	   | INT size names         { db_add_ints  ($3.list, &$2.size); }
	   | FLOAT size names       { db_add_floats($3.list, &$2.size); }
	   | RECORD NAME size names { db_add_records($2.name, $4.list, &$3.size); }
	   | STRUCT NAME size names { db_add_structs($2.name, $4.list, &$3.size); }
	   | FILENAME size names    { db_add_filenames($3.list, &$2.size); }
	   | STRING size names      { db_add_strings($3.list, &$2.size); }
	   | STRINGID size names    { db_add_string_ids($3.list, &$2.size); }
	   ;

bitpairtype: INT { $$.pairtype.type = DATUM_INT; $$.pairtype.extraData = NULL; }
		| FLOAT { $$.pairtype.type = DATUM_FLOAT; $$.pairtype.extraData = NULL; }
		| RECORD NAME { $$.pairtype.type = DATUM_RECORD; $$.pairtype.extraData = $2.name; }
		| STRUCT NAME { $$.pairtype.type = DATUM_STRUCT; $$.pairtype.extraData = $2.name; }
		| FILENAME { $$.pairtype.type = DATUM_FILE; $$.pairtype.extraData = NULL;}
		| STRING { $$.pairtype.type = DATUM_STRING; $$.pairtype.extraData = NULL;}
		| STRINGID { $$.pairtype.type = DATUM_STRINGID; $$.pairtype.extraData = NULL;}
		;

size:    '[' NUMBER ']' { $$.size.minSize = $$.size.maxSize = $2.val; }
	   | '[' NUMBER RANGE NUMBER ']'{ $$.size.minSize = $2.val;  $$.size.maxSize = $4.val; }
       | '[' ']' { $$.size.minSize = 0; $$.size.maxSize = k_MAX_SIZE_VARIABLE; }
	   | { $$.size.minSize = $$.size.maxSize = -1;}
	   ;

names: names ',' name
		{
			struct namelist *newnode = malloc(sizeof(struct namelist));
			struct namelist *find;
			newnode->name = $3.nameValue.name;
			newnode->flags = $3.nameValue.flags;
			newnode->v = $3.nameValue.v;
			newnode->next = NULL;
			if($$.list) {
				find = $$.list;
				while(find->next) {
					find = find->next;
				}
				find->next = newnode;
			} else {
				$$.list = newnode;
			}
		}
	   | name
		{
			struct namelist *newnode = malloc(sizeof(struct namelist));
			newnode->name = $1.nameValue.name;
			newnode->flags = $1.nameValue.flags;
			newnode->v = $1.nameValue.v;
			newnode->next = NULL;
			$$.list = newnode;
		}
	       
	   ;

name: NAME 	{ 
				$$.nameValue.name = $1.name;
			 	$$.nameValue.flags = 0;
			}
	| NAME '=' NUMBER
	{
		$$.nameValue.name = $1.name;
		$$.nameValue.flags = (k_NAMEVALUE_HAS_VALUE) | (k_NAMEVALUE_INT);
		$$.nameValue.v.intValue = $3.val;
	}
	| NAME '=' FLOATVALUE
	{
		$$.nameValue.name = $1.name;
		$$.nameValue.flags = (k_NAMEVALUE_HAS_VALUE) | (k_NAMEVALUE_FLOAT);
		$$.nameValue.v.floatValue = $3.fval;
	}
		
     | NAME '=' STRINGVALUE
	{
		$$.nameValue.name = $1.name;
		$$.nameValue.flags = (k_NAMEVALUE_HAS_VALUE) | (k_NAMEVALUE_STRING);
		$$.nameValue.v.textValue = $3.name;
	}
	;

structdescription: STRUCT NAME '{' { db_start_member_class($2.name); } data '}' { db_end_member_class($2.name); }
				   ;

bits: BITS NAME '{' names '}' { db_add_grouped_bits($2.name, $4.list); }
	  ;

exclusivebits: EXCLUSIVEBITS NAME '{' names '}'
			   ;

%%
void yyerror(char *err)
{
	printf("Line %d: %s\n", g_line_number, err);
	s_done = 1;
}

int yyparse();

int main(int argc, char **argv)
{
	int arg;
	char *outputDir = NULL;
	g_generateRequirementWarnings = 1;

	for(arg = 1; arg < argc; arg++) {
		if(argv[arg][0] == '-') {
			if(strcmp(argv[arg], "-r") == 0) {
				g_generateRequirementWarnings = 0;
			}
		} else {
			outputDir = argv[arg];
		}
	}

	if(!outputDir) {
		fprintf(stderr, "Usage: [-r] %s <outputdir>\n", argv[0]);
		exit(0);
	}

	db_set_output_dir(outputDir);
	s_done = 0;
	g_line_number = 1;
	while(!s_done) {
		yyparse();
	}
}

		
