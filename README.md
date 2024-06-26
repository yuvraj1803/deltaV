# deltaV - a baremetal hypervisor 
<img width="1086" alt="Screenshot 2023-08-25 at 9 24 37 AM" src="https://github.com/yuvraj1803/deltaV/assets/37622719/046f14d1-9ed9-49d0-8871-9f910e07c5dd">

 # deltaSSE support
  <img width="1233" alt="Screenshot 2023-11-03 at 4 04 10 PM" src="https://github.com/yuvraj1803/deltaV/assets/37622719/25d31856-bbb8-44d9-8da9-c32f09e11d55">
  
 # Build Dependencies
    + make
    + qemu-system-aarch64
    + gcc-aarch64-linux-gnu
 # Debug Dependencies
    + gdb-multiarch
 # How to use?
    + chmod +x install.sh
    + ./install.sh
    + make run
 # How to Debug?
    + make debug
    + (Another Terminal) gdb-multiarch ./deltaV.elf
    + (gdb) target remote localhost:1234


# EL2 Configuration

  **SCTLR.EE (CLEARED)** - data accesses and tables walks (stage 1&2) are little-endian.
  
  **SCTLR.M** - MMU enable/disable for stage 1 translation.
  
  **SCTLR.C (CLEARED)**- data access from EL2 non cacheable.
 
  **SCTLR.I (CLEARED)**- instruction access from EL2 non cacheable.
     
   ------------

  **HCR.TACR** - Auxiliary Control Registers read routed to EL2.
  
  **HCR.TID3** -  ID Group 3 Register read routed to EL2.
  
  **HCR.TID2** -  ID Group 2 Register read routed to EL2.
  
  **HCR.TID1** -  ID Group 1 Register read routed to EL2.
  
  **HCR.TWE** - WFE instruction in EL0/1 is routed to EL2.
  
  **HCR.TWI** - WFI instruction in EL0/1 is routed to EL2.
  
  **HCR.E2H (CLEARED)** - host OS in EL2 is not supported.
  
  **HCR.RW** - EL1 runs in AArch64.
  
  **HCR.TGE**(CLEARED) - EL0 execution no effect.
  
  **HCR.AMO** - SError routed to EL2.
  
  **HCR.IMO** - IRQs routed to EL2.
  
  **HCR.FMO**- FIQs routed to EL2.
    
  **HCR.VM** - Stage 2 translation enabled for EL0&1.
     
   ------------

  **SPSR.I**- IRQ interrupts masked on exception to EL2.
  
  **SPSR.F** - FIQ interrupts  masked on exception to EL2.
  
  **SPSR.A** - SError interrupts masked on exception to EL2.
  
  **SPSR.EL2h** - SP_EL2 selected on exception to EL2.
   
   ------------
  
  **TCR.T0SZ**- memory  addressed by TTBR0_EL2 is 2^48 bytes.
  
  **TCR.TG0 (CLEARED)**- TTBR0_EL2 granule 4KB.
   
   ------------
  
  **VTCR.NSA**- SEL0/1 stage 2 translations access Non-secure PA space.
  
  **VTCR.NSW** - SEL0/2 stage 2 table walks are to Non-secure PA space.
  
  **VTCR.VS (CLEARED)** - 8 bit VMIDs.
  
  **VTCR.PS**- 1TB PA size for stage 2 translations.
  
  **VTCR.TG0 (CLEARED)**- VTTBR_EL2 4KB granules.
  
  **VTCR.SH0** - Inner Sharable attribute for VTTBR_EL2 walks.
  
  **VTCR.ORGN0 (CLEARED)** - outer non-cacheable for VTTBR_EL2 walks.
  
  **VTCR.IRGN0 (CLEARED)**- inner non-cacheable for VTTBR_EL2 walks.
  
  **VTCR.SL0**- stage 2 translation lookups start at level 1.
  
  **VTCR.T0SZ**- memory addressed by VTTBR_EL2 is 38 bits.
  
     
   ------------
  
  
