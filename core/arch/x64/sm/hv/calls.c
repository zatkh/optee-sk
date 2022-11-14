/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */


#include <memory.h>
#include <trace.h>

#include <sm/hv/hyperv.h>
#include <sm/irq.h>

char hv_hypercall_input_page[VSM_PAGE_SIZE] __aligned(VSM_PAGE_SIZE);
char hv_hypercall_output_page[VSM_PAGE_SIZE] __aligned(VSM_PAGE_SIZE);

static void *hv_acquire_hypercall_input_page(void)
{
	memset(hv_hypercall_input_page, 0, sizeof(hv_hypercall_input_page));

	return hv_hypercall_input_page;
}

static void *hv_acquire_hypercall_output_page(void)
{
	memset(hv_hypercall_output_page, 0, sizeof(hv_hypercall_output_page));

	return hv_hypercall_output_page;
}

TEE_Result hv_get_vsm_code_page_offsets(uint16_t *vtl_call_offset,
	uint16_t *vtl_return_offset)
{
	int ret;

	uint64_t flags;

	/* Input parameters for HvGetVpRegisters hypercall */
	struct hv_input_get_vp_registers *hvin;

	/* Return value for the same */
	union hv_register_value *hvout;

	/* Internal structure of the resulting register value */
	union hv_register_vsm_code_page_offsets vsm_code_page_offsets;

	/* Acquire the input and output pages */
	hvin = hv_acquire_hypercall_input_page();
	hvout = hv_acquire_hypercall_output_page();

	/* Fill in the hypercall parameters */
	hvin->partition_id = HV_PARTITION_ID_SELF;
	hvin->vp_index = HV_VP_INDEX_SELF;
	hvin->input_vtl.as_u8 = 0;
	hvin->reserved8_z = 0;
	hvin->reserved16_z = 0;
	hvin->names[0] = HvRegisterVsmCodePageOffsets;

	/* Disable interrupts */
	flags = sk_irq_save();

	/* Perform the hypercall */
	ret = hv_rep_hypercall(HvCallGetVpRegisters, 1, 0, hvin, hvout);

	/* Enable interrupts */
	sk_irq_restore(flags);

	/* Check the return value, as provided by Hyper-V */
	if (ret != HvStatusSuccess)
		return TEE_ERROR_GENERIC;

	/* Retrieve and parse the result */
	vsm_code_page_offsets =
		(union hv_register_vsm_code_page_offsets)hvout->as_u64;

	/* Pass the retrieved values up, as required */
	if (vtl_call_offset)
		*vtl_call_offset = vsm_code_page_offsets.vtl_call_offset;
	if (vtl_return_offset)
		*vtl_return_offset = vsm_code_page_offsets.vtl_return_offset;

	/* Done */
	return TEE_SUCCESS;
}

TEE_Result hv_set_vsm_partition_info(bool enable_vtl_protection,
	uint8_t default_vtl_protection_mask)
{
	enum hv_status status;

	uint64_t flags;

	struct hv_input_set_vp_registers *hvin;

	union hv_register_value register_value;
	union hv_register_vsm_partition_config *vsm_partition_config;

	uint32_t elements_processed;

	/* Enable and set default VTL protections */
	vsm_partition_config = (union hv_register_vsm_partition_config *)
		&register_value;
	vsm_partition_config->as_u64 = 0;
	vsm_partition_config->enable_vtl_protection = enable_vtl_protection;
	vsm_partition_config->default_vtl_protection_mask =
		default_vtl_protection_mask;

	/* Acquire the input page */
	hvin = hv_acquire_hypercall_input_page();

	/* Fill in the hypercall parameters */
	hvin->partition_id = HV_PARTITION_ID_SELF;
	hvin->vp_index = HV_VP_INDEX_SELF;
	hvin->input_vtl.as_u8 = 0;
	hvin->reserved8_z = 0;
	hvin->reserved16_z = 0;
	hvin->elements[0].name = HvRegisterVsmPartitionConfig;
	hvin->elements[0].value = register_value;

	/* Disable interrupts */
	flags = sk_irq_save();

	/* Perform the hypercall */
	status = hv_hypercall(HvCallSetVpRegisters, hvin, NULL, 1,
		&elements_processed);

	/* Enable interrupts */
	sk_irq_restore(flags);

	/* Check the return value, as provided by Hyper-V */
	if (status != HvStatusSuccess)
		return TEE_ERROR_GENERIC;

	/* Done */
	return TEE_SUCCESS;
}

TEE_Result hv_modify_vtl_protection_mask(uint64_t gpa_page_list[],
	size_t *number_of_pages, uint32_t page_access)
{
	enum hv_status status;

	uint64_t flags;

	int64_t pages_remaining;
	uint32_t pages_processed;
	uint32_t total_pages_processed;
	size_t max_pages_per_request;

	uint32_t i;

	struct hv_input_modify_vtl_protection_mask *hvin;

	/* Check parameters */
	if (!gpa_page_list || !number_of_pages || *number_of_pages >= UINT32_MAX)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Compute the maximum number of pages that can be processed in one go */
	max_pages_per_request = (VSM_PAGE_SIZE - sizeof(*hvin)) / sizeof(uint64_t);

	/* Acquire the input page */
	hvin = hv_acquire_hypercall_input_page();

	/* Fill in the hypercall parameters */
	hvin->partition_id = HV_PARTITION_ID_SELF;
	hvin->map_flags = page_access;

	/*
	 * Batch-process pages based on the maximum number of pages that can be
	 * processed in a single hypercall
	 */
	pages_processed = 0;
	total_pages_processed = 0;
	pages_remaining = *number_of_pages;

	while (pages_remaining > 0) {
		for (i = 0 ;
			 ((i < max_pages_per_request) &&
				((total_pages_processed + i) < *number_of_pages)) ;
			 i++)
			hvin->gpa_page_list[i] = gpa_page_list[total_pages_processed + i];

		/* Disable interrupts */
		flags = sk_irq_save();

		/* Perform the hypercall */
		status = hv_hypercall(HvCallModifyVtlProtectionMask, hvin, NULL, i,
			&pages_processed);

		/* Enable interrupts */
		sk_irq_restore(flags);

		/*
		 * Update page accounting for the next iteration, if any
		 *
		 * N.B.: pages_processed is correct even if Hyper-V returned an error.
		 */
		pages_remaining -= pages_processed;
		total_pages_processed += pages_processed;

		/* See how things went */
		if (status != HvStatusSuccess)
			break;
	}

	/* Pass results out (valid on error) */
	*number_of_pages = total_pages_processed;

	/* Done */
	return status == HvStatusSuccess ? TEE_SUCCESS : TEE_ERROR_GENERIC;
}
