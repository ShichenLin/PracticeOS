/*******************************************************************************
* This is a first-stage bootloader for Versatile (ARM926ejs)
*******************************************************************************/
#include "boot.h"
#include "../include/versatile.h"
#include "../kernel/interrupts.h"

/*******************************************************************************
* Start Function (Link Entry Point)
*******************************************************************************/
.globl _start
_start:
  b reset
  ldr pc, =_undefined_instruction
  ldr pc, =_software_interrupt
  ldr pc, =_prefetch_abort
  ldr pc, =_data_abort
  ldr pc, =_not_used
  ldr pc, =_irq
  ldr pc, =_fiq

/*******************************************************************************
* Reset Function
*******************************************************************************/
.globl	reset
.type reset, %function
reset:
  /* Enter supervisor mode to disable interrupts */
  mrs	r0, cpsr
	bic	r0, r0, #0x0000001f
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
  orr r0, r0, #0x00001000 @ clear bit 12 (I), enable I-cache
  bic	r0, r0, #0x00000300 @ clear bits 9 and 8 (RS), disable ROM and system protection
	orr	r0, r0, #0x00000007 @ set bits 2, 1 and 0 (CAM), enable D-cache, alignment fault checking and MMU
  mcr	p15, 0, r0, c1, c0, 0

  /* Wait for cache initialization (around 256 cycles) */
  mov r0, #100
  mov r1, #0
cache_wait:
  sub r0, r0, #1
  cmp r0, r1
  bne cache_wait

  /* Jump to ROM section */
  mov r0, #0
  add r0, r0, =code_offset + VERSATILE_ROM_START
  mov pc, r0 @ jump to the next instruction in ROM position

code_offset:
  /* Unmap the memory */
  ldr r0, =VERSATILE_SYSTEM_BASE
  bic r0, r0, #0x000000100 @ clear bit 8, clear the DEVCHIP REMAP in system control register
  str r0, =VERSATILE_SYSTEM_BASE

  /* Initialize SDRAM based on register values for typical operation in versatile reference manual */
  mov r0, #0x0
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x008
  mov r0, #0x3
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x020
  mov r0, #0x22
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x024
  mov r0, #0x111
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x028
  mov r0, #0x2
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x030
  mov r0, #0x3
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x034
  mov r0, #0x5
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x038
  mov r0, #0x4
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x044
  mov r0, #0x5
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x048
  mov r0, #0x5
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x04c
  mov r0, #0x5
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x050
  mov r0, #0x1
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x054
  mov r0, #0x2
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x058
  mov r0, #0x1
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x05c
  mov r0, #0x5880
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x100
  mov r0, #0x202
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x104
  mov r0, #0x5880
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x120
  mov r0, #0x202
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x124
  mov r0, #0x5880
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x140
  mov r0, #0x202
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x144
  mov r0, #0x5880
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x160
  mov r0, #0x202
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x164
  mov r0, #0x0
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x400
  mov r0, #0x2
  str r0, =VERSATILE_MPMC_REGS_BASE + 0x408
  mov r0, #0x1
  str r0, =VERSATILE_MPMC_REGS_BASE @ enable SDRAM

  /* Wait for SDRAM initialization (around a few hundred us) */
  mov r0, #10000
  mov r1, #0
sdram_wait:
  sub r0, r0, #1
  cmp r0, r1
  bne sdram_wait

  /* Relocate the code and jump to SDRAM */
relocate_code:
  ldr r2, =_program_end
  ldr r0, #0
  ldr r1, =_start
  ldmia	r1!, {r10-r11}
	stmia	r0!, {r10-r11}
  cmp r2, r1
  blo relocate_code

  /* fix .rel.dyn relocations*/
	ldr	r2, =_rel_dyn_start
	ldr	r3, =_rel_dyn_end
fixloop:
	ldmia	r2!, {r0-r1}
	and	r1, r1, #0xff
	cmp	r1, #23
	bne	fixnext

	/* relative fix: increase location by offset */
	add	r0, r0, r4
	ldr	r1, [r0]
	add	r1, r1, r4
	str	r1, [r0]
fixnext:
	cmp	r2, r3
	blo	fixloop

  ldr r3, =clear_bss - VERSATILE_ROM_START
  mov pc, r3

  /* Set bss variables to 0 */
clear_bss:
  mov r0, #0
  ldr r1, =_bss_start
  ldr r2, =_bss_end
  stmia r1!, {r0}
  cmp r1, r2
  bne clear_bss

  /* Set up stack pointer and jump to C codes */
  ldr r0, =VERSATILE_SDRAM_END
  bic	r0, r0, #7 @ 8-byte alignment for ARM stack
	mov	sp, r0
	b	board_init

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

/* Undefined exception handlers */
undefined_instruction:
software_interrupt:
prefetch_abort:
data_abort:
not_used:
fiq:
1:
	b	1b			/* hang up and never return */

/* IRQ routine */
irq:
  /* Save registers to stack, enter IRQ mode and jump to generic_interrupt_handler */
  sub lr, lr, #4 @ set lr to the interrupted instruction
  stmfd sp!, {r0-r12, lr}
  mrs	r0, cpsr
  msr spsr, r0
	bic	r0, r0, #0x0000001f
	orr	r0, r0, #0x00000012		@ set IRQ mode
  msr cpsr, r0
  bl generic_interrupt_handler

  /* Get the saved registers back from stack and return to the interrupted instruction */
  mrs r0, spsr
  msr cpsr, r0
  ldmia sp!, {r0-r12, lr}
  mov pc, lr
