#include "SerialLogger.h"

#include <stdint.h>
#include <stdio.h>
void SerialLogger_Log(LogType_t type, uint8_t * str, uint32_t size)
{
  (void)type;
  (void)str;
  (void)size;
  fwrite(str, sizeof(uint8_t), size, stdout);
}
