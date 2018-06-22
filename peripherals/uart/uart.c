#include "uart.h"
#include "../../include/types.h"
#include "../../include/versatile.h"
#include "../../kernel/interrupts.h"

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

/* bit positions of uart interrupts */
#define UART_OE_IRQ 10//overrun error
#define UART_BE_IRQ 9//break error
#define UART_PE_IRQ 8//parity error
#define UART_FE_IRQ 7//framing error
#define UART_RT_IRQ 6//receive timeout
#define UART_TX_IRQ 5//transmit interrupt
#define UART_RX_IRQ 4//receive interrupt

#define UART_CLK_SPEED 24000000 //in unit of Hz
#define UART_RX_BUFFER_SIZE 1024
#define UART_FIFO_SIZE 16
#define UART_FIFO_IRQ_LEVEL 2
#ifdef VERSATILEPB
#define UART_PORT_NUM 4
#else
#define UART_PORT_NUM 3
#endif

/* macro definitions */
#define uart_read_reg(base_addr, reg) ({
  u16 *reg_addr = (u16 *)(base_addr + reg);
  *reg_addr;
})

#define uart_write_reg(base_addr, reg, data) ({
  u16 *reg_addr = (u16 *)(base_addr + reg);
  *reg_addr = (u16)data;
})

/* global variables */
#ifdef VERSATILEPB
static UART uart_instances[4] = {
  [0] = {
    .port = 0,
    .base_addr = VERSATILE_UART0_BASE,
    .enable = 0,
    .parity = 0,
    .stop_bit_num = 1,
    .baud_rate = 115200,
    .inuse = 0,
  },
  [1] = {
    .port = 1,
    .base_addr = VERSATILE_UART1_BASE,
    .enable = 0,
    .parity = 0,
    .stop_bit_num = 1,
    .baud_rate = 115200,
    .inuse = 0,
  },
  [2] = {
    .port = 2,
    .base_addr = VERSATILE_UART2_BASE,
    .enable = 0,
    .parity = 0,
    .stop_bit_num = 1,
    .baud_rate = 115200,
    .inuse = 0,
  },
  [3] = {
    .port = 3,
    .base_addr = VERSATILE_UART3_BASE,
    .enable = 0,
    .parity = 0,
    .stop_bit_num = 1,
    .baud_rate = 115200,
    .inuse = 0,
  },
};
static u8 uart_rx_buffer[4][UAET_RX_BUFFER_SIZE] = {[0] = {0},};
static u16 uart_rx_buffer_start[4] = {0};
static u16 uart_rx_buffer_end[4] = {0};
#else
static UART uart_instances[3] = {
  [0] = {
    .port = 0,
    .base_addr = VERSATILE_UART0_BASE,
    .enable = 0,
    .parity = 0,
    .stop_bit_num = 1,
    .baud_rate = 115200,
    .inuse = 0,
  },
  [1] = {
    .port = 1,
    .base_addr = VERSATILE_UART1_BASE,
    .enable = 0,
    .parity = 0,
    .stop_bit_num = 1,
    .baud_rate = 115200,
    .inuse = 0,
  },
  [2] = {
    .port = 2,
    .base_addr = VERSATILE_UART2_BASE,
    .enable = 0,
    .parity = 0,
    .stop_bit_num = 1,
    .baud_rate = 115200,
    .inuse = 0,
  },
};
static u8 uart_rx_buffer[3][UART_RX_BUFFER_SIZE] = {[0] = {0},};
static u16 uart_rx_buffer_start[3] = {0};
static u16 uart_rx_buffer_end[3] = {0};
#endif

static u8 uart_tx_flag = 0;
static u8 uart_rx_error_flag = 0;

/* helper function */
static u16 get_rx_data_size(u8 port){
    if(uart_rx_buffer_end[port] < uart_rx_buffer_start[port]){
      return (uart_rx_buffer_end[port] + UART_RX_BUFFER_SIZE - uart_rx_buffer_start[port]);
    }
    else{
      return (uart_rx_buffer_end[port] - uart_rx_buffer_start[port]);
    }
}

static void read_uart_rx_buffer(UART* uart, u8* dest, u32 len){
  while(get_rx_data_size(uart->port) < len);
  if(UART_RX_BUFFER_SIZE - uart_rx_buffer_start[uart->port] < len){
    memcpy(dest, &(uart_rx_buffer[uart->port][uart_rx_buffer_start[uart->port]]), UART_BUFFER_SIZE - uart_rx_buffer_start[uart->port]));
    uart_rx_buffer_start[uart->port] = (uart_rx_buffer_start[uart->port] + len) % UART_BUFFER_SIZE;
    memcpy(dest, &(uart_rx_buffer[uart->port][0]), uart_rx_buffer_start[uart->port]));
  }
  else{
    memcpy(dest, &(uart_rx_buffer[uart->port][uart_rx_buffer_start[uart->port]], len));
    uart_rx_buffer_start[uart->port] = (uart_rx_buffer_start[uart->port] + len) % UART_BUFFER_SIZE;
  }
}

