#include "BSP_Audio_Init.h"

#include "BSP_Audio_Buffer_Interface.h"

#include "stm32f7xx_hal.h"
#include "wm8994.h"
#include "My_Logger.h"

#define MY_DMA_BYTES_PER_FRAME 8
#define MY_DMA_BYTES_PER_MSIZE 2
#define MY_DMA_BUFFER_SIZE_BYTES MY_BUFFER_SIZE_SAMPLES * MY_DMA_BYTES_PER_FRAME
#define MY_DMA_BUFFER_SIZE_MSIZES MY_DMA_BUFFER_SIZE_BYTES / MY_DMA_BYTES_PER_MSIZE

static uint8_t saiDMATransmitBuffer[MY_DMA_BUFFER_SIZE_BYTES];
static uint8_t saiDMAReceiveBuffer[MY_DMA_BUFFER_SIZE_BYTES];

static void My_SAI_ClockConfig(uint32_t AudioFreq);
static void My_AUDIO_OUT_MspInit(void);
static void My_SAI_Out_Init(uint32_t AudioFreq);
static void My_AUDIO_IN_MspInit(void);
static void My_SAI_In_Init(uint32_t AudioFreq);

static SAI_HandleTypeDef         haudio_out_sai;
static SAI_HandleTypeDef         haudio_in_sai;

static uint32_t frequency = AUDIO_FREQUENCY_48K;
static uint8_t volume = 80;

///////////////////////////////////////////////////////////

#define AUDIO_OUT_SAIx_CLK_ENABLE()              __HAL_RCC_SAI1_CLK_ENABLE()
#define AUDIO_OUT_SAIx_CLK_DISABLE()             __HAL_RCC_SAI1_CLK_DISABLE()
#define AUDIO_OUT_SAIx_AF                        GPIO_AF6_SAI1

#define AUDIO_OUT_SAIx_MCLK_ENABLE()             __HAL_RCC_GPIOG_CLK_ENABLE()
#define AUDIO_OUT_SAIx_MCLK_GPIO_PORT            GPIOG
#define AUDIO_OUT_SAIx_MCLK_PIN                  GPIO_PIN_7
#define AUDIO_OUT_SAIx_SD_FS_CLK_ENABLE()        __HAL_RCC_GPIOE_CLK_ENABLE()
#define AUDIO_OUT_SAIx_SD_FS_SCK_GPIO_PORT       GPIOE
#define AUDIO_OUT_SAIx_FS_PIN                    GPIO_PIN_4
#define AUDIO_OUT_SAIx_SCK_PIN                   GPIO_PIN_5
#define AUDIO_OUT_SAIx_SD_PIN                    GPIO_PIN_6

/* SAI DMA Stream definitions */
#define AUDIO_OUT_SAIx_DMAx_CLK_ENABLE()         __HAL_RCC_DMA2_CLK_ENABLE()
#define AUDIO_OUT_SAIx_DMAx_STREAM               DMA2_Stream1
#define AUDIO_OUT_SAIx_DMAx_CHANNEL              DMA_CHANNEL_0
#define AUDIO_OUT_SAIx_DMAx_IRQ                  DMA2_Stream1_IRQn
#define AUDIO_OUT_SAIx_DMAx_PERIPH_DATA_SIZE     DMA_PDATAALIGN_HALFWORD
#define AUDIO_OUT_SAIx_DMAx_MEM_DATA_SIZE        DMA_MDATAALIGN_HALFWORD
#define DMA_MAX_SZE                              0xFFFF

#define AUDIO_OUT_SAIx_DMAx_IRQHandler           DMA2_Stream1_IRQHandler

/* Select the interrupt preemption priority and subpriority for the DMA interrupt */
#define AUDIO_OUT_IRQ_PREPRIO                    ((uint32_t)0x0E)


