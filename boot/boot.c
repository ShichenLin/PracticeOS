#include <stdio.h>
#include "boot.h"
#include "../peripherals/uart/uart.h"
#include "../kernel/interrupts.h"

void board_init(void){
    irq_init();
    UART* uart0;
    if((uart0 = uart_init(0)) == NULL) reset();
    uart_send(uart0, (unsigned char *)"hello world\n", 12);
}
