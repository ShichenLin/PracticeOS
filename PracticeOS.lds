OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS{
  .= 0x00000000;
  .text: {
    *(.text*);
  }
  .ALIGN(4)
  .data: {
    *(.data*);
  }
  .AILGH(4)
  .bss: {
    _bss_start = .;
    *(.bss*);
    _bss_end = .;
  }
  _program_end = .;

	.rel.dyn : {
    _rel_dyn_start;
		*(.rel*);
    _rel_dyn_end = .;
	}
}
