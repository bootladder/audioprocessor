//remember this file has C linkage

#include "SerialLogger.h"

#include "BSP_Fast_UART.h"


void SerialLogger_PrintBytes(uint8_t * str, uint32_t size)
{
  BSP_Fast_UART_Transmit_Bytes_Blocking(str, size);
}
