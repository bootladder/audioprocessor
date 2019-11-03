#include "MIDI_Input_Task.h"
#include "BSP_UART.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "My_Logger.h"

static void my_app_callback(uint8_t * buf);
static void Send_Hello_Message(void);

typedef struct {
  uint8_t type;
  uint8_t id;
  uint8_t value;
} MIDI_Message_t;

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

    //call command handler with MIDI_Message_t
    My_Logger_LogStringLn("Message Receievd");
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
  static uint8_t helloWorld[] = "MIDI Receiver Up:  115200 Baud \n";
  BSP_UART_Transmit_Bytes_Blocking(helloWorld, sizeof(helloWorld));
}
