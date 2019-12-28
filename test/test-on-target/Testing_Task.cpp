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
static void test_shortlengthcoefficient_firblock(void);

static void print_global_tick_count(void);

extern volatile int globalTickCount;

extern "C" void Testing_Task(void * params)
{
  (void)params;
  BSP_Fast_UART_Init();
  static uint8_t txbuf[] = "TEST Task Ready\n";
  BSP_Fast_UART_Transmit_Bytes_Blocking(txbuf, sizeof(txbuf));



  test_firblocks();

  test_shortlengthcoefficient_firblock();

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

sample_t short_test_buf[64];

class ShortLengthCoefficientTable : public CoefficientTable {

public:
  sample_t short_coeffs[8];

  ShortLengthCoefficientTable(){
    for(int i=0; i<8;i++){
      short_coeffs[i] = 1.0;
    }
  }

  sample_t * lookupCutoffFrequency(int freq){
    return short_coeffs;
  }
  int getNumTaps(){
    return 8;
  }
};

static void test_shortlengthcoefficient_firblock(void){
  ARMDSPFIRProcessor armdspfirp;
  ShortLengthCoefficientTable slct;
  FIRBlock  fir1("fir1", 64, armdspfirp, slct);
  fir1.setCutoffFrequency(10);

  for(int i=0; i<64;i++){
    short_test_buf[i] = 4;
  }

  fir1.process(short_test_buf);
  sample_t * outBuf = fir1.getOutputBuffer();

  //The outputBuffer shold be all 32 = 4*8, except the first 7 samples
  for(int i=0; i<64; i++){
    int myint = (int) outBuf[i];
    static char buf[100];
    int size = tfp_snprintf(buf, 100, "Hello: %d\n", myint);
    BSP_Fast_UART_Transmit_Bytes_Blocking((uint8_t *) buf, size);
  }
}

static void test_lowpass_firblock(void)
{
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
