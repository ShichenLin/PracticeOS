#include "interrupts.h"
#include "../include/types.h"
#include "../include/versatile.h"

/* PIC register offsets */
#define PIC_IRQ_STATUS_REG 0x000
#define PIC_FIO_STATUS_REG 0x004
#define PIC_RAW_STATUS_REG 0x008
#define PIC_SELECT_REG 0x00c
#define PIC_ENABLE_REG 0x010
#define PIC_ENABLE_CLR_REG 0x014
#define PIC_SW_REG 0x018
#define PIC_SW_CLR_REG 0x01c
#define PIC_PROT_ENABLE_REG 0x020
#define PIC_VEC_ADDR_REG 0x030
#define PIC_DEFAULT_VEC_ADDR_REG 0x034
#define PIC_VEC_ADDR0_REG 0x100
#define PIC_VEC_ADDR1_REG 0x104
#define PIC_VEC_ADDR2_REG 0x108
#define PIC_VEC_ADDR3_REG 0x10c
#define PIC_VEC_ADDR4_REG 0x110
#define PIC_VEC_ADDR5_REG 0x114
#define PIC_VEC_ADDR6_REG 0x118
#define PIC_VEC_ADDR7_REG 0x11c
#define PIC_VEC_ADDR8_REG 0x120
#define PIC_VEC_ADDR9_REG 0x124
#define PIC_VEC_ADDR10_REG 0x128
#define PIC_VEC_ADDR11_REG 0x12c
#define PIC_VEC_ADDR12_REG 0x130
#define PIC_VEC_ADDR13_REG 0x134
#define PIC_VEC_ADDR14_REG 0x138
#define PIC_VEC_ADDR15_REG 0x13c
#define PIC_VEC_CTRL0_REG 0x200
#define PIC_VEC_CTRL1_REG 0x204
#define PIC_VEC_CTRL2_REG 0x208
#define PIC_VEC_CTRL3_REG 0x20c
#define PIC_VEC_CTRL4_REG 0x210
#define PIC_VEC_CTRL5_REG 0x214
#define PIC_VEC_CTRL6_REG 0x218
#define PIC_VEC_CTRL7_REG 0x21c
#define PIC_VEC_CTRL8_REG 0x220
#define PIC_VEC_CTRL9_REG 0x224
#define PIC_VEC_CTRL10_REG 0x228
#define PIC_VEC_CTRL11_REG 0x22c
#define PIC_VEC_CTRL12_REG 0x230
#define PIC_VEC_CTRL13_REG 0x234
#define PIC_VEC_CTRL14_REG 0x238
#define PIC_VEC_CTRL15_REG 0x23c
#define PIC_TEST_CTRL_REG
#define PIC_PERIPH_ID0_REG 0xfe0
#define PIC_PERIPH_ID1_REG 0xfe4
#define PIC_PERIPH_ID2_REG 0xfe8
#define PIC_PERIPH_ID3_REG 0xfec
#define PIC_CELL_ID0_REG 0xff0
#define PIC_CELL_ID1_REG 0xff4
#define PIC_CELL_ID2_REG 0xff8
#define PIC_CELL_ID3_REG 0xffc

/* SIC register offsets */
#define SIC_STATUS_REG 0x000
#define SIC_RAW_STATUS_REG 0x004
#define SIC_ENABLE_REG 0x008
#define SIC_ENABLE_CLR_REG 0x00c
#define SIC_SW_REG 0x010
#define SIC_SW_CLR_REG 0x014
#define SIC_PIC_ENABLE_REG 0x020
#define SIC_PIC_ENABLE_CLR_REG 0x024

#define IRQ_MISS 32
#define IRQ_FAIL_COUNT_LIMIT 10

/* macro definitions */
#define PIC_read_reg(reg) ({ \
  u32 *reg_addr = (u32 *)(VERSATILE_PIC_BASE + reg); \
  *reg_addr; \
})

