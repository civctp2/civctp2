



















































































#include "c3.h"
#include "c3types.h"
#include "c3errors.h"

#include <ctype.h>

#include "math.h" 
#include "common.h"
#include "Linked_List.h"
#include "Globals.h"
#include "AIMain.h"
#include "IParser.h"
#include "ExpressionP.h"






#define ADVANCE_TOKEN (*first_symbol)++
#define CURRENT_TOKEN_CHAR string_symbols[*first_symbol][0]








































void bomb (char * message)
{

}











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
)

{
  
  double value;							
  

  (*error) = IPARSE_NO_ERR;

  if (first_symbol > last_symbol)		
  {
    (*error) = OTHER_ERROR;
  }

  else value = equals_not_equals(parse_domain,
           &first_symbol, last_symbol, int_symbols,
           double_symbols,string_symbols,
           symbol_types, is_integer, int_value, error);

  return(value);						
}













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
)
{
	
	double term1, term2;				
	int term1_is_int, term1_int_val, term2_is_int, term2_int_val;
	int is_is;							
	int done = BOGUS;					
	

	
	term1 = greater_than_less_than(parse_domain,
	first_symbol, last_symbol, int_symbols,
	double_symbols,string_symbols,
	symbol_types, &term1_is_int, &term1_int_val, error);


	if (*first_symbol > last_symbol) done = EXCELLENT;
	else
		if (string_symbols[*first_symbol][0] is '=') 
		{
			if (string_symbols[(*first_symbol)+1][0] is '=') 
			{
				is_is = EXCELLENT;
				(*first_symbol)++;		
			} 
		}
	else if (string_symbols[*first_symbol][0] is '!') 
	{
		if (string_symbols[(*first_symbol)+1][0] is '=')
		{
			is_is = BOGUS;
			(*first_symbol)++;			
		}
	}
	else if (!(is_is = !strcmp(string_symbols[*first_symbol], "is")))
		if ((is_is = strcmp(string_symbols[*first_symbol], "isnt")))
			done = EXCELLENT;

	if (!done) 
	{
		(*first_symbol)++;				

		
		term2 = greater_than_less_than(parse_domain,
						first_symbol, last_symbol, int_symbols,
						double_symbols,string_symbols,
						symbol_types, &term2_is_int, &term2_int_val, 
						error);


		
		term1_is_int = term1_is_int && term2_is_int;

		
		if (term1_is_int)				
		{
			if (is_is)
				term1_int_val = (term1_int_val is term2_int_val);
			else
				term1_int_val = (term1_int_val isnt term2_int_val);
		}
		else							
		{
			if (is_is)
				term1_int_val = (term1 is term2);
			else
				term1_int_val = (term1 isnt term2);
		}

		term1_is_int = EXCELLENT;
	} 

	*is_integer = term1_is_int;			

	if (term1_is_int)					
	{ 
		*int_value = term1_int_val;
		return((double) term1_int_val);
	}

	*int_value = (int) term1;			
	return(term1);
}













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
)
{
 	
	double term1, term2;				
	int term1_is_int, term1_int_val, term2_is_int, term2_int_val;
	int done = BOGUS;					
	int gt = BOGUS, gte = BOGUS, lt = BOGUS, lte = BOGUS;
	

	
	term1 = add_or_subtract(parse_domain,
		   first_symbol, last_symbol, int_symbols,
		   double_symbols,string_symbols,
		   symbol_types, &term1_is_int, &term1_int_val, error);



  if (*first_symbol > last_symbol) done = EXCELLENT;
  else
    if (!(gt = !strcmp(string_symbols[*first_symbol], ">")))
      if (!(lt = !strcmp(string_symbols[*first_symbol], "<")))
  done = EXCELLENT;

  if (!done) {
    (*first_symbol)++;    

    if (!strcmp(string_symbols[*first_symbol], "=")) {
      if (gt) {
  gt = BOGUS;
  gte = EXCELLENT;
      }
      if (lt) {
  lt = BOGUS;
  lte = EXCELLENT;
      }

    (*first_symbol)++;    

    }

    term2 = add_or_subtract(parse_domain,
          first_symbol, last_symbol, int_symbols,
          double_symbols,string_symbols,
          symbol_types, &term2_is_int, &term2_int_val, 
          error);
                                       

    
    term1_is_int = term1_is_int && term2_is_int;

    
    if (term1_is_int) {   
      if (gte)
  term1_int_val = (term1_int_val >= term2_int_val);
      if (lte)
  term1_int_val = (term1_int_val <= term2_int_val);
      if (gt)
  term1_int_val = (term1_int_val > term2_int_val);
      if (lt)
  term1_int_val = (term1_int_val < term2_int_val);
    }
    else {        
      if (gte)
  term1_int_val = (term1 >= term2);
      if (lte)
  term1_int_val = (term1 <= term2);
      if (gt)
  term1_int_val = (term1 > term2);
      if (lt)
  term1_int_val = (term1 < term2);
    }

    term1_is_int = EXCELLENT;
  } 

  *is_integer = term1_is_int;     

  if (term1_is_int) {     
    *int_value = term1_int_val;
    return((double) term1_int_val);
  }
  *int_value = (int) term1;     
  return(term1);
}













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
)
{
	
  double term1, term2;      
  int term1_is_int, term1_int_val, term2_is_int, term2_int_val;
  char plus_or_minus;       
	

  term1 = multiply_or_divide(parse_domain,
           first_symbol, last_symbol, int_symbols,
           double_symbols,string_symbols,
           symbol_types, &term1_is_int, &term1_int_val, error);
          

  if (*first_symbol <= last_symbol) 
    while (((plus_or_minus = string_symbols[*first_symbol][0]) is '+')||
   (string_symbols[*first_symbol][0] is '-')) {

    (*first_symbol)++;      

    term2 = multiply_or_divide(parse_domain,
           first_symbol, last_symbol, int_symbols,
           double_symbols,string_symbols,
           symbol_types, &term2_is_int, &term2_int_val, error);
          

    
    term1_is_int = term1_is_int && term2_is_int;

    
    if (term1_is_int) {     
      if (plus_or_minus is '+')
  term1_int_val += term2_int_val; 
      else
  term1_int_val -= term2_int_val; 
    }
    else {        

      if (plus_or_minus is '+')
  term1 += term2;     
      else
  term1 -= term2;     
    }
  }

  *is_integer = term1_is_int;     

  if (term1_is_int) {     
    *int_value = term1_int_val;
    return((double) term1_int_val);
  }

  *int_value = (int) term1;     
  return(term1);
}












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
)

