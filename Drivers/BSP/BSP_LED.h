typedef enum
  {
    LED1 = 0,
    LED_RED = LED1,
    LED2 = 1,
    LED_GREEN = LED2
  } Led_TypeDef;

void             BSP_LED_Init(Led_TypeDef Led);
void             BSP_LED_DeInit(Led_TypeDef Led);
void             BSP_LED_On(Led_TypeDef Led);
void             BSP_LED_Off(Led_TypeDef Led);
