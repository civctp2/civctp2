





#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum COMMAND_TYPE
{
  CT_NOP,
  CT_USE_FUNCTION,	   
  CT_USE_FUNCTION_NULL,
  CT_USE_FLAG,		   
  CT_USE_INT,		   
  CT_USE_FLOAT,		   
  CT_USE_STRING		   
};		   


#define CTFLAGS_NOP			0x00000000 
#define CTFLAGS_WARN		0x00000001 
#define CTFLAGS_ERROR		0x00000002 
#define CTFLAGS_REQUIRED	0x00000004 
#define CTFLAGS_NOCASE		0x00000008 
#define CTFLAGS_INVERT		0x00000010 

	
	typedef struct command_block_struct
	{
		COMMAND_TYPE  CommandType;
  		unsigned      flags;
  		char         *Key;
  		int		      KeyLen;
  		struct        command_block_struct	*next;
  		
  		union
  		{
  			bool (*ActionFunc)(char *,int len);
  			bool  *flag;              
  			int   *ival;              
  			float *fval;              
  			char **sval;              
  		
  		} Value;
  		
  		command_block_struct()
  		{
  			CommandType	= CT_NOP;
  			flags		= CTFLAGS_NOP;
  			Key			= NULL;
  			KeyLen		= 0;
  			Value.ival	= NULL;
  		};
  		
	} __CBS;




class CommandLineParser
{

protected:
public:

   	
   	CommandLineParser();
   ~CommandLineParser();

	
	void SetUsage(void (*UsageFunc)());

	
   	void AddCommandOption(char *Key,unsigned flags,bool (*ActionFunc)(char *,int len));
   	void AddCommandOption(char *Key,unsigned flags,bool *flag);
   	void AddCommandOption(char *Key,unsigned flags,int   *val);
   	void AddCommandOption(char *Key,unsigned flags,float *val);
   	void AddCommandOption(char *Key,unsigned flags,char **val);

	
   	bool Execute(char *command_line);
   	bool Execute(int num_args,char **command_line);

	
	void ResetOptions();

protected:

	
   	void		AddCommandOption(__CBS *);

	
	__CBS		*LookupCommandOption(char *str);

	
	bool		 ExecuteOption(__CBS *option,char *buf);

	
	bool		AssignFlag	(__CBS *option,char *buf,int len);
    bool		AssignInt	(__CBS *option,char *buf,int len);
	bool		AssignFloat	(__CBS *option,char *buf,int len);
	bool		AssignString(__CBS *option,char *buf,int len);

	
	bool		IsStringLegal(char *buf,int len);

	
	void        (*Usage)();
	__CBS		*Options;		
	__CBS		*XOption;		

private:

};