{
	
  double term1, term2;      
  char mult_or_div;       
  int term1_is_int, term1_int_val, term2_is_int, term2_int_val;
	

  term1 = exponent(parse_domain,
           first_symbol, last_symbol, int_symbols,
           double_symbols,string_symbols,
           symbol_types, &term1_is_int, &term1_int_val,error);
          

    if (*first_symbol <= last_symbol) 
      while (((mult_or_div = string_symbols[*first_symbol][0]) is '*')||
   (string_symbols[*first_symbol][0] is '/')) {

    (*first_symbol)++;      

    term2 = exponent(parse_domain,
           first_symbol, last_symbol, int_symbols,
           double_symbols,string_symbols,
           symbol_types, &term2_is_int, &term2_int_val, error);
          

    
    term1_is_int = term1_is_int && term2_is_int;

    
    if (term1_is_int) {     
      if (mult_or_div is '*')
  term1_int_val *= term2_int_val; 
      else
  term1_int_val /= term2_int_val; 
    }
    else {        
      if (mult_or_div is '*')
  term1 *= term2;     
      else
  term1 /= term2;     
    }
  }

  *is_integer = term1_is_int;     

  if (term1_is_int) {     
    *int_value = term1_int_val;
    return((double) term1_int_val);
  }

  *int_value = (int) term1;     

  return(term1);
}













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
)
{
	
  double term1, term2;      
  int i;        
  int int_result;
  int term1_is_int, term1_int_val, term2_is_int, term2_int_val;
	

  term1 = negation(parse_domain,
           first_symbol, last_symbol, int_symbols,
           double_symbols,string_symbols,
           symbol_types, &term1_is_int, &term1_int_val, error);
          

  if (string_symbols[*first_symbol][0] is '^'){

    (*first_symbol)++;      

    term2 = exponent(parse_domain,
           first_symbol, last_symbol, int_symbols,
           double_symbols,string_symbols,
           symbol_types, &term2_is_int, &term2_int_val, error);
          


    
    term1_is_int = term1_is_int && term2_is_int;

    
    if (term1_is_int) {     
      int_result = 1;       
      for (i=0;i<term2;i++) {
  int_result *= (int) term1;    
      }
      term1_int_val = int_result;
    }
    else {        
      term1 = pow(term1,term2);
    }
 
    
  }


  *is_integer = term1_is_int;     

  if (term1_is_int) {     
    *int_value = term1_int_val;
    return((double) term1_int_val);
  }

  *int_value = (int) term1;     
  return(term1);
}













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
)
{
	
  double term1;       
  int negate = BOGUS;
	

  if (string_symbols[*first_symbol][0] is '-'){

    (*first_symbol)++;      

    negate = EXCELLENT;

  }

  term1 = parentheses(parse_domain,
          first_symbol, last_symbol, int_symbols,
          double_symbols,string_symbols,
          symbol_types, is_integer, int_value, error);
          

  if (*is_integer) {
   if (negate)  
     *int_value *= -1;
   return((double) (*int_value));
  }

  if (negate) 
    term1 *= -1.0;

  *int_value = (int) term1;

  return(term1);
}













