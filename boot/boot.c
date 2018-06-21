#include "boot.h"
#include "../peripherals/uart/uart.h"
#include "../kernel/interrupts.h"

void board_init(void){
    irq_init();
    UART* uart = uart_init(0);
}
