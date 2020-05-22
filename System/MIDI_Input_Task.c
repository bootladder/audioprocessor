#include "MIDI_Input_Task.h"

#include "MIDIMessageHandler.h"

#include "BSP_UART.h" // for MIDI input
#include "FreeRTOS.h"
#include "queue.h"
#include "MemoryLogger.h"
#include "SerialLogger.h"

static void my_app_callback(uint8_t * buf);
static void Send_Hello_Message(void);

static QueueHandle_t xQueue_MIDIMessages;
static MIDI_Message_t received_MIDI_message;

void MIDI_Input_Task(void * argument)
{
  (void)argument;

  BSP_UART_Init();
  Send_Hello_Message();

  //Receive 3 bytes forever (callback receives more bytes)
  BSP_UART_Receive_Bytes(3, my_app_callback);

  xQueue_MIDIMessages = xQueueCreate(32, sizeof(MIDI_Message_t));

  while(1){
    xQueueReceive( xQueue_MIDIMessages, &received_MIDI_message, portMAX_DELAY );
    MIDIMessageHandler_Handle(received_MIDI_message);
  }
}


void my_app_callback(uint8_t * buf){

  LOG_ONESHOT("midi received app callback");

  static MIDI_Message_t msg;
  msg.type  = buf[0];
  msg.id    = buf[1];
  msg.value = buf[2];

  static BaseType_t higherPriorityTaskWoken = 0;
  xQueueSendFromISR( xQueue_MIDIMessages, &msg, &higherPriorityTaskWoken );

  BSP_UART_Receive_Bytes(3, my_app_callback);
}


static void Send_Hello_Message(void){
  //freaking queue isn't up yet
  //SerialLogger_LogLiteralString(LOGTYPE_EVENT, "MIDI Receiver Up:  115200 Baud \n");
}
