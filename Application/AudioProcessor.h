#include <stdint.h>
#include "SamplingTypes.h"

int16_t * AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples);

void AudioProcessor_Init(void);

char * AudioProcessor_GetActiveBlockGraphEdgeListToString(void);
sample_t * AudioProcessor_GetFFTSpectrum(void);