/* SAI peripheral configuration defines */
#define AUDIO_IN_SAIx                           SAI1_Block_B
#define AUDIO_IN_SAIx_CLK_ENABLE()              __HAL_RCC_SAI1_CLK_ENABLE()
#define AUDIO_IN_SAIx_CLK_DISABLE()             __HAL_RCC_SAI1_CLK_DISABLE()
#define AUDIO_IN_SAIx_AF                        GPIO_AF6_SAI1
#define AUDIO_IN_SAIx_SD_ENABLE()               __HAL_RCC_GPIOE_CLK_ENABLE()
#define AUDIO_IN_SAIx_SD_GPIO_PORT              GPIOE
#define AUDIO_IN_SAIx_SD_PIN                    GPIO_PIN_3

/* SAI DMA Stream definitions */
#define AUDIO_IN_SAIx_DMAx_CLK_ENABLE()         __HAL_RCC_DMA2_CLK_ENABLE()
#define AUDIO_IN_SAIx_DMAx_STREAM               DMA2_Stream4
#define AUDIO_IN_SAIx_DMAx_CHANNEL              DMA_CHANNEL_1
#define AUDIO_IN_SAIx_DMAx_IRQ                  DMA2_Stream4_IRQn
#define AUDIO_IN_SAIx_DMAx_PERIPH_DATA_SIZE     DMA_PDATAALIGN_HALFWORD
#define AUDIO_IN_SAIx_DMAx_MEM_DATA_SIZE        DMA_MDATAALIGN_HALFWORD

#define AUDIO_IN_IRQ_PREPRIO                ((uint32_t)0x0F)


#define AUDIO_I2C_ADDRESS                ((uint16_t)0x34)


///////////////////////////////////////////////////////////

void My_BSP_Audio_Init(void)
{
  haudio_out_sai.Instance = SAI1_Block_A;
  haudio_in_sai.Instance = SAI1_Block_B;

  RUN_AND_LOG( My_SAI_ClockConfig(frequency); );

  RUN_AND_LOG( My_AUDIO_OUT_MspInit(); );
  RUN_AND_LOG( My_SAI_Out_Init(frequency); );
  RUN_AND_LOG( My_AUDIO_IN_MspInit(); );
  RUN_AND_LOG( My_SAI_In_Init(frequency); );

  RUN_AND_LOG( wm8994_Init(AUDIO_I2C_ADDRESS, INPUT_DEVICE_INPUT_LINE_1 | OUTPUT_DEVICE_HEADPHONE, volume, frequency); );

  RUN_AND_LOG( HAL_SAI_Transmit_DMA(&haudio_out_sai, saiDMATransmitBuffer, MY_DMA_BUFFER_SIZE_MSIZES); );
  RUN_AND_LOG( HAL_SAI_Receive_DMA( &haudio_in_sai,  saiDMAReceiveBuffer,  MY_DMA_BUFFER_SIZE_MSIZES); );
}

///////////////////////////////////////////////////////////

static void My_SAI_ClockConfig(uint32_t AudioFreq)
{
  RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;

  HAL_RCCEx_GetPeriphCLKConfig(&rcc_ex_clk_init_struct);

  /* Set the PLL configuration according to the audio frequency */
  if((AudioFreq == AUDIO_FREQUENCY_11K) || (AudioFreq == AUDIO_FREQUENCY_22K) || (AudioFreq == AUDIO_FREQUENCY_44K))
  {
    /* Configure PLLSAI prescalers */
    /* PLLSAI_VCO: VCO_429M
    SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
    SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 214.5/19 = 11.289 Mhz */
    rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    rcc_ex_clk_init_struct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 429;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 2;
    rcc_ex_clk_init_struct.PLLI2SDivQ = 19;

    HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);

  }
  else /* AUDIO_FREQUENCY_8K, AUDIO_FREQUENCY_16K, AUDIO_FREQUENCY_48K, AUDIO_FREQUENCY_96K */
  {
    /* SAI clock config
    PLLSAI_VCO: VCO_344M
    SAI_CLK(first level) = PLLSAI_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
    SAI_CLK_x = SAI_CLK(first level)/PLLSAIDIVQ = 49.142/1 = 49.142 Mhz */
    rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    rcc_ex_clk_init_struct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLI2S;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 344;
    rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 7;
    rcc_ex_clk_init_struct.PLLI2SDivQ = 1;

    HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);
  }
}

