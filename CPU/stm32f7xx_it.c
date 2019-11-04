#include "stm32f7xx_hal.h"
#include "MemoryLogger.h"
#include "BSP_LED.h"

extern void xPortSysTickHandler(void);


void SysTick_Handler(void)
{
  LOG_ONESHOT("SYSTICK HANDLER");
  HAL_IncTick();
  LOG_ONESHOT("INC TICK");
  xPortSysTickHandler();
  LOG_ONESHOT("FREERRTOS TICK");
}


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  BSP_LED_On(LED_RED);
  LOG_ONESHOT("\n\nHARD FAULT\n\n");
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @}
  */

/**
  * @}
  */
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
