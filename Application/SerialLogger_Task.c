//app code calls this
#include "SerialLogger.h"

//main.c calls this
#include "SerialLogger_Task.h"

#include "FreeRTOS.h"
#include "queue.h"

#include "BSP_Fast_UART.h"


typedef struct {
  LogType_t type;
  uint8_t * msg;
  uint32_t size;
} SerialLoggerMessage_t;

SerialLoggerMessage_t receivedMessage;
static QueueHandle_t xQueue_SerialLoggerMessages;

void SerialLogger_Log(LogType_t type, uint8_t * str, uint32_t size)
{
  static SerialLoggerMessage_t txMsg;
  txMsg.type = type;
  txMsg.msg = str;
  txMsg.size = size;
  xQueueSend(xQueue_SerialLoggerMessages, &txMsg, 1000);
}



void SerialLogger_Task(void * params)
{
  (void)params;
  BSP_Fast_UART_Init();
  static uint8_t txbuf[] = "Serial Logger Task Ready\n";
  BSP_Fast_UART_Transmit_Bytes_Blocking(txbuf, sizeof(txbuf));

  xQueue_SerialLoggerMessages = xQueueCreate(32, sizeof(SerialLoggerMessage_t));

  while(1){
    //block until message availbale
    xQueuePeek(xQueue_SerialLoggerMessages, &receivedMessage, portMAX_DELAY);

    UBaseType_t num = uxQueueMessagesWaiting(xQueue_SerialLoggerMessages);
    for(unsigned i=0; i<num; i++){
      xQueueReceive(xQueue_SerialLoggerMessages, &receivedMessage, portMAX_DELAY);
      uint8_t typeBuf[1];
      typeBuf[0] = receivedMessage.type;
      BSP_Fast_UART_Transmit_Bytes_Blocking(typeBuf, 1);
      BSP_Fast_UART_Transmit_Bytes_Blocking(receivedMessage.msg, receivedMessage.size);
    }

  }
}
