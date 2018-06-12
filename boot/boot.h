#include "../include/versatile.h"

#define enable_caches_and_mmu() \
  __asm__( \
    "mrc	p15, 0, r0, c1, c0, 0" \
    "orr	r0, r0, #0x00001305" \
    "mcr	p15, 0, r0, c1, c0, 0" \
  )

#define unmap_memory() \
  __asm__( \
    "ldr r0, %0" \
    "bic r0, r0, #0x100" \
    "str r0, %0" \
    ::"r"(VERSATILE_SYSTEM_BASE): "r0" \
  )

unsigned char [0] __attribute__(section());
