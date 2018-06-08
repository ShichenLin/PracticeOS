#include "types.h"

/* generic virtual read and write */
#define getb(a) (*(volatile u8 *)(a))
#define gethw(a) (*(volatile u16 *)(a))
#define getw(a) (*(volatile u32 *)(a))
#define getdw(a) (*(volatile u64 *)(a))

#define putb(a, d) (*(volatile u8 *)(a) = (d))
#define puthw(a, d) (*(volatile u16 *)(a) = (d))
#define putw(a, d) (*(volatile u32 *)(a) = (d))
#define putdw(a, d) (*(volatile u64 *)(a) = (d))

/* gpio input and output */
#define gpio_in(port, pin)

#define gpio_out(port, pin, value)
