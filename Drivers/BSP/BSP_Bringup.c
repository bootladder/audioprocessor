#include "BSP_Bringup.h"
#include "BSP_UART.h"
#include "My_Logger.h"
#include "BSP_Audio_Init.h"
#include "BSP_LED.h"


void My_BSP_Bringup(void)
{
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Off(LED_RED);
  BSP_LED_Off(LED_GREEN);

  BSP_UART_Init();
}


