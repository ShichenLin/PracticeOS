extern void reset(void);

#define disable_interrupts() \
  __asm__( \
    "mrs r0, cpsr" \
    "orr r0, r0, #0x000000c0" \
    "msr cpsr, r0" \
  )

#define enable_interrupts() \
  __asm__( \
    "mrs r0, cpsr" \
    "bic r0, r0, #0x000000c0" \
    "msr cpsr, r0" \
  )

/* bit positions of interrupt sources in IRQ status register of PIC */
#define PIC_WDOG_IRQ 0
#define PIC_SW_IRQ 1
#define PIC_DEBUG_RX_IRQ 2
#define PIC_DEBUG_TX_IRQ 3
#define PIC_TIMER01_IRQ 4
#define PIC_TIMER23_IRQ 5
#define PIC_GPIO0_IRQ 6
#define PIC_GPIO1_IRQ 7
#define PIC_GPIO2_IRQ 8
#define PIC_GPIO3_IRQ 9
#define PIC_RTC_IRQ 10
#define PIC_SSP_IRQ 11
#define PIC_UART0_IRQ 12
#define PIC_UART1_IRQ 13
#define PIC_UART2_IRQ 14
#define PIC_SCI_IRQ 15
#define PIC_CLCD_IRQ 16
#define PIC_DMA_IRQ 17
#define PIC_PWR_FAIL_IRQ 18
#define PIC_GPU_IRQ 19
#define PIC_DOC_FLASH_IRQ 21 //disk-on-chip flash interrupt
#define PIC_MCI0A_IRQ 22
#define PIC_AACI_IRQ 24
#define PIC_ETHERNET_IRQ 25
#define PIC_USB_IRQ 26
#define PIC_EXP_CONNECTOR0_IRQ 27 //expansion connector interrupt
#define PIC_EXP_CONNECTOR1_IRQ 28
#define PIC_SIC_IRQ 31 //secondary interrupt controller interrupt

/* bit positions of interrupt sources in IRQ status register of SIC*/
#define SIC_SW_IRQ 0
#define SIC_MMCI0B_IRQ 1 //multimedia card 0B interrupt
#define SIC_KEYBOARD_IRQ 3
#define SIC_MOUSE_IRQ 4
#define SIC_TOUCHSCREEN_IRQ 8
#define SIC_KEYPAD_IRQ 9
#define SIC_DOC_FLASH_IRQ 21
#define SIC_MMCI0A_IRQ 22 //multimedia card 0A interrupt
#define SIC_AACI_IRQ 24 //audio codec interface interrupt
#define SIC_ETHERNET_IRQ 25
#define SIC_USB_IRQ 26

void (*PIC_irq_handler_tbl[32])(void);
void (*SIC_irq_handler_tbl[32])(void);

void generic_interrupt_handler(void);
void irq_init(void);
