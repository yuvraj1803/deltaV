#ifndef __MMU_H__
#define __MMU_H__

#define PAGE_SIZE                               0x1000
#define MMU_DESCRIPTOR_TABLE_DESCRIPTOR_FLAG    0b11
#define MMU_DESCRIPTOR_TABLE_BLOCK_FLAG         0b01

#define MAIR_HYPERVISOR_MEM_ATTR_INDEX               0
#define MAIR_HYPERVISOR_MMIO_ATTR_INDEX              1      // nGnRnE
#define MAIR_HYPERVISOR_MEM_FLAGS               0b01000100  // Normal Memory Inner Non-Cacheable
#define MAIR_HYPERVISOR_MMIO_FLAGS              0x0         // nGnRnE
#define EL2_MAIR                                (MAIR_HYPERVISOR_MEM_FLAGS << (8 * MAIR_HYPERVISOR_MEM_ATTR_INDEX) | MAIR_HYPERVISOR_MMIO_FLAGS << (8 * MAIR_HYPERVISOR_MMIO_ATTR_INDEX))

#define MMU_AF                                  (1U << 10) // ELx R+W
#define MMU_SH                                  (3U << 8)  // Inner Shareable


#define MMU_HYPERVISOR_MEM_FLAGS                (MMU_DESCRIPTOR_TABLE_BLOCK_FLAG | MMU_AF | MMU_SH | (MAIR_HYPERVISOR_MEM_ATTR_INDEX << 2))
#define MMU_HYPERVISOR_MMIO_FLAGS               (MMU_DESCRIPTOR_TABLE_BLOCK_FLAG | MMU_AF | MMU_SH | (MAIR_HYPERVISOR_MMIO_ATTR_INDEX << 2))


#endif