#ifndef __iparser__h__
#define __iparser__h__


















































































































#define DIRECTORY_CHARACTER '\\'

#define EARLY_END_OF_FILE (-1)
#define BAD_DECLARATION (-3)
#define BAD_VARIABLE (-2)
#define INT_VARIABLE 1
#define FLOAT_VARIABLE 2
#define DOUBLE_VARIABLE 3
#define CHAR_VARIABLE 4
#define STRUCT_DEFINITION 5
#define DEFINE_DECLARATION 7
#define STRUCT_ARRAY_VARIABLE 8
#define VARIABLE_ASSIGN 9
#define STRING_VARIABLE 10
#define CONDITIONAL_CODE 11
#define PARSE_FOR_LOOP 12
#define CODE_BRACKET 13
#define ELSE_CODE 14
#define INCLUDE_DIRECTIVE 16

#define NO_DECLARATION 6

#define IPARSE_MAX_STRING_LENGTH 80

#define INT_TOK 1
#define DOUBLE_TOK 3
#define STR_TOK 4

#define MAX_SYMBOLS 1000
#define MAX_FIELD_SYMBOLS 30














enum {IPARSE_NO_ERR = 0, VARIABLE_NOT_FOUND, WRONG_TYPE, OTHER_ERROR};






#define IFAILURE (-1)
#define NO_FILE_ERROR 10
#define IPARSE_DONT_REPORT_ERROR  (-17)


enum {NO_TYPE = -1, WHITE_SPACE, ALNUM, SYMBOL, END_OF_FILE};

enum {REPORT_WARNING, REPORT_ERROR, REPORT_FATAL};

class AiMain;




typedef struct _field_def
{
	int field_type;
	char *field_name;
	int field_length;
	struct _field_def *next_field;
} field_def;

typedef struct _struct_list_element
{
	char *struct_name;
	int struct_length;
	field_def *the_definition;
	struct _struct_list_element *next_struct;
} struct_list_element;

typedef struct _variable_struct
{
	int variable_type;
	char *variable_name;
	char *string_value;

	int  int_value;
	double double_value;
	struct_list_element *the_struct_el;
	int struct_element_count;
	struct _variable_struct *next_array_variable;
	struct _variable_struct *next_variable;
} variable_struct;









class iparser
{




protected:
	int number_of_variables;
	variable_struct *variables;
	int number_of_structures;
	struct_list_element *the_structs;
	linked_list text_path;
	AiMain *ai;
    char m_current_file_name[80];





public:









	iparser(AiMain *init_ai);













	~iparser();











	int Parse_File
	(
	  char *file_name
	);













	signed char Get_Runtime_Char
	(
	  char *name,
	  int *error
	);













	int Get_Runtime_Int
	(
	  char *name,
	  int *error
	);













	double Get_Runtime_Float
	(
	  char *name,
	  int *error
	);













	double Get_Runtime_Double
	(
	  char *name,
	  int *error
	);














	char *Get_Runtime_String
	(
	  char *name,
	  int *error,
	  char *new_ptr
	);













	char *Allocate_And_Get_Runtime_String
	(
	  char *name,
	  int *error
	);














	signed char Get_Runtime_Char_Field
	(
	  char *name,
	  int *error
	);













	int Get_Runtime_Int_Field
	(
	  char *name,
	  int *error
	);













	double Get_Runtime_Float_Field
	(
	  char *name,
	  int *error
	);













	double Get_Runtime_Double_Field
	(
	  char *name,
	  int *error
	);













	char *Get_Runtime_String_Field
	(
	  char *name,
	  int *error,
	  char *buf
	);














	char *Allocate_And_Get_Runtime_String_Field
	(
	  char *name,
	  int *error
	);













	int Get_Struct_Array_Element_Count
	(
	  char *name,
	  int *error
	);

















	void Append_Path
	(
	  char *new_path
	);





protected:






	void Report_Error
	(
	  char *message,
	  int report_type
	);









	struct_list_element *Get_New_Struct
	(
	  char *the_struct_name
	);









	variable_struct *Get_New_Variable
	(
	  int the_variable_type,
	  char *the_variable_name
	);









	field_def *Get_New_Field
	(
	  int the_field_type,
	  char *the_field_name
	);








