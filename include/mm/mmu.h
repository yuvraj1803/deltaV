/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#ifndef __MMU_H__
#define __MMU_H__

#define PAGE_SIZE                               0x1000
#define MMU_TABLE_ENTRY_FLAG                    0b11
#define MMU_BLOCK_ENTRY_FLAG                    0b01

#define MAIR_HYPERVISOR_MEM_ATTR_INDEX               0
#define MAIR_HYPERVISOR_MMIO_ATTR_INDEX              1      // nGnRnE
#define MAIR_HYPERVISOR_MEM_FLAGS               0b01000100  // Normal Memory Inner Non-Cacheable
#define MAIR_HYPERVISOR_MMIO_FLAGS              0x0         // nGnRnE
#define EL2_MAIR                                (MAIR_HYPERVISOR_MEM_FLAGS << (8 * MAIR_HYPERVISOR_MEM_ATTR_INDEX) | MAIR_HYPERVISOR_MMIO_FLAGS << (8 * MAIR_HYPERVISOR_MMIO_ATTR_INDEX))

#define MMU_AF                                  (1U << 10) // Access Flag
#define MMU_SH                                  (3U << 8)  // Inner Shareable
#define MMU_AP                                  (3U << 6)  // ELx Read-Only
#define MMU_AP_NO_ACCESS                        (0U << 6)

#define MMU_HYPERVISOR_MEM_FLAGS                (MMU_BLOCK_ENTRY_FLAG | MMU_AF | MMU_SH | (MAIR_HYPERVISOR_MEM_ATTR_INDEX << 2))
#define MMU_HYPERVISOR_MMIO_FLAGS               (MMU_BLOCK_ENTRY_FLAG | MMU_AF | MMU_SH | (MAIR_HYPERVISOR_MMIO_ATTR_INDEX << 2))

#define MMU_STAGE_2_MEM_FLAGS                   (MMU_TABLE_ENTRY_FLAG | MMU_AP | MMU_SH | (MAIR_HYPERVISOR_MEM_ATTR_INDEX << 2) | MMU_AF)
#define MMU_STAGE_2_MMIO_FLAGS                  (MMU_TABLE_ENTRY_FLAG | MMU_AP_NO_ACCESS| MMU_SH | (MAIR_HYPERVISOR_MMIO_ATTR_INDEX << 2) | MMU_AF)

#endif
