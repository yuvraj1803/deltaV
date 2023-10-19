/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


.globl get_current_el

get_current_el:
	mrs x0, currentel
	lsr x0, x0, #2
	ret
