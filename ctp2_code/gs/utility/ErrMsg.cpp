#include "c3.h"
#include "c3types.h"

#include "ErrMsg.h"

#ifdef LPRNT
FILE *lprint_fout;
#endif

ErrorMsg::ErrorMsg()
{

   Assert(0);
   str = "Generic Error";
}

ErrGSParse::ErrGSParse (char *str1)

{
   strcpy(msg, str1);
}

ErrGSParse::ErrGSParse (char *str1, char *str2)

{
   strcpy(msg, str1);
   strcat(msg, str2);
}

ErrGSParse::ErrGSParse (char *str1, char *str2, sint32 x)
{
   char tmp [2 * _MAX_PATH];

   strcpy(msg, str1);
   sprintf (tmp, str2, x);
   strcat(msg, tmp);
}

ErrGSParse::ErrGSParse (char *str1, char *str2, char *val1)

{
   char tmp [2 * _MAX_PATH];

   strcpy(msg, str1);
   sprintf (tmp, str2, val1);
   strcat(msg, tmp);
}

ErrGSParse::ErrGSParse (char *str1, char *str2, char *val1, char *val2)

{
   char tmp [2 * _MAX_PATH];

   strcpy(msg, str1);
   sprintf (tmp, str2, val1, val2);
   strcat(msg, tmp);
}









void ErrorMsg::display ()

{
   exit(0);
}
