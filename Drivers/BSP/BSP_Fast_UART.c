#include "MemoryLogger.h"

#include "BSP_Fast_UART.h"
#include "stm32f7xx_hal.h"

static void BSP_Fast_UART_MspInit(void);

static UART_HandleTypeDef UartHandle;


void BSP_Fast_UART_Transmit_Bytes_Blocking(uint8_t * buf, uint16_t size)
{
  HAL_UART_Transmit(&UartHandle, buf, size, 1000);
}

//not using interrupts yet
//void USART6_IRQHandler(void){
//  HAL_UART_IRQHandler(&UartHandle);
//}

void BSP_Fast_UART_Init(void)
{
  UartHandle.Instance        = USART6;

  UartHandle.Init.BaudRate   = 921600;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;

  BSP_Fast_UART_MspInit();


  if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
      ;
    }
}


static void BSP_Fast_UART_MspInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  __GPIOC_CLK_ENABLE();  //lol.  for rx and tx, they both use the same port.
  __GPIOC_CLK_ENABLE();

  __USART6_CLK_ENABLE();

  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_6;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_USART6;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  //not using Rx for now

  //// No interrupts for now
  //HAL_NVIC_SetPriority(USART6_IRQn, 7, 8); //priority, subpriority
  //HAL_NVIC_EnableIRQ(USART6_IRQn);
}
