
/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <trace.h>

#include <mm/core_mmu.h>

#include <sm/hv/hyperv.h>
#include <sm/vsm/vsm.h>

static TEE_Result vsm_protect_page_range(paddr_t start, size_t size)
{
	TEE_Result res;

	size_t i;
	size_t page_count;

	uint64_t *pfns;

	/* Assert that the region to protect is page-aligned */
	if (((start % VSM_PAGE_SIZE)) != 0 || ((size % VSM_PAGE_SIZE) != 0))
		return TEE_ERROR_BAD_PARAMETERS;

	/* Compute the number of pages to protect */
	page_count = size / VSM_PAGE_SIZE;

	/* Set up the list of Page Frame Numbers (PFNs) */
	pfns = malloc(page_count * sizeof(*pfns));
	if (!pfns)
		return TEE_ERROR_OUT_OF_MEMORY;

	for (i = 0 ; i < page_count ; i++)
		pfns[i] = VSM_PAGE_TO_PFN(VSM_PAGE_AT(start, i));

	/* Revoke all VTL 0 access rights on these pages */
	res = hv_modify_vtl_protection_mask(pfns, &page_count, HV_PAGE_ACCESS_NONE);

	/* Free the PFN list */
	free(pfns);

	return res;
}

TEE_Result vsm_protect_optee_pages(void)
{
	TEE_Result res;

	size_t i;
	paddr_t start;
	paddr_t end;

	/* List of secure memory regions */
	const enum teecore_memtypes types[] = {
		MEM_AREA_TEE_RAM,
		MEM_AREA_TEE_RAM_RX,
		MEM_AREA_TEE_RAM_RO,
		MEM_AREA_TEE_RAM_RW,
		MEM_AREA_NEX_RAM_RW,
		MEM_AREA_TEE_COHERENT,
		MEM_AREA_TEE_ASAN,
		MEM_AREA_TA_RAM,
		MEM_AREA_RAM_SEC,
		MEM_AREA_IO_SEC,
		MEM_AREA_SEC_RAM_OVERALL,
		MEM_AREA_MAXTYPE
	};

	/* For each secure memory region in the memory map -> */
	for (i = 0 ; i < sizeof(types) ; i++) {
		/* See if a region of the type exists */
		core_mmu_get_pa_mem_by_type(types[i], &start, &end);
		if (start == 0 && end == 0)
			continue;

		/* If so, protect it from VTL 0 */
		DMSG("Protecting memory region: type %d, start 0x%lX, size 0x%lX",
			types[i], start, end - start);

		res = vsm_protect_page_range(start, end - start);

		DMSG(res == TEE_SUCCESS ? "  OK" : "  Failed");

		if (res != TEE_SUCCESS)
			break;
	}

	return res;
}

#ifdef CFG_LVBS_KERNEL_HVCI
TEE_Result vsm_restrict_memory(uint64_t start, size_t size, uint32_t permissions)
{
	TEE_Result res;

	size_t i;
	size_t page_count;

	uint64_t *pfns;

	/* Assert that the region to protect is page-aligned */
	if (((start % VSM_PAGE_SIZE)) != 0 || ((size % VSM_PAGE_SIZE) != 0))
		return TEE_ERROR_BAD_PARAMETERS;

	/* Compute the number of pages to protect */
	page_count = size / VSM_PAGE_SIZE;

	/* Set up the list of Page Frame Numbers (PFNs) */
	pfns = malloc(page_count * sizeof(*pfns));
	if (!pfns)
		return TEE_ERROR_OUT_OF_MEMORY;

	for (i = 0 ; i < page_count ; i++)
		pfns[i] = VSM_PAGE_TO_PFN(VSM_PAGE_AT(start, i));

	/* Revoke all VTL 0 access rights on these pages */
	res = hv_modify_vtl_protection_mask(pfns, &page_count, permissions);

	/* Free the PFN list */
	free(pfns);

	return res;
}
#endif