#ifndef __mm_h__
#define __mm_h__

#include <stdint.h>

#define PERIPH_BASE		0x3f000000
#define AUX_BASE		PERIPH_BASE + 0x215000
#define GPIO_BASE		PERIPH_BASE + 0x200000

#define AUX_IRQ 		(AUX_BASE + 0x0)
#define AUX_ENABLES 		(AUX_BASE + 0x4)
#define AUX_MU_IO_REG 		(AUX_BASE + 0x40)
#define AUX_MU_IER_REG 		(AUX_BASE + 0x44)
#define AUX_MU_IIR_REG 		(AUX_BASE + 0x48)
#define AUX_MU_LCR_REG 		(AUX_BASE + 0x4c)
#define AUX_MU_MCR_REG 		(AUX_BASE + 0x50)
#define AUX_MU_LSR_REG 		(AUX_BASE + 0x54)
#define AUX_MU_MSR_REG 		(AUX_BASE + 0x58)
#define AUX_MU_SCRATCH 		(AUX_BASE + 0x5c)
#define AUX_MU_CNTL_REG 	(AUX_BASE + 0x60)
#define AUX_MU_STAT_REG 	(AUX_BASE + 0x64)
#define AUX_MU_BAUD_REG 	(AUX_BASE + 0x68)
#define AUX_SPI0_CNTL0_REG 	(AUX_BASE + 0x80)
#define AUX_SPI0_CNTL1_REG 	(AUX_BASE + 0x84)
#define AUX_SPI0_STAT_REG 	(AUX_BASE + 0x88)
#define AUX_SPI0_IO_REG 	(AUX_BASE + 0x90)
#define AUX_SPI0_PEEK_REG 	(AUX_BASE + 0x94)
#define AUX_SPI1_CNTL0_REG 	(AUX_BASE + 0xc0)
#define AUX_SPI1_CNTL1_REG 	(AUX_BASE + 0xc4)


#define GPFSEL0         	(GPIO_BASE+0x00)
#define GPFSEL1         	(GPIO_BASE+0x04)
#define GPFSEL2         	(GPIO_BASE+0x08)
#define GPFSEL3         	(GPIO_BASE+0x0C)
#define GPFSEL4         	(GPIO_BASE+0x10)
#define GPFSEL5         	(GPIO_BASE+0x14)
#define GPSET0          	(GPIO_BASE+0x1C)
#define GPSET1          	(GPIO_BASE+0x20)
#define GPCLR0          	(GPIO_BASE+0x28)
#define GPLEV0         		(GPIO_BASE+0x34)
#define GPLEV1          	(GPIO_BASE+0x38)
#define GPEDS0          	(GPIO_BASE+0x40)
#define GPEDS1          	(GPIO_BASE+0x44)
#define GPHEN0          	(GPIO_BASE+0x64)
#define GPHEN1          	(GPIO_BASE+0x68)
#define GPPUD           	(GPIO_BASE+0x94)
#define GPPUDCLK0       	(GPIO_BASE+0x98)
#define GPPUDCLK1       	(GPIO_BASE+0x9C)

#define	HEAP_LOW		0x400000
#define HEAP_HIGH		0x3F000000
#define PAGING_MEM		(HEAP_HIGH - HEAP_LOW)
#define PAGE_SIZE		0x1000
#define TOTAL_PAGES		(PAGING_MEM/PAGE_SIZE)


void 	 mm_w(uint64_t reg, uint64_t val);
uint64_t mm_r(uint64_t reg);
void* malloc(uint64_t size);
void free(void* base);

#endif
