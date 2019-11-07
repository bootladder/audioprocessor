
#include <stdint.h>

typedef enum{
  LOGTYPE_EVENT = 'L',
} LogType_t;

#define Monitor_LogLiteralString(t, a) do{static uint8_t buf[] = a; Monitor_Log(t, buf, sizeof(buf));} while(0);

void Monitor_Log(LogType_t type, uint8_t * str, uint32_t size);


void Monitor_Task(void * argument);


