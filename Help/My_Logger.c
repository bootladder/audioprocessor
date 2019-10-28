#include "My_Logger.h"
#include "tinyprintf.h"

#define BUFFER_SIZE 2048
static char log_buffer[BUFFER_SIZE];
static char * log_ptr = log_buffer;



void My_Logger_LogString(char * str)
{
  int bytesWritten = tfp_snprintf(log_ptr, BUFFER_SIZE, "%s", str);
  log_ptr += bytesWritten;
}

void My_Logger_LogStringLn(char * str)
{
  //don't allow writing past the buffer

  if(log_ptr >= log_buffer + BUFFER_SIZE){
    return;
  }
  int bytesWritten = tfp_snprintf(log_ptr, BUFFER_SIZE - (log_ptr - log_buffer), "%s\n", str);
  log_ptr += bytesWritten;
}

void My_Logger_LogFilenameAndLine(char * str, int line)
{
  //don't allow writing past the buffer

  if(log_ptr >= log_buffer + BUFFER_SIZE){
    return;
  }
  int bytesWritten = tfp_snprintf(log_ptr, BUFFER_SIZE - (log_ptr - log_buffer), "%s:%d ", str, line);
  log_ptr += bytesWritten;
}




char * logger_oneshotstrings[100];

void My_Logger_SetOneTime(MY_LOGGER_ONESHOTS index, char * str)
{
  logger_oneshotstrings[index] = str;
}
