#include "MemoryLogger.h"

#include "BSP_Fast_UART.h"
#include "stm32f7xx_hal.h"

static void BSP_Fast_UART_MspInit(void);

static UART_HandleTypeDef UartHandle;


void BSP_Fast_UART_Transmit_Bytes_Blocking(uint8_t * buf, uint16_t size)
{
  HAL_UART_Transmit(&UartHandle, buf, size, 1000);
}

void BSP_Fast_UART_Transmit_Bytes_NonBlocking(uint8_t * buf, uint16_t size)
{
  HAL_UART_Transmit_DMA(&UartHandle, buf, size);
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

  __HAL_RCC_DMA2_CLK_ENABLE();

  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_6;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_USART6;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  //not using Rx for now

  //dma
  static DMA_HandleTypeDef hdma_tx;
#define USARTx_TX_DMA_STREAM              DMA2_Stream6
#define USARTx_TX_DMA_CHANNEL             DMA_CHANNEL_5
#define USARTx_DMA_TX_IRQn                DMA2_Stream6_IRQn
#define USARTx_DMA_TX_IRQHandler          DMA2_Stream6_IRQHandler
#define USARTx_IRQn                      USART6_IRQn
#define USARTx_IRQHandler                USART6_IRQHandler

  hdma_tx.Instance                 = USARTx_TX_DMA_STREAM;
  hdma_tx.Init.Channel             = USARTx_TX_DMA_CHANNEL;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&hdma_tx);
  __HAL_LINKDMA(&UartHandle, hdmatx, hdma_tx);

  //// No interrupts for now
  //HAL_NVIC_SetPriority(USART6_IRQn, 7, 8); //priority, subpriority
  //HAL_NVIC_EnableIRQ(USART6_IRQn);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USART6_TX) */
  HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
}


#include "MemoryLogger.h"

void USARTx_DMA_TX_IRQHandler(void)
{
  LOG_ONESHOT("DMATXIRQ\n");
  HAL_DMA_IRQHandler(UartHandle.hdmatx);
}

void USARTx_IRQHandler(void)
{
  LOG_ONESHOT("USAERTTXIRQ\n");
  HAL_UART_IRQHandler(&UartHandle);
}

/**
 * @brief  Tx Transfer completed callback
 * @param  UartHandle: UART handle. 
 * @note   This example shows a simple way to report end of DMA Tx transfer, and 
 *         you can add your own implementation. 
 * @retval None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  (void)UartHandle;
  LOG_ONESHOT("UARTTXCPLTCALLBACK\n");
}
