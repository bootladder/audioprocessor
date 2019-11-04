#include "MemoryLogger.h"
#include "tinyprintf.h"

#define BUFFER_SIZE 1024*16  //large buffer size here
static char log_buffer[BUFFER_SIZE];
static char * log_ptr = log_buffer;



void MemoryLogger_LogString(char * const str)
{
  int bytesWritten = tfp_snprintf(log_ptr, BUFFER_SIZE, "%s", str);
  log_ptr += bytesWritten;
}

void MemoryLogger_LogStringLn(char * const str)
{
  //don't allow writing past the buffer

  if(log_ptr >= log_buffer + BUFFER_SIZE){
    return;
  }
  int bytesWritten = tfp_snprintf(log_ptr, BUFFER_SIZE - (log_ptr - log_buffer), "%s\n", str);
  log_ptr += bytesWritten;
}

void MemoryLogger_LogFilenameAndLine(char * const str, int line)
{
  //don't allow writing past the buffer

  if(log_ptr >= log_buffer + BUFFER_SIZE){
    return;
  }
  int bytesWritten = tfp_snprintf(log_ptr, BUFFER_SIZE - (log_ptr - log_buffer), "%s:%d ", str, line);
  log_ptr += bytesWritten;
}




char * logger_oneshotstrings[100];

void MemoryLogger_SetOneTime(MY_LOGGER_ONESHOTS index, char * const str)
{
  logger_oneshotstrings[index] = str;
}
