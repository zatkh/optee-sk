/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <kernel/panic.h>

#include <mm/core_mmu.h>

#include <sm/hv/hyperv.h>
#include <sm/msr.h>
#include <sm/paging.h>
#include <sm/sk.h>

char hv_hypercall_page[VSM_PAGE_SIZE] __aligned(VSM_PAGE_SIZE);
char hv_vp_assist_page[VSM_PAGE_SIZE] __aligned(VSM_PAGE_SIZE);

void *hv_vtl_return;

static inline uint64_t hv_generate_guest_id(uint64_t d_info1,
	uint64_t kernel_version, uint64_t d_info2)
{
	uint64_t guest_id = 0;

	guest_id = (((uint64_t)HV_LINUX_VENDOR_ID) << 48);
	guest_id |= (d_info1 << 48);
	guest_id |= (kernel_version << 16);
	guest_id |= d_info2;

	return guest_id;
}

static void hv_init_hypercalls(void)
{
	int ret;

	uint64_t guest_id;
	uint64_t hypercall_pfn;

	union hv_x64_msr_hypercall_contents hypercall_msr;

	/* Set guest ID */
	guest_id = hv_generate_guest_id(
		HV_CANONICAL_VENDOR_ID,
		LINUX_VERSION_CODE,
		PKG_ABI);

	sk_write_msr(HV_X64_MSR_GUEST_OS_ID, guest_id);

	/* Enable hypercalls */
	hypercall_pfn = VSM_PAGE_TO_PFN((uint64_t)(void *)hv_hypercall_page);

	hypercall_msr.as_uint64 = sk_read_msr(HV_X64_MSR_HYPERCALL);
	hypercall_msr.enable = 1;
	hypercall_msr.guest_physical_address = hypercall_pfn;

	sk_write_msr(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);

	ret = arch_mmu_map((vaddr_t)hv_hypercall_page, (paddr_t)hv_hypercall_page,
		1, X86_MMU_PG_P);
	if (ret)
		panic("Cannot map hypercall code page as executable.");
}

static void hv_init_vp_assist_page(void)
{
	uint64_t vp_assist_pfn;

	union hv_x64_msr_vp_assist_page_contents vp_assist_page_msr;

	vp_assist_pfn = VSM_PAGE_TO_PFN((uint64_t)(void *)hv_vp_assist_page);

	vp_assist_page_msr.enable = 1;
	vp_assist_page_msr.reserved = 0;
	vp_assist_page_msr.vp_assist_page_base_address = vp_assist_pfn;

	sk_write_msr(HV_X64_MSR_VP_ASSIST_PAGE, vp_assist_page_msr.as_uint64);
}

static void hv_init_vsm_code_page(void)
{
	uint16_t vtl_return_offset;

	hv_get_vsm_code_page_offsets(NULL, &vtl_return_offset);

	hv_vtl_return = (void *)((uint64_t)hv_hypercall_page + vtl_return_offset);
}

static void hv_init_default_vtl_protections(void)
{
	TEE_Result res;

	res = hv_set_vsm_partition_config(true, HV_PAGE_ACCESS_ALL);
	if (res != TEE_SUCCESS)
		panic("Unable to enable and set default VTL protections.");
}

/* Returns a properly typed pointer to the VP assist page */
union hv_vp_assist_page *hv_get_vp_assist_page(void)
{
	return (union hv_vp_assist_page *)hv_vp_assist_page;
}

void hv_init(void)
{
	/* Initialize hypercalls */
	hv_init_hypercalls();

	/* Initialize VP assist page */
	hv_init_vp_assist_page();

	/* Flush the TLB */
	sk_flush_tlb();

	/* Initialize VSM code page */
	hv_init_vsm_code_page();

	/* Initialize default VTL protections (prereq for VTL 1 memory isolation) */
	hv_init_default_vtl_protections();
}
