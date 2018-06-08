/*******************************************************************************
* This is a first-stage bootloader for Versatile (ARM926ejs)
*******************************************************************************/
#include "include/versatile.h"
#include "boot.h"

.thumb

.globl text_start, text_end, data_start, data_end, bss_start, bss_end
.text
text_start:
.data
data_start:
.bss
bss_start:
.text

/*******************************************************************************
* Start Function (Link Entry Point)
*******************************************************************************/
.globl _start
_start:
  b reset

/*******************************************************************************
* Reset Function
*******************************************************************************/
.globl	reset
.type reset, %function
reset:
  /* Enter supervisor mode to disable interrupts */
  mrs	r0, cpsr
	bic	r0, r0, #0x0000001f		@ clear all mode bits
	orr	r0, r0, #0x00000013		@ set SVC mode
	orr	r0, r0, #0x000000c0		@ disable FIQ and IRQ
	msr	cpsr, r0

  /* Invalidate and disable caches, TLBs and MMU */
  mov	r0, #0
  mrc	p15, 0, r15, c7, c10, 3 @ flush D-cache
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate I-TLB and D-TLB
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate I-cache
  mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bit 13 (V), normal exception vectors selected
  bic r0, r0, #0x00001000 @ clear bit 12 (I), disable I-cache
  bic	r0, r0, #0x00000300 @ clear bits 9 and 8 (RS), disable ROM and system protection
	bic	r0, r0, #0x00000005	@ clear bits 2 and 0 (CM), disable D-cache and MMU
	orr	r0, r0, #0x00000002	@ set bit 1 (A), enable alignment fault checking
  mcr	p15, 0, r0, c1, c0, 0
  b #0x34001000

  . = 0x1000
  /* Set up stack pointer and jump to C codes */
  mov r0, =VERSATILE_SDRAM_END
  bic	r0, r0, #3 @ 4-byte alignment for 32-bit instruction
	mov	sp, r0 @ initialize sp
	bl	board_init

/*******************************************************************************
* Indirect Vector Table
*******************************************************************************/
.globl	_undefined_instruction
.globl	_software_interrupt
.globl	_prefetch_abort
.globl	_data_abort
.globl	_not_used
.globl	_irq
.globl	_fiq

_undefined_instruction:	.word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq

undefined_instruction:
software_interrupt:
prefetch_abort:
data_abort:
not_used:
irq:
fiq:
1:
	bl	1b			/* hang and never return */
