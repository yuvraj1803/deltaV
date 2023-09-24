#ifndef __config_h__
#define __config_h__

#define CONFIG_VM_CONSOLE_BUFFER_SIZE	512
#define CONFIG_MAX_VMs					256
#define CONFIG_SCHED_QUANTA				100 // in milliseconds




// EL3
#define EL3_SCR 	/*RW*/	       (1U << 10)  |\
			/*HCE*/        (1U << 8)   |\
			/*NS*/         (1U << 0)
			
#define EL3_SPSR	/*I+F MASK*/	(7 << 6)   |\
			/*EL2h*/	(9 << 0)



// EL2
#define EL2_STACK_BASE		0x80000 - 0x1

#define EL2_HCR	    /*TACR*/	 (1 << 21) |  \
		    /*TID3*/	 (1 << 18) |  \
		    /*TID2*/     (1 << 17) |  \
	            /*TID1*/	 (1 << 16) |  \
		    /*TWE*/	 (1 << 14) |  \
		    /*TWI*/	 (1 << 13) |  \
		    /*E2H*/	 (0 << 34) |  \
		    /*RW*/	 (1 << 31) |  \
      		    /*TGE*/	 (0 << 27) |  \
		    /*AMO*/	 (1 << 5) |   \
		    /*IMO*/	 (1 << 4) |   \
		    /*FMO*/	 (1 << 3) |   \
		    /*SWIO*/	 (1 << 1) |   \
		    /*VM*/	 (1 << 0)
		 

#define EL2_VTCR	/*NSA*/		(1 << 30) | \
			/*NSW*/		(1 << 29) | \
			/*VS*/		(0 << 19) | \
			/*PS*/		(2 << 16) | \
			/*TG0*/		(0 << 14) | \
			/*SH0*/		(3 << 12) | \
			/*ORGN0*/	(0 << 10) | \
			/*IRGN0*/	(0 << 8)  | \
			/*SL0*/		(1 << 6)  | \
			/*T0SZ*/	(64 - 38)


#define EL2_SCTLR_NO_MMU	/*EE*/		(0 << 25) | \
				/*I-CACHE*/	(0 < 12)  | \
				/*D-CACHE*/	(0 << 2)  | \
				/*MMU*/		(0 << 0)
#define EL2_SCTLR_WITH_MMU	EL2_SCTLR_NO_MMU | (1 << 0)

#define EL2_SPSR		EL3_SPSR
#define EL2_TCR	   /*T0SZ*/	(64-38) | \
		   /*4K TG0*/	(0 << 14)


#endif
