/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


.globl mm_w32
.globl mm_r32

mm_r32:
	ldr w0, [x0]
	ret
mm_w32:
	str w1, [x0]
	ret

