
void My_Logger_LogString(char * const str);
void My_Logger_LogStringLn(char * const str);
void My_Logger_LogLineNumber(int line);
void My_Logger_LogStringSpace(char * const str);
void My_Logger_LogFilenameAndLine(char * const str, int line);

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define RUN_AND_LOG(a) do{ My_Logger_LogString("\n"); My_Logger_LogFilenameAndLine( __FILENAME__ , __LINE__ ); My_Logger_LogStringLn(#a); a;} while(0);



typedef enum {
  MY_LOGGER_DMA2_STREAM_1_IRQ,
  MY_LOGGER_SYSTICK_HANDLER,
  MY_LOGGER_HAL_INCTICK,
} MY_LOGGER_ONESHOTS;


void My_Logger_SetOneTime(MY_LOGGER_ONESHOTS index, char * const str);

#define LOG_ONESHOT(s) \
  {static short happened = 0;                  \
  if(!happened) \
    My_Logger_LogStringLn(s); \
  happened = 1;}
