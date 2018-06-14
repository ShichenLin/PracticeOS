#include "boot.h"
#include "../peripherals/uart/uart.h"

void board_init(void){
    timer_init();
    uart_init();
}
