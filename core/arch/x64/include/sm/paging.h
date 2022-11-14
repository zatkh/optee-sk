/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_PAGING_H
#define VSM_PAGING_H

/* Defines the page size */
#define VSM_PAGE_SHIFT  12

/* Computed page size */
#define VSM_PAGE_SIZE  (((uint32_t)1) << VSM_PAGE_SHIFT)

/* Number of entries in a page table (all levels) */
#define VSM_ENTRIES_PER_PT  512

/* Compute the address of the page at the given index with the given base */
#define VSM_PAGE_AT(addr, idx)  ((addr) + (idx) * VSM_PAGE_SIZE)

/* Compute the address of the next page with the given base */
#define VSM_NEXT_PAGE(addr)  VSM_PAGE_AT((addr), 1)

/* Compute the page frame number (PFN) from a page address */
#define VSM_PAGE_TO_PFN(addr)  ((addr) / VSM_PAGE_SIZE)

#endif /* VSM_PAGING_H */
