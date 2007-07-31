#ifndef INCexpressionsP_h
#define INCexpressionsP_h

#ifdef __cplusplus
extern "C" {
#endif










































































#define CAST_TO_INT 1
#define CAST_TO_DOUBLE 2

#define EXP_MAX_STRING_LENGTH 80


















void bomb (char * message);










double expression
(
	iparser * parse_domain,				
	int first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);








double equals_not_equals
(
	iparser * parse_domain,				
	int *first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);








double greater_than_less_than
(
	iparser * parse_domain,				
	int *first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);







double add_or_subtract
(
	iparser * parse_domain,				
	int *first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);







double multiply_or_divide
(
	iparser * parse_domain,				
	int *first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);







double exponent
(
	iparser * parse_domain,				
	int *first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);








double negation
(
	iparser * parse_domain,				
	int *first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);







int check_type_cast
(
	char *type_string					
);








double parentheses
(
	iparser * parse_domain,				
	int *first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);








double constant_or_variable
(
	iparser * parse_domain,				
	int *first_symbol,					
	int last_symbol,					
	int *int_symbols,					
	double *double_symbols,				
	char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
	int *symbol_types,					
	int *is_integer,					
	int *int_value,						
	int *error							
);

#ifdef __cplusplus
}
#endif

#endif 