static void My_AUDIO_OUT_MspInit(void)
{
  static DMA_HandleTypeDef hdma_sai_tx;
  GPIO_InitTypeDef  gpio_init_structure;

  /* Enable SAI clock */
  AUDIO_OUT_SAIx_CLK_ENABLE();

  /* Enable GPIO clock */
  AUDIO_OUT_SAIx_MCLK_ENABLE();
  AUDIO_OUT_SAIx_SD_FS_CLK_ENABLE();

  /* CODEC_SAI pins configuration: FS, SCK, MCK and SD pins ------------------*/
  gpio_init_structure.Pin = AUDIO_OUT_SAIx_FS_PIN | AUDIO_OUT_SAIx_SCK_PIN | AUDIO_OUT_SAIx_SD_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_AF;
  HAL_GPIO_Init(AUDIO_OUT_SAIx_SD_FS_SCK_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin = AUDIO_OUT_SAIx_MCLK_PIN;
  HAL_GPIO_Init(AUDIO_OUT_SAIx_MCLK_GPIO_PORT, &gpio_init_structure);

  /* Enable the DMA clock */
  AUDIO_OUT_SAIx_DMAx_CLK_ENABLE();

  /* Configure the hdma_saiTx handle parameters */
  hdma_sai_tx.Init.Channel             = AUDIO_OUT_SAIx_DMAx_CHANNEL;
  hdma_sai_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_sai_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_sai_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_sai_tx.Init.PeriphDataAlignment = AUDIO_OUT_SAIx_DMAx_PERIPH_DATA_SIZE;
  hdma_sai_tx.Init.MemDataAlignment    = AUDIO_OUT_SAIx_DMAx_MEM_DATA_SIZE;
  hdma_sai_tx.Init.Mode                = DMA_CIRCULAR;
  hdma_sai_tx.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_sai_tx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  hdma_sai_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_sai_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
  hdma_sai_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

  hdma_sai_tx.Instance = AUDIO_OUT_SAIx_DMAx_STREAM;

  /* Associate the DMA handle */
  __HAL_LINKDMA(&haudio_out_sai, hdmatx, hdma_sai_tx);

  /* Deinitialize the Stream for new transfer */
  HAL_DMA_DeInit(&hdma_sai_tx);

  /* Configure the DMA Stream */
  HAL_DMA_Init(&hdma_sai_tx);

  /* SAI DMA IRQ Channel configuration */
  HAL_NVIC_SetPriority(AUDIO_OUT_SAIx_DMAx_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
  HAL_NVIC_EnableIRQ(AUDIO_OUT_SAIx_DMAx_IRQ);
}

static void My_SAI_Out_Init(uint32_t AudioFreq)
{
  /* Disable SAI peripheral to allow access to SAI internal registers */
  __HAL_SAI_DISABLE(&haudio_out_sai);

  /* Configure SAI_Block_x
  LSBFirst: Disabled
  DataSize: 16 */
  haudio_out_sai.Init.MonoStereoMode = SAI_STEREOMODE;
  haudio_out_sai.Init.AudioFrequency = AudioFreq;
  haudio_out_sai.Init.AudioMode = SAI_MODEMASTER_TX;
  haudio_out_sai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLED;
  haudio_out_sai.Init.Protocol = SAI_FREE_PROTOCOL;
  haudio_out_sai.Init.DataSize = SAI_DATASIZE_16;
  haudio_out_sai.Init.FirstBit = SAI_FIRSTBIT_MSB;
  haudio_out_sai.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  haudio_out_sai.Init.Synchro = SAI_ASYNCHRONOUS;
  haudio_out_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLED;
  haudio_out_sai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
  haudio_out_sai.Init.SynchroExt     = SAI_SYNCEXT_DISABLE;
  haudio_out_sai.Init.CompandingMode = SAI_NOCOMPANDING;
  haudio_out_sai.Init.TriState       = SAI_OUTPUT_NOTRELEASED;
  haudio_out_sai.Init.Mckdiv         = 0;

  /* Configure SAI_Block_x Frame
  Frame Length: 64
  Frame active Length: 32
  FS Definition: Start frame + Channel Side identification
  FS Polarity: FS active Low
  FS Offset: FS asserted one bit before the first bit of slot 0 */
  haudio_out_sai.FrameInit.FrameLength = 64;
  haudio_out_sai.FrameInit.ActiveFrameLength = 32;
  haudio_out_sai.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
  haudio_out_sai.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  haudio_out_sai.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

  /* Configure SAI Block_x Slot
  Slot First Bit Offset: 0
  Slot Size  : 16
  Slot Number: 4
  Slot Active: All slot actives */
  haudio_out_sai.SlotInit.FirstBitOffset = 0;
  haudio_out_sai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  haudio_out_sai.SlotInit.SlotNumber = 4;
  haudio_out_sai.SlotInit.SlotActive = 0xF;

  HAL_SAI_Init(&haudio_out_sai);

  /* Enable SAI peripheral to generate MCLK */
  __HAL_SAI_ENABLE(&haudio_out_sai);

}

static void My_AUDIO_IN_MspInit(void)
{
    static DMA_HandleTypeDef hdma_sai_rx;
    GPIO_InitTypeDef  gpio_init_structure;

    /* Enable SAI clock */
    AUDIO_IN_SAIx_CLK_ENABLE();

    /* Enable SD GPIO clock */
    AUDIO_IN_SAIx_SD_ENABLE();
    /* CODEC_SAI pin configuration: SD pin */
    gpio_init_structure.Pin = AUDIO_IN_SAIx_SD_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FAST;
    gpio_init_structure.Alternate = AUDIO_IN_SAIx_AF;
    HAL_GPIO_Init(AUDIO_IN_SAIx_SD_GPIO_PORT, &gpio_init_structure);

    /* Enable the DMA clock */
    AUDIO_IN_SAIx_DMAx_CLK_ENABLE();

    /* Configure the hdma_sai_rx handle parameters */
    hdma_sai_rx.Init.Channel             = AUDIO_IN_SAIx_DMAx_CHANNEL;
    hdma_sai_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_sai_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_sai_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_sai_rx.Init.PeriphDataAlignment = AUDIO_IN_SAIx_DMAx_PERIPH_DATA_SIZE;
    hdma_sai_rx.Init.MemDataAlignment    = AUDIO_IN_SAIx_DMAx_MEM_DATA_SIZE;
    hdma_sai_rx.Init.Mode                = DMA_CIRCULAR;
    hdma_sai_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_sai_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_sai_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_sai_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
    hdma_sai_rx.Init.PeriphBurst         = DMA_MBURST_SINGLE;

    hdma_sai_rx.Instance = AUDIO_IN_SAIx_DMAx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(&haudio_in_sai, hdmarx, hdma_sai_rx);

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_sai_rx);

    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_sai_rx);

    /* SAI DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(AUDIO_IN_SAIx_DMAx_IRQ, AUDIO_IN_IRQ_PREPRIO, 0);
    HAL_NVIC_EnableIRQ(AUDIO_IN_SAIx_DMAx_IRQ);
}


static void My_SAI_In_Init(uint32_t AudioFreq)
{
    /* Initialize SAI1 block B in SLAVE RX synchronous from SAI1 block A */
    /* Initialize the haudio_in_sai Instance parameter */
    haudio_in_sai.Instance = AUDIO_IN_SAIx;

    /* Disable SAI peripheral to allow access to SAI internal registers */
    __HAL_SAI_DISABLE(&haudio_in_sai);

    /* Configure SAI_Block_x */
    haudio_in_sai.Init.MonoStereoMode = SAI_STEREOMODE;
    haudio_in_sai.Init.AudioFrequency = AudioFreq;
    haudio_in_sai.Init.AudioMode      = SAI_MODESLAVE_RX;
    haudio_in_sai.Init.NoDivider      = SAI_MASTERDIVIDER_ENABLE;
    haudio_in_sai.Init.Protocol       = SAI_FREE_PROTOCOL;
    haudio_in_sai.Init.DataSize       = SAI_DATASIZE_16;
    haudio_in_sai.Init.FirstBit       = SAI_FIRSTBIT_MSB;
    haudio_in_sai.Init.ClockStrobing  = SAI_CLOCKSTROBING_FALLINGEDGE;
    haudio_in_sai.Init.Synchro        = SAI_SYNCHRONOUS;
    haudio_in_sai.Init.OutputDrive    = SAI_OUTPUTDRIVE_DISABLE;
    haudio_in_sai.Init.FIFOThreshold  = SAI_FIFOTHRESHOLD_1QF;
    haudio_in_sai.Init.SynchroExt     = SAI_SYNCEXT_DISABLE;
    haudio_in_sai.Init.CompandingMode = SAI_NOCOMPANDING;
    haudio_in_sai.Init.TriState       = SAI_OUTPUT_RELEASED;
    haudio_in_sai.Init.Mckdiv         = 0;

    /* Configure SAI_Block_x Frame */
    haudio_in_sai.FrameInit.FrameLength       = 64;
    haudio_in_sai.FrameInit.ActiveFrameLength = 32;
    haudio_in_sai.FrameInit.FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION;
    haudio_in_sai.FrameInit.FSPolarity        = SAI_FS_ACTIVE_LOW;
    haudio_in_sai.FrameInit.FSOffset          = SAI_FS_BEFOREFIRSTBIT;

    /* Configure SAI Block_x Slot */
    haudio_in_sai.SlotInit.FirstBitOffset = 0;
    haudio_in_sai.SlotInit.SlotSize       = SAI_SLOTSIZE_DATASIZE;
    haudio_in_sai.SlotInit.SlotNumber     = 4;
    haudio_in_sai.SlotInit.SlotActive     = 0xF;

    HAL_SAI_Init(&haudio_in_sai);

    /* Enable SAI peripheral */
    __HAL_SAI_ENABLE(&haudio_in_sai);
}

