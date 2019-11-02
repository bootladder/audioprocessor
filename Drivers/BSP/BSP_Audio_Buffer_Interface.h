#ifndef __MY_AUDIO_BUFFER_INTERFACE__H__
#define __MY_AUDIO_BUFFER_INTERFACE__H__

#include <stdint.h>

#define MY_BUFFER_SIZE_SAMPLES 1024

//used by the AudioProcessor
#define MY_PROCESSING_BUFFER_SIZE_SAMPLES MY_BUFFER_SIZE_SAMPLES / 2

void ExtractSamplesFromDMAReceiveBuffer_LowerHalf(int16_t * sampleBuffer, uint32_t num_samples);
void ExtractSamplesFromDMAReceiveBuffer_UpperHalf(int16_t * sampleBuffer, uint32_t num_samples);
void InsertSamplesIntoDMATransmitBuffer_LowerHalf(int16_t * sampleBuffer, uint32_t num_samples);
void InsertSamplesIntoDMATransmitBuffer_UpperHalf(int16_t * sampleBuffer, uint32_t num_samples);

void My_AUDIO_IN_TransferComplete_CallBack(void);
void My_AUDIO_IN_HalfTransfer_CallBack(void);
void My_AUDIO_OUT_Error_CallBack(void);

#endif
