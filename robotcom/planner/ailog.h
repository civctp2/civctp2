#ifndef __AILOG_H__
#define __AILOG_H__


#define LOG(v) 

#define AILOG(x) { char wstr[500]; sprintf x; if (ai != NULL) ai->Log(wstr); }

#endif  
