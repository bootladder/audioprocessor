extern "C"{
#include "BSP_Fast_UART.h"
#include "tinyprintf.h"
#include "stm32f7xx_hal.h"
}

static void test_firblocks(void);
static void print_global_tick_count(void);

extern volatile int globalTickCount;

extern "C" void Testing_Task(void * params)
{
  (void)params;
  BSP_Fast_UART_Init();
  static uint8_t txbuf[] = "TEST Task Ready\n";
  BSP_Fast_UART_Transmit_Bytes_Blocking(txbuf, sizeof(txbuf));


  print_global_tick_count();

  test_firblocks();

  print_global_tick_count();

  while(1){
  }
}

// Testing framework
//////////////////////////////////////////////////////////
// Tests

#include "FIRBlock.hpp"

#define TEST_LENGTH 1024
sample_t testbuf[TEST_LENGTH];
static void test_firblocks(void)
{
  ARMDSPFIRProcessor armdspfirp;
  FIRBlock fir1("name", TEST_LENGTH, armdspfirp);

  for(int i=0;i<TEST_LENGTH;i++){
    testbuf[i] = 3;
  }

  fir1.process(testbuf);
  fir1.process(testbuf);
  fir1.process(testbuf);
  fir1.process(testbuf);
  fir1.process(testbuf);

  CircularFIRProcessor cfirp;
  FIRBlock fir2("name", TEST_LENGTH, cfirp);

  fir2.process(testbuf);
  fir2.process(testbuf);
  fir2.process(testbuf);
  fir2.process(testbuf);
  fir2.process(testbuf);
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