#define PIC_write_reg(reg, data) ({ \
  u32 *reg_addr = (u32 *)(VERSATILE_PIC_BASE + reg); \
  *reg_addr = (u32)data; \
})

#define PIC_enable_irq(irq) ({ \
  u32 *enable_reg = (u32 *)(VERSATILE_PIC_BASE + PIC_ENABLE_REG); \
  *enable_reg = 1 << irq; \
})

#define PIC_clear_irq(irq) ({ \
  u32 *clr_reg = (u32 *)(VERSATILE_PIC_BASE + PIC_ENABLE_CLR_REG); \
  *clr_reg = 1 << irq; \
})

#define SIC_read_reg(reg) ({ \
  u32 *reg_addr = (u32 *)(VERSATILE_SIC_BASE + reg); \
  *reg_addr; \
})

#define SIC_write_reg(reg, data) ({ \
  u32 *reg_addr = (u32 *)(VERSATILE_SIC_BASE + reg); \
  *reg_addr = (u32)data; \
})

#define SIC_enable_irq(irq) ({ \
  u32 *enable_reg = (u32 *)(VERSATILE_SIC_BASE + SIC_ENABLE_REG); \
  *enable_reg = 1 << irq; \
})

#define SIC_clear_irq(irq) ({ \
  u32 *clr_reg = (u32 *)(VERSATILE_SIC_BASE + SIC_ENABLE_CLR_REG); \
  *clr_reg = 1 << irq; \
})

/* global variables */
u8 irq_fail_count = 0;

/* helper functions */
u8 PIC_indicate_irq_source(u32 irq_status){
  if(irq_status & (1 << PIC_WDOG_IRQ)) return PIC_WDOG_IRQ;
  else if(irq_status & (1 << PIC_SW_IRQ)) return PIC_SW_IRQ;
  else if(irq_status & (1 << PIC_DEBUG_RX_IRQ)) return PIC_DEBUG_RX_IRQ;
  else if(irq_status & (1 << PIC_DEBUG_TX_IRQ)) return PIC_DEBUG_TX_IRQ;
  else if(irq_status & (1 << PIC_TIMER01_IRQ)) return PIC_TIMER01_IRQ;
  else if(irq_status & (1 << PIC_TIMER23_IRQ)) return PIC_TIMER23_IRQ;
  else if(irq_status & (1 << PIC_GPIO0_IRQ)) return PIC_GPIO0_IRQ;
  else if(irq_status & (1 << PIC_GPIO1_IRQ)) return PIC_GPIO1_IRQ;
  else if(irq_status & (1 << PIC_GPIO2_IRQ)) return PIC_GPIO2_IRQ;
  else if(irq_status & (1 << PIC_GPIO3_IRQ)) return PIC_GPIO3_IRQ;
  else if(irq_status & (1 << PIC_RTC_IRQ)) return PIC_RTC_IRQ;
  else if(irq_status & (1 << PIC_SSP_IRQ)) return PIC_SSP_IRQ;
  else if(irq_status & (1 << PIC_UART0_IRQ)) return PIC_UART0_IRQ;
  else if(irq_status & (1 << PIC_UART1_IRQ)) return PIC_UART1_IRQ;
  else if(irq_status & (1 << PIC_UART2_IRQ)) return PIC_UART2_IRQ;
  else if(irq_status & (1 << PIC_SCI_IRQ)) return PIC_SCI_IRQ;
  else if(irq_status & (1 << PIC_CLCD_IRQ)) return PIC_CLCD_IRQ;
  else if(irq_status & (1 << PIC_DMA_IRQ)) return PIC_DMA_IRQ;
  else if(irq_status & (1 << PIC_PWR_FAIL_IRQ)) return PIC_PWR_FAIL_IRQ;
  else if(irq_status & (1 << PIC_GPU_IRQ)) return PIC_GPU_IRQ;
  else if(irq_status & (1 << PIC_DOC_FLASH_IRQ)) return PIC_DOC_FLASH_IRQ;
  else if(irq_status & (1 << PIC_MCI0A_IRQ)) return PIC_MCI0A_IRQ;
  else if(irq_status & (1 << PIC_AACI_IRQ)) return PIC_AACI_IRQ;
  else if(irq_status & (1 << PIC_ETHERNET_IRQ)) return PIC_ETHERNET_IRQ;
  else if(irq_status & (1 << PIC_USB_IRQ)) return PIC_USB_IRQ;
  else if(irq_status & (1 << PIC_EXP_CONNECTOR0_IRQ)) return PIC_EXP_CONNECTOR0_IRQ;
  else if(irq_status & (1 << PIC_EXP_CONNECTOR1_IRQ)) return PIC_EXP_CONNECTOR1_IRQ;
  else if(irq_status & (1 << PIC_SIC_IRQ)) return PIC_SIC_IRQ;
  else return IRQ_MISS;
}

