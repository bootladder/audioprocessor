#include <stdint.h>

void BSP_UART_Init(void);

typedef void (*BSP_UART_Receive_Callback_t)(uint8_t * buf);

void BSP_UART_Receive_Bytes(uint8_t length, BSP_UART_Receive_Callback_t callback);
void BSP_UART_Transmit_Bytes_Blocking(uint8_t * buf, uint16_t size);