////////////////////////////////////////////////////////////////

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai){
  My_AUDIO_IN_HalfTransfer_CallBack();
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai){
  My_AUDIO_IN_TransferComplete_CallBack();
}

void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai){
  My_AUDIO_OUT_Error_CallBack();
}

void DMA2_Stream4_IRQHandler(void){
  HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}

void DMA2_Stream1_IRQHandler(void){
  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}


//frame length 8 bytes, ie. samples are spaced 8 bytes apart
//sample size 2 bytes
//frame is every 8 bytes, data appears at the 1st and 4th, left and right channel
//for now just take 1 channel when extracting, and duplicate both channels when inserting
void ExtractSamplesFromDMAReceiveBuffer_LowerHalf(int16_t * sampleBuffer, uint32_t num_samples)
{
  for(uint32_t i = 0; i<num_samples; i++){
    int16_t * samplePointer = (int16_t *) & saiDMAReceiveBuffer[i*8];
    sampleBuffer[i] = *samplePointer;
  }
}
void ExtractSamplesFromDMAReceiveBuffer_UpperHalf(int16_t * sampleBuffer, uint32_t num_samples)
{
  for(uint32_t i = 0; i<num_samples; i++){
    int16_t * samplePointer = (int16_t *) & saiDMAReceiveBuffer[(MY_DMA_BUFFER_SIZE_BYTES / 2) + i*8];
    sampleBuffer[i] = *samplePointer;
  }
}
void InsertSamplesIntoDMATransmitBuffer_LowerHalf(int16_t * sampleBuffer, uint32_t num_samples)
{
  for(uint32_t i=0; i<num_samples; i++){
    int16_t * p = (int16_t *) &saiDMATransmitBuffer[i*8];
    *p = sampleBuffer[i];
    *(p+2) = sampleBuffer[i];
  }
}
void InsertSamplesIntoDMATransmitBuffer_UpperHalf(int16_t * sampleBuffer, uint32_t num_samples)
{
  for(uint32_t i=0; i<num_samples; i++){
    int16_t * p = (int16_t *) &saiDMATransmitBuffer[(MY_DMA_BUFFER_SIZE_BYTES / 2) + i*8];
    *p = sampleBuffer[i];
    *(p+2) = sampleBuffer[i];
  }
}

