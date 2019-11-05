#include "BSP_Audio_Task.h"

#include "BSP_Audio_Buffer_Interface.h"
#include "AudioProcessor.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "MemoryLogger.h"

#include "BSP_Fast_UART.h"

static void CopySampleBuffer(int16_t * dst, int16_t * src, uint32_t num_samples);

enum {
  BUFFER_STATUS_LOWER_HALF_FULL,
  BUFFER_STATUS_UPPER_HALF_FULL
};
typedef uint32_t BufferStatusMessage_t ;

static QueueHandle_t xQueue_BufferStatus;
static BufferStatusMessage_t bufferStatusMessage;

static int16_t recordBuffer[MY_BUFFER_SIZE_SAMPLES/2];

void My_Audio_Task(void * argument)
{
  (void)argument;
  RUN_AND_LOG( My_BSP_Audio_Init(); );

  RUN_AND_LOG( AudioProcessor_Init() );

  xQueue_BufferStatus = xQueueCreate(32, sizeof(BufferStatusMessage_t));

  //temporarily init here..  move this somewhere else
  BSP_Fast_UART_Init();
  static uint8_t txbuf[] = "hurrrr durrrrrr";
  BSP_Fast_UART_Transmit_Bytes_Blocking(txbuf, sizeof(txbuf));

  while (1)
  {
    xQueueReceive( xQueue_BufferStatus, &bufferStatusMessage, 1000 );
    LOG_ONESHOT("RECEIVED QUEUE ITEM");

    switch(bufferStatusMessage)
    {
    case BUFFER_STATUS_LOWER_HALF_FULL:
      {
        ExtractSamplesFromDMAReceiveBuffer_LowerHalf(recordBuffer,
                                                     MY_BUFFER_SIZE_SAMPLES / 2);

        int16_t * outBuf = AudioProcessor_ProcessSampleBuffer(recordBuffer,
                                                              MY_BUFFER_SIZE_SAMPLES / 2);

        InsertSamplesIntoDMATransmitBuffer_LowerHalf(outBuf,
                                                     MY_BUFFER_SIZE_SAMPLES / 2);
        break;
      }
    case BUFFER_STATUS_UPPER_HALF_FULL:
      {
        ExtractSamplesFromDMAReceiveBuffer_UpperHalf(recordBuffer,
                                                     MY_BUFFER_SIZE_SAMPLES / 2);

        int16_t * outBuf = AudioProcessor_ProcessSampleBuffer(recordBuffer,
                                                              MY_BUFFER_SIZE_SAMPLES / 2);

        InsertSamplesIntoDMATransmitBuffer_UpperHalf(outBuf,
                                                     MY_BUFFER_SIZE_SAMPLES / 2);
        break;
      }
    }
  }
}


static void CopySampleBuffer(int16_t * dst, int16_t * src, uint32_t num_samples)
{
  for(uint32_t i=0; i < num_samples; i++){
    dst[i] = src[i];
  }
}



// DMA receive complete ISRs

void My_AUDIO_IN_TransferComplete_CallBack(void)
{
  LOG_ONESHOT("AUDIO IN COMPLETE");
  static BufferStatusMessage_t msg = BUFFER_STATUS_UPPER_HALF_FULL;
  static BaseType_t higherPriorityTaskWoken = 0;
  xQueueSendFromISR( xQueue_BufferStatus, &msg, &higherPriorityTaskWoken );
}

void My_AUDIO_IN_HalfTransfer_CallBack(void)
{
  LOG_ONESHOT("AUDIO IN HALF");
  static BufferStatusMessage_t msg = BUFFER_STATUS_LOWER_HALF_FULL;
  static BaseType_t higherPriorityTaskWoken = 0;
  xQueueSendFromISR( xQueue_BufferStatus, &msg, &higherPriorityTaskWoken );
}

void My_AUDIO_OUT_Error_CallBack(void){
}

