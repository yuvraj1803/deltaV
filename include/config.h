#ifndef __config_h__
#define __config_h__

#define SET(value, bit)		(value << bit)

// EL3
#define EL3_SCR 		SET(1,10) | SET(1,8) | SET(1,0)
//				    RW		HCE	    NS
#define EL3_SPSR		SET(7,6) | SET(9,0)
//			         I+F MASK    EL2h


// EL2
#define EL2_STACK_BASE		0x80000 - 0x1
#define EL2_HCR			SET(1,21) | SET(1,18) | SET(1,17) | SET(1,16) | SET(1,14) | SET(1,13) | SET(0,34 | SET(1,31) | SET(0,27) | SET(1,5) | SET(1,4) | SET(1,3) | SET(1,1) | SET(1,0)
//				TACR	    TID3       	TID2        TID1        TWE         TWI         E2H        RW          TGE         AMO        IMO        FMO        SWIO       VM
#define EL2_SPSR		0
#define EL2_VTCR
#define EL2_SCTLR


#define FS_TOTAL_FILE_DESCRIPTORS 1024
#define FS_MAX_PATH_UNIT_LENGTH	  108
#define FS_MAX_PATH_LENGTH        1024

#endif