/* public functions */
UART *uart_init(u8 port){
  if(port > UART_PORT_NUM-1){
    return NULL;
  }

  UART *uart = &(uart_instances[port]);
  if(uart->inuse){
    return uart;
  }
  else{
    /* initialize uart registers */
    uart->enable = 0;
    uart->parity = UART_NO_PARITY;
    uart->stop_bit_num = 1;
    uart->fifo_mode = 0;
    uart->baud_rate = 115200;
    uart->inuse = 1;
    float baud_rate_divisor = (float)UART_CLK_SPEED / (16 * uart->baud_rate);
    u16 divisor_int = baud_rate_divisor;
    u16 divisor_frac = (baud_rate_divisor - divisor_int) * 64;
    uart_write_reg(uart->base-addr, UART_I_BAUD_RATE_REG, divisor_int);
    uart_write_reg(uart->base-addr, UART_I_BAUD_RATE_REG, divisor_frac);
    uart_write_reg(uart->base_addr, UART_LINE_CTRL_REG, 0x60); //set to FIFOs disabled, no parity and 1 stop bit
    uart_write_reg(uart->base_addr, UART_ITR_MASK_REG, 0x078f); //mask out error and modem interrupts
    uart_write_reg(uart->base_addr, UART_CTROL_REG, 0x0300);

    /* set corresponding uart interrupt handler */
    switch(port){
      case 0:
        PIC_irq_handler_tbl[PIC_UART0_IRQ] = uart0_interrupt_handler;
        break;
      case 1:
        PIC_irq_handler_tbl[PIC_UART1_IRQ] = uart1_interrupt_handler;
        break;
      case 2:
        PIC_irq_handler_tbl[PIC_UART2_IRQ] = uart2_interrupt_handler;
        break;
      case 3:
      default:
        return UART_FAILURE;
    }
  }
  return uart;
}

u8 uart_enable(UART* uart){
  if(uart->inuse == 0) return UART_FAILURE;
  if(uart->enable == 0){
    uart->enable = 1;
    u16 ctrl_reg = uart_read_reg(uart->base_addr, UART_CTRL_REG);
    ctrl_reg |= 0x0001; //enable uart
    uart_write_reg(uart->base_addr, UART_CTROL_REG, ctrl_reg);
  }
  return UART_SUCCESS;
}

u8 uart_disable(UART* uart){
  if(uart->inuse == 0) return UART_FAILURE;
  if(uart->enable){
    uart->enable = 0;
    u16 ctrl_reg = uart_read_reg(uart->base_addr, UART_CTRL_REG);
    ctrl_reg &= 0xfffe; //disable uart
    uart_write_reg(uart->base_addr, UART_CTROL_REG, ctrl_reg);
  }
  return UART_SUCCESS;
}

u8 uart_apply_configs(UART* uart){
  if(uart->enable == 0) return UART_FAILURE;

  u16 line_ctrl_reg = uart_read_reg(uart->base_addr, UART_LINE_CTRL_REG);
  line_ctrl_reg &= 0x61;

  /* set parity */
  if(uart->parity == UART_EVEN_PARITY){
    line_ctrl_reg |= 0x06; //set even parity
  }
  else if(uart->parity == UART_ODD_PARITY){
    line_ctrl_reg |= 0x02; //set odd parity
  }
  else if(uart->parity != UART_NO_PARITY){
    return UART_FAILURE;
  }

  /* set stop bit(s) */
  if(uart->stop_bit_num != 2){
    line_ctrl_reg |= 0x08; //set 2 stop bits
  }
  else if(uart->stop_bit_num != 1){
    return UART_FAILURE;
  }

  /* set fifo mode */
  if(uart->fifo_mode == 1){
    line_ctrl_reg |= 0x10; //enable fifo
  }
  else if(uart->fifo_mode != 0){
    return UART_FAILURE;
  }

  /* set baud rate */
  float baud_rate_divisor = (float)UART_CLK_SPEED / (16 * uart->baud_rate);
  u16 divisor_int = baud_rate_divisor;
  u16 divisor_frac = (baud_rate_divisor - divisor_int) * 64;

  if(uart_disable(uart) == UART_FAILURE) return UARRT_FAILURE;
  uart_write_reg(uart->base-addr, UART_I_BAUD_RATE_REG, divisor_int);
  uart_write_reg(uart->base-addr, UART_I_BAUD_RATE_REG, divisor_frac);
  uart_write_reg(uart->base_addr, UART_LINE_CTRL_REG, line_ctrl_reg);
  if(uart_enable(uart) == UART_FAILURE) return UARRT_FAILURE;
  return UART_SUCCESS;
}

