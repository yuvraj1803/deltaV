ENTRY(_start)


SECTIONS{
	. = 0x80000;
	__delta_start = .;
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

	. = ALIGN(4096);
	__page_map_start = .;
	.page_map  : { *(.page_map) }
	. = ALIGN(4096);
	__page_map_end = .;

	__delta_end = .;
	. = ALIGN(4096);
	__page_dir_start = .;
	

	/DISCARD/ : {
		*(.comment)
		*(.gnu*)
		*(.note*)
		*(eh_frame*)
	}

}
