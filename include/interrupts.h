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