u8 uart_send(UART* uart, u8 *src, u32 len){
  while(len > 0){
    if(uart_tx_flag == 0){
      if(uart->fifo_mode){ //fifo enabled
        u8 i;
        if(len > UART_FIFO_SIZE/UART_FIFO_IRQ_LEVEL){
          for(i=0; i<UART_FIFO_SIZE/UART_FIFO_IRQ_LEVEL; i++){
            uart_write_reg(uart->base_addr, UART_DATA_REG, *src);
            src++;
            len--;
          }
          uart_tx_flag = 1;
        }
        else{
          for(i=0; i<len; i++){
            uart_write_reg(uart->base_addr, UART_DATA_REG, *src);
            src++;
            len--;
          }
        }
      }
      else{ //fifo disabled
        uart_write_reg(uart->base_addr, UART_DATA_REG, *src);
        src++;
        len--;
        if(len != 0) uart_tx_flag = 1;
      }
    }
  }
  return UART_SUCCESS;
}

u8 uart_receive(UART* uart, u8 *dest, u32 len){
  uart_rx_flag = 1;
  while(len > 0){
    if(len > UART_RX_BUFFER_SIZE/2){
      read_uart_rx_buffer(uart, dest, UART_RX_BUFFER_SIZE/2);
      len -= UART_RX_BUFFER_SIZE / 2;
    }
    else{
      read_uart_rx_buffer(uart, dest, len);
      len -= len;
    }
  }
  uart_rx_flag = 0;
  if(uart_rx_error_flag){
    uart_rx_error_flag = 0;
    return UART_FAILURE;
  }
  else{
    return UART_SUCCESS;
  }
}

/* uart interrupt handler */
static void uart_generic_interrupt_handler(u8 port){
  u16 irq_status_reg = uart_read_reg(uart_instances[port].base_addr, UART_ITR_STATUS_REG);
  if(irq_status_reg & (1 << UART_TX_IRQ)){
    if(uart_tx_flag == 0){
      uart_write_reg(uart_instances[port].base_addr, UART_ITR_CLR_REG, (1 << UART_TX_IRQ));
    }
    uart_tx_flag = 0;
  }
  else if(irq_status_reg & (1 << UART_RX_IRQ)){
    if(uart_instances[port].fifo_mode){
      u8 i;
      for(i=0; i<UART_FIFO_SIZE/UART_FIFO_IRQ_LEVEL; i++){
        u16 data_reg = uart_read_reg(uart_instances[port].base_addr, UART_DATA_REG);
        if(uart_rx_flag){
          if(data_reg & 0x0f00){
            uart_rx_error_flag = 1;
            uart_rx_buffer[port][uart_rx_buffer_end[port]] = 0;
            uart_rx_buffer_end[port]++;
          }
          else{
            uart_rx_buffer[port][uart_rx_buffer_end[port]] = (u8)(data_reg & 0x00ff);
            uart_rx_buffer_end[port]++;
          }
        }
      }
    }
    else{
      u16 data_reg = uart_read_reg(uart_instances[port].base_addr, UART_DATA_REG);
      if(uart_rx_flag){
        if(data_reg & 0x0f00){
          uart_rx_error_flag = 1;
          uart_rx_buffer[port][uart_rx_buffer_end[port]] = 0;
          uart_rx_buffer_end[port]++;
        }
        else{
          uart_rx_buffer[port][uart_rx_buffer_end[port]] = (u8)(data_reg & 0x00ff);
          uart_rx_buffer_end[port]++;
        }
      }
    }
  }
  else if(irq_status_reg & (1 << UART_RT_IRQ)){
    u16 data_reg = uart_read_reg(uart_instances[port].base_addr, UART_DATA_REG);
    if(uart_rx_flag){
      if(data_reg & 0x0f00){
        uart_rx_error_flag = 1;
        uart_rx_buffer[port][uart_rx_buffer_end[port]] = 0;
        uart_rx_buffer_end[port]++;
      }
      else{
        uart_rx_buffer[port][uart_rx_buffer_end[port]] = (u8)(data_reg & 0x00ff);
        uart_rx_buffer_end[port]++;
      }
    }
  }
}

void uart0_interrupt_handler(void){
  uart_generic_interrupt_handler(0);
}

void uart1_interrupt_handler(void){
  uart_generic_interrupt_handler(1);
}

void uart2_interrupt_handler(void){
  uart_generic_interrupt_handler(2);
}

#ifdef VERSATILEPB
void uart3_interrupt_handler(void){
  uart_generic_interrupt_handler(3);
}
#endif
