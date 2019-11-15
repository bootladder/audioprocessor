#ifndef __SERIAL_LOGGER_H__
#define __SERIAL_LOGGER_H__

#include <stdint.h>

typedef enum{
  LOGTYPE_EVENT = 'L',
  LOGTYPE_IDLE_MONITOR = 'I',
  LOGTYPE_MIDI_MESSAGE_PROCESSED = 'M',
  LOGTYPE_BLOCKGRAPH_NODE_UPDATE = 'B',
  LOGTYPE_BLOCKGRAPH_EDGELIST_UPDATE = 'E',
} LogType_t;


//cannot use these in if else statements that don't have curly braces
#define SerialLogger_LogLiteralString(t, a) do{static uint8_t buf[] = a; SerialLogger_Log(t, buf, sizeof(buf));} while(0);

void SerialLogger_Log(LogType_t type, uint8_t * str, uint32_t size);


#endif
