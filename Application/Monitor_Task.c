#include "Monitor_Task.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "BSP_Fast_UART.h"

typedef struct {
  LogType_t type;
  uint8_t * msg;
  uint32_t size;
} MonitorMessage_t;

MonitorMessage_t receivedMessage;
static QueueHandle_t xQueue_MonitorMessages;

void Monitor_Log(LogType_t type, uint8_t * str, uint32_t size)
{
  static MonitorMessage_t txMsg;
  txMsg.type = type;
  txMsg.msg = str;
  txMsg.size = size;
  xQueueSend(xQueue_MonitorMessages, &txMsg, 1000);
}


void Monitor_Task(void * argument)
{
  (void)argument;

  BSP_Fast_UART_Init();
  static uint8_t txbuf[] = "Monitor Task Ready\n";
  BSP_Fast_UART_Transmit_Bytes_Blocking(txbuf, sizeof(txbuf));

  xQueue_MonitorMessages = xQueueCreate(32, sizeof(MonitorMessage_t));

  while(1){
    xQueueReceive(xQueue_MonitorMessages, &receivedMessage, portMAX_DELAY);

    uint8_t typeBuf[1];
    typeBuf[0] = receivedMessage.type;
    BSP_Fast_UART_Transmit_Bytes_Blocking(typeBuf, 1);
    BSP_Fast_UART_Transmit_Bytes_Blocking(receivedMessage.msg, receivedMessage.size);
  }
}
