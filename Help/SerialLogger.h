#include <stdint.h>

#define SerialLogger_PrintLiteralString(a) do{static uint8_t buf[] = a; SerialLogger_PrintBytes(buf, sizeof(buf));} while(0);

void SerialLogger_PrintBytes(uint8_t * str, uint32_t size);
