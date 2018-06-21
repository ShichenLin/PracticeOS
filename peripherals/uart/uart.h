#include "../../include/types.h"
#include "../../include/versatile.h"

#define UART_SUCCESS 1
#define UART_FAILURE 0
#define UART_NO_PARITY 0
#define UART_ODD_PARITY 1
#define UART_EVEN_PARITY 2

typedef struct{
  u8 port;
  u8 parity;
  u8 stop_bit_num;
  u8 fifo_mode;
  u32 base_addr;
  u32 baud_rate;
  u8 enable;
  u8 inuse;
}UART;

UART* uart_init(u8 port);
u8 uart_enable(UART* uart);
u8 uart_disable(UART* uart);
u8 uart_apply_configs(UART* uart);
u8 uart_send(UART *uart, u8 *src, u32 len);
u8 uart_receive(UART *uart, u8 *dest, u32 len);
void uart0_interrupt_handler(void);
void uart1_interrupt_handler(void);
void uart2_interrupt_handler(void);
#ifdef VERSATILEPB
void uart3_interrupt_handler(void);
#endif
