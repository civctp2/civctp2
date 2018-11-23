void gslog_LogPlayerStats(sint32 player);
void gslog_dipprint(const char *fmt, ...)
#ifdef __GNUC__
	__attribute__((__format__ (__printf__, 1, 2)))
#endif
	;
