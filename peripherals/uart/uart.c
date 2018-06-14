#include "uart.h"

/* UART register adresses */
#define UART_DATA_REG 0x000
#define UART_STATUS_REG 0x004
#define UART_FLAG_REG 0x018
#define UART_LPC_REG 0x020 //lower power counter register
#define UART_I_BAUD_RATE_REG 0x024 //integer baud rate register
#define UART_F_BAUD_RATE_REG 0x028 //fractional baud rate register
#define UART_LINE_CTRL_REG 0x02c
#define UART_CTRL_REG 0x030
#define UART_ITR_FIFO_REG 0x034 //interrupt FIFO level select register
#define UART_ITR_MASK_REG 0x038
#define UART_RAW_ITR_REG 0x03c
#define UART_ITR_STATUS_REG 0x040
#define UART_ITR_CLR_REG 0x044
#define UART_DMA_REG 0x048
#define UART_PHERIPH_ID0_REG 0xfe0
#define UART_PHERIPH_ID1_REG 0xfe4
#define UART_PHERIPH_ID2_REG 0xfe8
#define UART_PHERIPH_ID3_REG 0xfec
#define UART_PCELL_ID0_REG 0xff0
#define UART_PCELL_ID1_REG 0xff4
#define UART_PCELL_ID2_REG 0xff8
#define UART_PCELL_ID3_REG 0xffc

#define read_uart_reg(port, reg) ({
  u32 *reg_addr = (u32 *)VERSATILE_UART##port##_BASE + reg;
  *reg_addr;
})

#define write_uart_reg(port, reg, data) ({
  u32 *reg_addr = (u32 *)VERSATILE_UART##port##_BASE + reg;
  *reg_addr = (u32)data;
})

static u8 uart_buffer[1024];

void uart_init(void){
  
}

bool uart_send(u8 *src, u32 len){

}

bool uart_receive(u8 *dest, u32){

}
