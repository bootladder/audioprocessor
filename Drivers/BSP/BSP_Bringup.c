#include "BSP_Bringup.h"
//#include "My_UART.h"
#include "My_Logger.h"
#include "My_Audio.h"
#include "BSP_LED.h"


void My_BSP_Bringup(void)
{
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Off(LED_RED);
  BSP_LED_Off(LED_GREEN);

  //My_UART_Init();

  My_BSP_Audio_Init();
}


