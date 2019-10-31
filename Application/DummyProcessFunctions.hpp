#include <stdint.h>

int16_t * DummyProcessFunctions_noop(int16_t * buf, uint32_t num_samples);
int16_t * DummyProcessFunctions_half_volume(int16_t * buf, uint32_t num_samples);
int16_t * DummyProcessFunctions_add_1(int16_t * buf, uint32_t num_samples);
int16_t * DummyProcessFunctions_add_2(int16_t * buf, uint32_t num_samples);
int16_t * DummyProcessFunctions_Constant_5(int16_t * buf, uint32_t num_samples);
int16_t * DummyProcessFunctions_Constant_7(int16_t * buf, uint32_t num_samples);
int16_t * DummyProcessFunctions_mixer_addition(int16_t * buf, uint32_t num_samples);
