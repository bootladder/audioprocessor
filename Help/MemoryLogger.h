#ifndef __MEMORYLOGGER_H__
#define __MEMORYLOGGER_H__

void MemoryLogger_LogString(const char * const str);
void MemoryLogger_LogStringLn(const char * const str);
void MemoryLogger_LogLineNumber(int line);
void MemoryLogger_LogFilenameAndLine(const char * const str, int line);

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define RUN_AND_LOG(a) do{ MemoryLogger_LogString("\n"); MemoryLogger_LogFilenameAndLine( __FILENAME__ , __LINE__ ); MemoryLogger_LogStringLn(#a); a;} while(0);

#define LOG_ONESHOT(s) \
  {static short happened = 0;                  \
  if(!happened) \
    MemoryLogger_LogStringLn(s); \
  happened = 1;}


#endif