u8 SIC_indicate_irq_source(u32 irq_status){
  if(irq_status & (1 << SIC_SW_IRQ)) return SIC_SW_IRQ;
  else if(irq_status & (1 << SIC_MMCI0B_IRQ)) return SIC_MMCI0B_IRQ;
  else if(irq_status & (1 << SIC_KEYBOARD_IRQ)) return SIC_KEYBOARD_IRQ;
  else if(irq_status & (1 << SIC_MOUSE_IRQ)) return SIC_MOUSE_IRQ;
  else if(irq_status & (1 << SIC_TOUCHSCREEN_IRQ)) return SIC_TOUCHSCREEN_IRQ;
  else if(irq_status & (1 << SIC_KEYPAD_IRQ)) return SIC_KEYPAD_IRQ;
  else if(irq_status & (1 << SIC_DOC_FLASH_IRQ)) return SIC_DOC_FLASH_IRQ;
  else if(irq_status & (1 << SIC_MMCI0A_IRQ)) return SIC_MMCI0A_IRQ;
  else if(irq_status & (1 << SIC_AACI_IRQ)) return SIC_AACI_IRQ;
  else if(irq_status & (1 << SIC_ETHERNET_IRQ)) return SIC_ETHERNET_IRQ;
  else if(irq_status & (1 << SIC_USB_IRQ)) return SIC_USB_IRQ;
  else return IRQ_MISS;
}

/* interrupt handlers */
void void_interrupt_handler(void){
  return;
}

void generic_interrupt_handler(void){
  u32 PIC_irq_status = PIC_read_reg(PIC_IRQ_STATUS_REG);
  u8 PIC_irq_source = PIC_indicate_irq_source(PIC_irq_status);
  if(PIC_irq_source == IRQ_MISS){
    irq_fail_count++;
    if (irq_fail_count == IRQ_FAIL_COUNT_LIMIT) reset();
  }
  else if(PIC_irq_source == PIC_SIC_IRQ){
    PIC_clear_irq(PIC_irq_source);
    u32 SIC_irq_status = SIC_read_reg(SIC_STATUS_REG);
    u8 SIC_irq_source = SIC_indicate_irq_source(SIC_irq_status);
    if(SIC_irq_source == IRQ_MISS){
      irq_fail_count++;
      if (irq_fail_count == IRQ_FAIL_COUNT_LIMIT) reset();
    }
    else{
      SIC_clear_irq(SIC_irq_source);
      (*SIC_irq_handler_tbl[SIC_irq_source])();
      SIC_enable_irq(SIC_irq_source);
    }
    PIC_enable_irq(PIC_irq_source);
  }
  else{
    PIC_clear_irq(PIC_irq_source);
    (*PIC_irq_handler_tbl[PIC_irq_source])();
    PIC_enable_irq(PIC_irq_source);
  }
}

void irq_init(void){
  u8 i;
  for(i=0; i<32; i++){
    PIC_irq_handler_tbl[i] = void_interrupt_handler;
    SIC_irq_handler_tbl[i] = void_interrupt_handler;
  }
  enable_interrupts();
}
