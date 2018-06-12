#include "boot.h"
#include "../peripherals/uart/uart.h"

static void timer_init(void){

}

static void flash_init(void){

}

static void dram_init(void){

}

static void uart_init(void){

}

void board_init(void){
    enable_caches_and_mmu();
    unmap_memory();
    timer_init();
    flash_init();
    dram_init();
    uart_init();
}
