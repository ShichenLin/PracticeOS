#include "boot.h"
#include "../peripherals/uart/uart.h"

static void timer_init(void){

}

static void flash_init(void){

}

static void uart_init(void){

}

void board_init(void){
    //timer_init();
    uart_init();
}
