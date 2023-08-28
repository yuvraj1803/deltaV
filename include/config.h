#ifndef __config_h__
#define __config_h__

#define EL3_STACK_BASE		0
#define EL3_VECTOR_TABLE	0
#define EL3_SPSR		0
#define EL3_SCR			0

#define EL2_VECTOR_TABLE	0
#define EL2_STACK_BASE		0x80000 - 0x1
#define EL2_HCR			0
#define EL2_SPSR		0



#define FS_TOTAL_FILE_DESCRIPTORS 1024
#define FS_MAX_PATH_UNIT_LENGTH	  108
#define FS_MAX_PATH_LENGTH        1024

#endif
