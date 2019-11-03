#include "MIDI_Input_Task.h"
#include "BSP_UART.h"

static void my_app_callback(uint8_t * buf);
static uint8_t dummyEchoTxBuffer[3];
static int flag = 0;

void MIDI_Input_Task(void * argument)
{
  (void)argument;
  BSP_UART_Init();
  static uint8_t helloWorld[] = "MIDI Receiver Up:  115200 Baud \n";
  BSP_UART_Transmit_Bytes_Blocking(helloWorld, sizeof(helloWorld));

  // MOVE THIS UP TO APP LEVEL!!!
  BSP_UART_Receive_Bytes(3, my_app_callback);
  while(1){

    if(flag){
      flag = 0;
      //BSP_UART_Transmit_Bytes_Blocking(dummyEchoTxBuffer, sizeof(dummyEchoTxBuffer));
    }
  }
}


void my_app_callback(uint8_t * buf){

  for(int i=0; i<3; i++){
    dummyEchoTxBuffer[i] = buf[i];
  }

  flag = 1;
  BSP_UART_Receive_Bytes(3, my_app_callback);
}