	void Insert_Variable
	(
	  variable_struct *the_variable
	);








	void Insert_Struct
	(
	  struct_list_element *the_struct
	);










	int Read_Next_String
	(
	  char *parse_buffer,
	  int *parse_pos,
	  int buffer_length,
	  char *declaration_string
	);










	struct_list_element *Which_Struct
	(
	  char *declaration_string
	);










	variable_struct *Which_Variable
	(
	  char *declaration_string
	);











	int Variable_Match
	(
	  char *declaration_string
	);










	int Search_For_Next_Declaration
	(
	  char *parse_buffer,
	  int *parse_pos,
	  int buffer_length,
	  char *declaration_string
	);











	int Read_Until_String
	(
		char *parse_buffer,
		int *parse_pos,
		int buffer_length,
		char *end_string,
		char **declaration_buffer,
		int & current_declaration_length
	);











	void Append_Good_Char
	(
	  char *to_string,
	  int *pos,
	  char the_char
	);











	void Append_Quote_Char
	(
	  char *to_string,
	  int *pos,
	  char the_char
	);










	char * Save_Only_Alpha_Numeric
	(
		char *a_string,
		int (*check_char_fcn)(char),
		int source_length
	);











	int Parse_Int
	(
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);










	int Parse_Char
	(
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);










	int Parse_Double
	(
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types,
	  int the_type
	);












	int Assign_To_Variable
	(
	  char *declaration_string,
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);












	int Parse_Pound_Define
	(
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);













	int Parse_Include
	(
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);










	void Append_Field_To_Structure
	(
	  struct_list_element *new_struct,
	  int *current_symbol,
	  int num_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int field_type,
	  int the_field_length
	);












	int Add_Next_Field
	(
	  struct_list_element *new_struct,
	  int *current_symbol,
	  int num_symbols,
	  int *int_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH]
	);











	int Parse_Struct
	(
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);











	int Fill_Individual_Field
	(
	  char *buf,
	  int the_type,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types,
	  int *current_symbol
	);











	void Fill_All_Fields
	(
	  struct_list_element *the_struct,
	  char **buf,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types,
	  int *current_symbol
	);











	int Parse_Struct_Array
	(
	  char *declaration_string,
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);











	int Find_Matched_Bracket
	(
	  char *parse_buffer,
	  int parse_pos,
	  int buffer_length
	);








	void Get_Next_Statement_Start_And_End
	(
	  char *parse_buffer,
	  int *parse_pos,
	  int buffer_length,
	  int *the_start,
	  int *the_end
	);










	int Get_Else_Start_And_End
	(
	  char *parse_buffer,
	  int *temp_pos,
	  int buffer_length,
	  int *the_start,
	  int *the_end
	);











	int Parse_Conditional_Code
	(
	  char *parse_buffer,
	  int *parse_pos,
	  int buffer_length,
	  int num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);











	int Next_Symbol
	(
	  char *the_string,
      const int the_string_len,
	  int *place_in_the_string,
	  char *new_symbol,
      const int new_symbol_len

	);










	void Parse_New_Command
	(
	  char *command_str,
      const int command_str_len,
	  int *num_symbols,
	  int *int_symbols,
	  double *double_symbols,
	  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH],
	  int *symbol_types
	);












	int Parse_Declaration
	(
		char *parse_buffer,
		int *parse_pos,
		int buffer_length,
		int declaration_type,
		char *declaration_string,
		char **buf,
		int current_declaration_length
	);











	char *Get_Termination_String
	(
	  int declaration_type,
	  char *end_string
	);










	int Complete_Declaration
	(
		char *parse_buffer,
		int *parse_pos,
		int buffer_length,
		int declaration_type,
		char *declaration_string,
		char *declaration_buffer

	);












	int Do_Parse
	(
	  char *parse_buffer,
	  int *parse_pos,
	  int buffer_length
	);












	void Read_File_Into_Buffer
	(
	  FILE *parse_file,
	  char **parse_buffer,
	  int *length
	);








	void Trash_Variables
	(
	  variable_struct *the_variable
	);










	void Trash_Field_Definitions
	(
	  field_def *the_field
	);










	void Trash_Structs
	(
	  struct_list_element *the_struct
	);












	char *Get_Runtime_Struct_Field
	(
		char *source_name
	);

};






#endif
