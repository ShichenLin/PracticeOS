OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
OUTPUT(PracticeOS.elf)

ENTRY(_start)

SECTIONS{
  . = 0x00000000;
  .text : {
    *(.text*);
  }
  . = ALIGN(4);

  .data : {
    *(.data*);
  }
  . = ALIGN(4);

  .bss : {
    _bss_start = .;
    *(.bss*);
    _bss_end = .;
  }
  _program_end = .;
  . = ALIGN(4);

	.rel.dyn : {
    _rel_dyn_start = .;
		*(.rel*);
    _rel_dyn_end = .;
	}
}
