

























#ifdef LOG
#undef LOG
#endif
#define LOG(log_parameters)									\
	{														\
		Log_Begin (__FILE__, __LINE__);						\
		Log_Middle log_parameters;							\
		Log_End();											\
	}











#ifdef LOG_INDIRECT
#undef LOG_INDIRECT
#endif
#define LOG_INDIRECT(file, line, log_parameters)			\
	{														\
		Log_Begin (file, line);								\
		Log_Middle log_parameters;							\
		Log_End();											\
	}
