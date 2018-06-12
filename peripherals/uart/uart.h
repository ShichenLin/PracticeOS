#include "../../include/types.h"
#include "../../include/versatile.h"

#define UART_SUCCESS 1
#define UART_FAILURE 0

void uart_init(void);

u8 uart_send(u8 *src, u32 len);
u8 uart_receive(u8 *dest, u32 buffer_size);
