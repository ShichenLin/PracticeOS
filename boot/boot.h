#define enable_caches_and_mmu() \
  __asm__( \
    "mrc	p15, 0, r0, c1, c0, 0" \
    "orr	r0, r0, #0x00001305" \
    "mcr	p15, 0, r0, c1, c0, 0" \
  )
