#include "BSP_UART.h"
#include "stm32f7xx_hal.h"



static void BSP_UART_MspInit(void);

static UART_HandleTypeDef UartHandle;

void BSP_UART_Init(void)
{
  UartHandle.Instance        = UART5;

  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;

  BSP_UART_MspInit();


  if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
      ;
    }

  uint8_t txBuffer[] = "MIDI Receiver Up:  115200 Baud \n";

  if(HAL_UART_Transmit(&UartHandle, txBuffer, sizeof(txBuffer), 1000) != HAL_OK)
    {
    }
}


static void BSP_UART_MspInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();

  __UART5_CLK_ENABLE();

  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_12;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART5;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART5;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}
