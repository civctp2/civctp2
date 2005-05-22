










#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ERR_MSG_H__
#define __ERR_MSG_H__ 1



class ErrorMsg { 

   uint32 val; 
   
public:
   char *str; 

   ErrorMsg();
   ErrorMsg(sint32 v); 
   void display(); 
   char * msg() { return str; }; 
}; 

class ErrGSParse : public ErrorMsg{
   
   char msg[2 * _MAX_PATH];
public:
   
   ErrGSParse (char *str); 
   ErrGSParse (char *str1, char *str2, sint32 x); 
   ErrGSParse (char *str1, char *str2); 
   ErrGSParse (char *str1, char *str2, char *val1); 
   ErrGSParse (char *str1, char *str2, char *val1, char *val2); 

};

class ErrMemory : public ErrorMsg 
{

public:
   ErrMemory() { str = "Out of memory";  }; 
};

#ifdef _DEBUG
#define LPRNT 1
#endif

#ifdef LPRNT
extern FILE *lprint_fout; 
extern int turn_count; 
extern int my_team; 
extern char *szAppName; 
#endif

#ifdef LPRNT
#define LPRINT_INIT lprint_fout=fopen("civlog.txt","w");fprintf(lprint_fout,"%s\n",szAppName);fclose(lprint_fout);  
#else
#define LPRINT_INIT
#endif

#ifdef LPRNT
#define LSTART lprint_fout=fopen("civlog.txt","a");fprintf(lprint_fout," %3d-%d:%12s %4d: ",turn_count, my_team, strrchr(__FILE__,'\\')+1,__LINE__);
#define LEND fprintf(lprint_fout,"\n");fclose(lprint_fout);
#endif

#ifdef LPRNT
#define LPRINT0(lprint_string)  LSTART;fprintf(lprint_fout,lprint_string);LEND
#else
#define LPRINT0(lprint_string)
#endif

#ifdef LPRNT
#define LPRINT1(lprint_string, lprint_first)  LSTART;fprintf(lprint_fout,lprint_string,lprint_first);LEND
#else
#define LPRINT1(lprint_string, lprint_first)
#endif

#ifdef LPRNT
#define LPRINT2(lprint_string, lprint_first, lprint_second)  LSTART;fprintf(lprint_fout,lprint_string,lprint_first, lprint_second);LEND
#else
#define LPRINT2(lprint_string, lprint_first, lprint_second)
#endif


#ifdef LPRNT
#define LPRINT3(lprint_string, lprint_first, lprint_second, lprint_third)  LSTART;fprintf(lprint_fout,lprint_string,lprint_first, lprint_second, lprint_third);LEND
#else
#define LPRINT3(lprint_string, lprint_first, lprint_second, lprint_third)
#endif


#ifdef LPRNT
#define LPRINT4(lprint_string, lprint_first, lprint_second, lprint_third, lprint_fourth)  LSTART;fprintf(lprint_fout,lprint_string,lprint_first, lprint_second, lprint_third, lprint_fourth);LEND
#else
#define LPRINT4(lprint_string, lprint_first, lprint_second, lprint_third, lprint_fourth)
#endif

#endif 
