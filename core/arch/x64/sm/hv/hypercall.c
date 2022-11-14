/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <sm/hv/hyperv.h>

enum hv_status hv_hypercall(enum hv_call_code call_code, void *input_page,
	void *output_page, uint32_t count_of_elements, uint32_t *elements_processed)
{
	union hv_hypercall_input input = { 0 };
	union hv_hypercall_output output = { 0 };

	input.call_code = call_code;
	input.count_of_elements = count_of_elements;

	output.as_uint64 = hv_hypercall_raw(input.as_uint64, input_page,
		output_page);

	if (elements_processed)
		*elements_processed = output.elements_processed;

	return output.call_status;
}
