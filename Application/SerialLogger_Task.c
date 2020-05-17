//app code calls this
#include "SerialLogger.h"

//main.c calls this
#include "SerialLogger_Task.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "tinyprintf.h"
#include "BSP_Fast_UART.h"

typedef struct {
  LogType_t type;
  uint8_t * msg;
  uint32_t size;
} SerialLoggerMessage_t;

static SerialLoggerMessage_t receivedMessage;
static QueueHandle_t xQueue_SerialLoggerMessages;
static QueueHandle_t signalSemaphore;

#define TXBUFSIZE 1024*10
static uint8_t bulkTransmitBuffer[TXBUFSIZE];

static void _send_hello_message(void);


void SerialLogger_Log(LogType_t type, uint8_t * str, uint32_t size)
{
  static SerialLoggerMessage_t txMsg;
  txMsg.type = type;
  txMsg.msg = str;
  txMsg.size = size;
  xQueueSend(xQueue_SerialLoggerMessages, &txMsg, 1000);
}

void SerialLogger_Signal(){
  xSemaphoreGive(signalSemaphore);
}



#include "BSP_SDRAM.h"
void SerialLogger_Task(void * params)
{
  BSP_SDRAM_Init();

  (void)params;
  BSP_Fast_UART_Init();
  _send_hello_message();

  vSemaphoreCreateBinary(signalSemaphore );
  xQueue_SerialLoggerMessages = xQueueCreate(32, sizeof(SerialLoggerMessage_t));

  while(1){

    //block until signaled by audio task
    //xQueuePeek(xQueue_SerialLoggerMessages, &receivedMessage, portMAX_DELAY);
    xSemaphoreTake(signalSemaphore, portMAX_DELAY);

    UBaseType_t num = uxQueueMessagesWaiting(xQueue_SerialLoggerMessages);
    int index = 0;
    for(unsigned i=0; i<num; i++){
      xQueueReceive(xQueue_SerialLoggerMessages, &receivedMessage, portMAX_DELAY);

      bulkTransmitBuffer[index] = receivedMessage.type;
      index++;
      int size = tfp_snprintf((char*)&bulkTransmitBuffer[index], TXBUFSIZE - index,
                              (char*)receivedMessage.msg, receivedMessage.size);
      index += size;

    }

    BSP_Fast_UART_Transmit_Bytes_NonBlocking(bulkTransmitBuffer, index);
  }
}

static void _send_hello_message(void){
  static uint8_t txbuf[] = "Serial Logger Task Ready\n";
  BSP_Fast_UART_Transmit_Bytes_Blocking(txbuf, sizeof(txbuf));
}
