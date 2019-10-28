#include "BSP_LED.h"
#include "stm32f7xx_hal.h"


#define LED1_GPIO_PORT                   ((GPIO_TypeDef*)GPIOJ)
#define LED2_GPIO_PORT                   ((GPIO_TypeDef*)GPIOJ)

#define LED1_PIN                         ((uint32_t)GPIO_PIN_13)
#define LED2_PIN                         ((uint32_t)GPIO_PIN_5)

uint32_t GPIO_PIN[] = {LED1_PIN,
                           LED2_PIN};

GPIO_TypeDef* GPIO_PORT[] = {LED1_GPIO_PORT,
                                 LED2_GPIO_PORT};

#define LEDx_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOJ_CLK_ENABLE()



void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;

  LEDx_GPIO_CLK_ENABLE();
  /* Configure the GPIO_LED pin */
  gpio_init_structure.Pin   = GPIO_PIN[Led];
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;

  HAL_GPIO_Init(GPIO_PORT[Led], &gpio_init_structure);

}
void BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
}

/**
 * @brief  Turns selected LED Off.
 * @param  Led: LED to be set off
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval None
 */
void BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}

/**
 * @brief  Toggles the selected LED.
 * @param  Led: LED to be toggled
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval None
 */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
}
