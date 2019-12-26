extern "C"{
#include "BSP_Fast_UART.h"
#include "tinyprintf.h"
#include "stm32f7xx_hal.h"
}
#include "ARMDSPFIRProcessor.hpp"
#include "CircularFIRProcessor.hpp"
#include "LowPassCoefficientTable.hpp"

static void test_firblocks(void);
static void test_lowpass_firblock(void);

static void print_global_tick_count(void);

extern volatile int globalTickCount;

extern "C" void Testing_Task(void * params)
{
  (void)params;
  BSP_Fast_UART_Init();
  static uint8_t txbuf[] = "TEST Task Ready\n";
  BSP_Fast_UART_Transmit_Bytes_Blocking(txbuf, sizeof(txbuf));



  test_firblocks();

  test_lowpass_firblock();


  static uint8_t txbuf2[] = "TEST Task DONE\n";
  BSP_Fast_UART_Transmit_Bytes_Blocking(txbuf2, sizeof(txbuf2));
  while(1){
  }
}

// Testing framework
//////////////////////////////////////////////////////////
// Tests

#include "FIRBlock.hpp"
#include "DelayBlock.hpp"

#define TEST_LENGTH 1024
sample_t testbuf[TEST_LENGTH];
static void test_firblocks(void)
{
  static sample_t coeffValues[512];
  static FIRCoefficients coeffs(coeffValues, 512);

  static ARMDSPFIRProcessor armdspfirp;
  static FakeCoefficientTable fakeCoefficientTable;
  static FIRBlock fir1("name", TEST_LENGTH, armdspfirp, fakeCoefficientTable);

  static DelayBuffer delayBuffer(1024);
  static CircularFIRProcessor cfirp(delayBuffer);
  static FIRBlock fir2("name", TEST_LENGTH, cfirp, fakeCoefficientTable);

  for(int i=0;i<TEST_LENGTH;i++){
    testbuf[i] = 10;
  }

  print_global_tick_count();
  fir1.process(testbuf);

  print_global_tick_count();
  fir2.process(testbuf);

  print_global_tick_count();
}

static void test_lowpass_firblock(void){
  LowPassCoefficientTable lowPassCoefficientTable;
}


// Tests
//////////////////////////////////////////////////////////
// Testing framework

static void print_global_tick_count(void)
{
  static char buf[100];
  int size = tfp_snprintf(buf, 100, "Tick Count: %d\n", globalTickCount);
  BSP_Fast_UART_Transmit_Bytes_Blocking((uint8_t *) buf, size);
}
