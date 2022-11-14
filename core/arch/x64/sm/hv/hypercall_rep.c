/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <sm/hv/hyperv.h>

enum hv_status hv_rep_hypercall(enum hv_call_code call_code, uint16_t rep_count,
	uint16_t varhead_size, void *input_page, void *output_page)
{
	uint64_t control = call_code;
	uint64_t status;
	uint16_t rep_comp;

	control |= (uint64_t)varhead_size << HV_HYPERCALL_VARHEAD_OFFSET;
	control |= (uint64_t)rep_count << HV_HYPERCALL_REP_COMP_OFFSET;

	do {
		status = hv_hypercall_raw(control, input_page, output_page);
		if ((status & HV_HYPERCALL_RESULT_MASK) != HvStatusSuccess)
			return status;

		rep_comp = (status & HV_HYPERCALL_REP_COMP_MASK) >>
			HV_HYPERCALL_REP_COMP_OFFSET;

		control &= ~HV_HYPERCALL_REP_START_MASK;
		control |= (uint64_t)rep_comp << HV_HYPERCALL_REP_START_OFFSET;
	} while (rep_comp < rep_count);

	return (enum hv_status)(status & HV_HYPERCALL_REP_COMP_MASK);
}