int check_type_cast
(
	char *type_string					
)                                  
{
  if (!strcmp(type_string, "int"))
    return(CAST_TO_INT);
  if (!strcmp(type_string, "char"))
    return(CAST_TO_INT);
  if (!strcmp(type_string, "long"))
    return(CAST_TO_INT);
  if (!strcmp(type_string, "float"))
    return(CAST_TO_DOUBLE);
  if (!strcmp(type_string, "double"))
    return(CAST_TO_DOUBLE);
  return(BOGUS);
}













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
)
{
	
  double term1;       
  int do_type_cast = BOGUS;     
	

  if (string_symbols[*first_symbol][0] is '('){

    
    (*first_symbol)++;      
    if (do_type_cast = check_type_cast(string_symbols[*first_symbol])) {
      (*first_symbol)++;    
 
      if (string_symbols[*first_symbol][0] isnt ')') {
  bomb("Unmatched parenthesis in type-casting\n");
  return(-1.0);
      }

      (*first_symbol)++;    
      term1 = negation(parse_domain,
        first_symbol, last_symbol, int_symbols,
        double_symbols,string_symbols,
        symbol_types, is_integer, int_value, error);
          
      switch (do_type_cast) {
      case CAST_TO_INT :
  *is_integer = EXCELLENT;
  term1 = (int) term1;
  break;
      default :
  *is_integer = BOGUS;
  (*int_value) = (int) (double) (*int_value);
  break;
      }

      return(term1);      
    }

    

    term1 = equals_not_equals(parse_domain,
        first_symbol, last_symbol, int_symbols,
        double_symbols,string_symbols,
        symbol_types, is_integer, int_value, error);
          

    if (string_symbols[*first_symbol][0] isnt ')') {
      bomb("Unmatched parentheses\n");
      return(-1.0);
    }

    (*first_symbol)++;      
  }
  else term1 = constant_or_variable(parse_domain,
          first_symbol, last_symbol, int_symbols,
          double_symbols,string_symbols,
          symbol_types, is_integer, int_value, error);
          

  return(term1);
}













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
)
{
	
	double value;                         
	int new_error;      
	

	if (symbol_types[*first_symbol] is DOUBLE_TOK) 
	{
		(*int_value) = (int) double_symbols[*first_symbol];
		value = double_symbols[*first_symbol];
		(*is_integer) = BOGUS;
	}
	else if (symbol_types[*first_symbol] is INT_TOK) 
	{
		(*int_value) = int_symbols[*first_symbol];
		value = (double) int_symbols[*first_symbol];
		(*is_integer) = EXCELLENT;
	}
	else if (symbol_types[*first_symbol] is STR_TOK)
	{

		
		

		
		
		new_error = IPARSE_DONT_REPORT_ERROR;

		value = parse_domain->Get_Runtime_Double ( 
		string_symbols[*first_symbol],
		&new_error);

		if (new_error is IPARSE_NO_ERR) 
		{
			*is_integer = BOGUS;
			*int_value = (int) value;
		}

		if (new_error is WRONG_TYPE) 
		{
			
			
			new_error = IPARSE_DONT_REPORT_ERROR;

			value = (double) parse_domain->Get_Runtime_Float ( 
										string_symbols[*first_symbol],
										&new_error);
			if (new_error is IPARSE_NO_ERR) 
			{
				*is_integer = BOGUS;
				*int_value = (int) value;
			}

			if (new_error is WRONG_TYPE) 
			{
				
				
				new_error = IPARSE_DONT_REPORT_ERROR;

				*int_value = parse_domain->Get_Runtime_Int ( 
				string_symbols[*first_symbol],
				&new_error);

				if (new_error is IPARSE_NO_ERR) 
				{
					*is_integer = EXCELLENT;
					value = (double) *int_value;
				}

				if (new_error is WRONG_TYPE) 
				{
					(*error) = VARIABLE_NOT_FOUND;
					bomb("Variable not declared");
					return(-1.0);
				}

			} 

		} 

	} 

	ADVANCE_TOKEN;

	return(value);

}







