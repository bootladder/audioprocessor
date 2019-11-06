#include "MemoryLogger.h"

#include "BSP_UART.h"
#include "stm32f7xx_hal.h"

#define STATIC_UART_RECEIVE_BUFFER_SIZE 256    //length is a uint8_t anyway, can't overflow

static void BSP_UART_MspInit(void);

static UART_HandleTypeDef UartHandle;
static BSP_UART_Receive_Callback_t _receive_callback;
static uint8_t myreceivebuf[STATIC_UART_RECEIVE_BUFFER_SIZE];


void BSP_UART_Receive_Bytes(uint8_t length, BSP_UART_Receive_Callback_t callback){
  _receive_callback = callback;
  HAL_UART_Receive_IT(&UartHandle, myreceivebuf, length);
}

void BSP_UART_Transmit_Bytes_Blocking(uint8_t * buf, uint16_t size)
{
  HAL_UART_Transmit(&UartHandle, buf, size, 1000);
}

// ISR Defined here!!!
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

  LOG_ONESHOT("UART RX COMPLETE CALLBACK");
  (void)huart;
  //ASSERT CALLBACK EXISTS???
  //CHECK FOR UART ERRORS?
  _receive_callback(myreceivebuf);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  (void)huart;
  LOG_ONESHOT("UART Error");
}

void UART5_IRQHandler(void){
  HAL_UART_IRQHandler(&UartHandle);
}

void BSP_UART_Init(void)
{
  UartHandle.Instance        = UART5;

  UartHandle.Init.BaudRate   = 9600;
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

  // Interrupts
  HAL_NVIC_SetPriority(UART5_IRQn, 7, 8); //priority, subpriority
  HAL_NVIC_EnableIRQ(UART5_IRQn);
}
