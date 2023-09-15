#ifndef __config_h__
#define __config_h__


// EL3
#define EL3_SCR 		(1U << 10)  | (1U << 8) | (1 << 0)
//				    RW		HCE	    NS
#define EL3_SPSR		(7 << 6) |  (9 << 0)
//			         I+F MASK    EL2h


// EL2
#define EL2_STACK_BASE		0x80000 - 0x1
#define EL2_HCR			 (1 << 21) |  (1 << 18) |  (1 << 17) |  (1 << 16) |  (1 << 14) |  (1 << 13) |  (0 << 34) |  (1 << 31) |  (0 << 27) |  (1 << 5) |  (1 << 4) |  (1 << 3) |  (1 << 1) |  (1 << 0)
//				  TACR	        TID3   	      TID2        TID1        TWE           TWI            E2H         RW           TGE          AMO         IMO         FMO        SWIO       VM
#define EL2_VTCR		(1 << 30) | (1 << 29) | (0 << 19) | (2 << 16) | (0 << 14) | (3 << 12) | (0 << 10) | (0 << 8) | (1 << 6) | (64 - 38)
//				   NSA         NSW        VS          PS           TG0        SH0         ORGN0        IRGN0     SL0        T0SZ
#define EL2_SCTLR_NO_MMU	(0 << 25) | (0 < 12) | (0 << 2) | (0 << 0)
// 				  EE      I-CACHE->0  D-CACHE->0  MMU-OFF
#define EL2_SCTLR_WITH_MMU	EL2_SCTLR_NO_MMU | (1 << 0)
#define EL2_MAIR
#define EL2_SPSR		EL3_SPSR
#define EL2_TCR


#define FS_TOTAL_FILE_DESCRIPTORS 1024
#define FS_MAX_PATH_UNIT_LENGTH	  108
#define FS_MAX_PATH_LENGTH        1024

#endif
