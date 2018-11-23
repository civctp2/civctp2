#include "command.h"

CommandLineParser::CommandLineParser()
{

  Usage		= NULL;

  Options	= NULL;
  XOption	= NULL;
}

CommandLineParser::~CommandLineParser()
{
  ResetOptions();
}

void
CommandLineParser::SetUsage(void (*UsageFunc)())
{
  Usage = UsageFunc;
}

void
CommandLineParser::ResetOptions()
{
  __CBS *options=Options;

  while(Options!=NULL)
  {
	 options = Options->next;
	 delete Options;
	 Options = options;
  }

  if(XOption)
	 delete XOption;

  XOption = NULL;

  Usage   = NULL;
}

void
CommandLineParser::AddCommandOption(char *Key,unsigned flags,bool (*ActionFunc)(char *,int len))
{

	if(ActionFunc==NULL)
		return;

	__CBS *new_option=new __CBS;

	new_option->Value.ActionFunc	= ActionFunc;
	new_option->flags				= flags;

	if(Key==NULL)
		new_option->CommandType = CT_USE_FUNCTION_NULL;
	else
	{
		new_option->CommandType = CT_USE_FUNCTION;
		new_option->Key			= strdup(Key);
	}

	AddCommandOption(new_option);
}

void
CommandLineParser::AddCommandOption(char *Key,unsigned flags,bool *flag)
{

	if((Key==NULL)||(flag==NULL))
		return;

	__CBS *new_option=new __CBS;

	new_option->CommandType	= CT_USE_FLAG;
	new_option->Key			= strdup(Key);
	new_option->Value.flag	= flag;
	new_option->flags		= flags;

	AddCommandOption(new_option);
}

void
CommandLineParser::AddCommandOption(char *Key,unsigned flags,int *val)
{

	if((Key==NULL)||(val==NULL))
		return;

	__CBS *new_option=new __CBS;

	new_option->CommandType	= CT_USE_INT;
	new_option->Key			= strdup(Key);
	new_option->Value.ival  = val;
	new_option->flags		= flags;

	AddCommandOption(new_option);
}

void
CommandLineParser::AddCommandOption(char *Key,unsigned flags,float *val)
{

	if((Key==NULL)||(val==NULL))
		return;

	__CBS *new_option=new __CBS;

	new_option->CommandType	= CT_USE_FLOAT;
	new_option->Key			= strdup(Key);
	new_option->Value.fval	= val;
	new_option->flags		= flags;

	AddCommandOption(new_option);
}

void
CommandLineParser::AddCommandOption(char *Key,unsigned flags,char **val)
{

	if((Key==NULL)||(val==NULL))
		return;

	__CBS *new_option=new __CBS;

	new_option->CommandType	= CT_USE_STRING;
	new_option->Key			= strdup(Key);
	new_option->Value.sval  = val;
	new_option->flags		= flags;

	AddCommandOption(new_option);
}


void
CommandLineParser::AddCommandOption(__CBS *new_option)
{

	if(new_option==NULL)
		return;

	if(new_option->Key==NULL)
	{
	   if(XOption!=NULL)
		  delete XOption;

	   XOption = new_option;

	   return;
	}
	else
	 new_option->KeyLen = strlen(new_option->Key);

	new_option->next= Options;
	Options			= new_option;
}


__CBS *
CommandLineParser::LookupCommandOption(char *str)
{

	if(str==NULL)
	   return NULL;

	__CBS	*cur=Options;
	int		test=true;

	while(cur!=NULL)
	{
		if(cur->Key!=NULL)
		{
			if(cur->flags & CTFLAGS_NOCASE)
               test = _strnicmp(cur->Key,str,cur->KeyLen);
			else
			   test = strncmp  (cur->Key,str,cur->KeyLen);
		}

		if(!test)
		   return cur;

		cur=cur->next;
	}

	return NULL;
}

bool
CommandLineParser::ExecuteOption(__CBS *option,char *buff)
{

	if(option==NULL)
		return false;

  	int  len=0;
  	bool retval = false;

  	if(buff!=NULL)
  	{
  		 len   = strlen(buff)-option->KeyLen;
  		 buff += option->KeyLen;
  	}

  	switch(option->CommandType)
  	{
  	case	CT_NOP              :
  			return true;
  	case	CT_USE_FUNCTION 	:
  	case	CT_USE_FUNCTION_NULL:
  			retval = option->Value.ActionFunc(buff,len);
  			break;
  	case	CT_USE_FLAG 		:
  			retval = AssignFlag(option,buff,len);
  		    break;
  	case	CT_USE_INT			:
  			retval = AssignInt(option,buff,len);
  			break;
  	case	CT_USE_FLOAT		:
  			retval = AssignFloat(option,buff,len);
  			break;
  	case	CT_USE_STRING		:
  			retval = AssignString(option,buff,len);
  			break;
  	default:
  		    return false;
  	}

  	return retval;
}







bool
CommandLineParser::AssignFlag	(__CBS *option,char *buf,int len)
{

  bool  val=true;

  if(option->flags & CTFLAGS_INVERT)
	 val = false;

  *(option->Value.flag) = val;

  return true;
}

bool
CommandLineParser::AssignInt	(__CBS *option,char *buf,int len)
{

  if(!IsStringLegal(buf,len))
	 return false;

  *(option->Value.ival) = atoi(buf);

  return true;
}

bool
CommandLineParser::AssignFloat	(__CBS *option,char *buf,int len)
{

  if(!IsStringLegal(buf,len))
	 return false;

  *(option->Value.fval) = (float)atof(buf);

  return true;

}

bool
CommandLineParser::AssignString(__CBS *option,char *buf,int len)
{

  if(!IsStringLegal(buf,len))
	 return false;
  *(option->Value.sval) = strdup(buf);

  return true;
}

bool
CommandLineParser::IsStringLegal(char *buf,int len)
{
  if(buf==NULL)
	 return false;

  if(len<=0)
	 return false;

  return true;
}


bool
CommandLineParser::Execute(char *command_line)
{
  return true;
}

bool
CommandLineParser::Execute(int num_args,char **command_line)
{
  if(num_args<=0)
	 return false;

  __CBS	 *option;

  for(int i=0;i<num_args;i++)
  {
	 option = LookupCommandOption(command_line[i]);

	 if(option==NULL)
		option=XOption;

  	 if(!ExecuteOption(option,command_line[i]))
		return false;
  }

  return true;
}
