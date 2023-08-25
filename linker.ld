ENTRY(_start)

SECTIONS{
	. = 0x80000;
	.text.boot : { *(.text.boot) }
	
	. = ALIGN(4096);
	.text      : { *(.text) }
	. = ALIGN(4096);
	
	. = ALIGN(4096);
	.rodata    : { *(.rodata) }
	. = ALIGN(4096);

	. = ALIGN(4096);
	.data	   : { *(.data) }
	. = ALIGN(4096);

	. = ALIGN(4096);
	__bss_start = .;
	.bss	   : { *(.bss) }
	. = ALIGN(4096);
	__bss_end = .;

}
