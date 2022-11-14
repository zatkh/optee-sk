/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_HV_HYPERV_H
#define VSM_HV_HYPERV_H

#include <tee_api_defines.h>
#include <tee_api_types.h>

#include <sm/hv/internal/assist.h>
#include <sm/hv/internal/call.h>
#include <sm/hv/internal/msr.h>
#include <sm/hv/internal/register.h>

/* Guest OS Identifiers */
#define LINUX_VERSION_CODE  266002
#define PKG_ABI             0

#define HV_CANONICAL_VENDOR_ID  0x80
#define HV_LINUX_VENDOR_ID      0x8100

/* Partition Identifiers */
#define HV_PARTITION_ID_SELF  ((uint64_t)-1)

/* Virtual Processor (VP) Identifiers */
#define HV_VP_INDEX_SELF  ((uint32_t)-2)
#define HV_VP_ANY_VP	  ((uint32_t)-1)

/* Hypercall Status Codes */
enum hv_status {
	HvStatusSuccess               = 0,
	HvStatusInvalidHypercallCode  = 2,
	HvStatusInvalidHypercallInput = 3,
	HvStatusInvalidAlignment      = 4,
	HvStatusInsufficientMemory    = 11,
	HvStatusInvalidConnectionId   = 18,
	HvStatusInsufficientBuffers   = 19
};

/* Public Hyper-V Functions */
void hv_init(void);

union hv_vp_assist_page *hv_get_vp_assist_page(void);

uint64_t hv_hypercall_raw(uint64_t control, void *input_page,
	void *output_page);

enum hv_status hv_hypercall(enum hv_call_code call_code, void *input_page,
	void *output_page, uint32_t count_of_elements,
	uint32_t *elements_processed);

enum hv_status hv_rep_hypercall(enum hv_call_code call_code, uint16_t rep_count,
	uint16_t varhead_size, void *input_page, void *output_page);

TEE_Result hv_get_vsm_code_page_offsets(uint16_t *vtl_call_offset,
	uint16_t *vtl_return_offset);

TEE_Result hv_set_vsm_partition_info(bool enable_vtl_protection,
	uint8_t default_vtl_protection_mask);

TEE_Result hv_modify_vtl_protection_mask(uint64_t gpa_page_list[],
	size_t *number_of_pages, uint32_t page_access);

#endif /* VSM_HV_HYPERV_H */
