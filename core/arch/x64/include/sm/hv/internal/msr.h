/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_HV_MSR_H
#define VSM_HV_MSR_H

#include <compiler.h>
#include <stdint.h>

/* Model-Specific Register (MSR) Identifiers */
#define HV_X64_MSR_GUEST_OS_ID     0x40000000
#define HV_X64_MSR_HYPERCALL       0x40000001
#define HV_X64_MSR_VP_ASSIST_PAGE  0x40000073

/* Internal structure of the HV_X64_MSR_HYPERCALL synthetic MSR */
union hv_x64_msr_hypercall_contents {
	uint64_t as_uint64;

	struct {
		uint64_t enable                 : 1;
		uint64_t reserved               : 11;
		uint64_t guest_physical_address : 52;
	} __packed;
};

/* Internal structure of the HV_X64_VP_ASSIST_PAGE synthetic MSR */
union hv_x64_msr_vp_assist_page_contents {
	uint64_t as_uint64;

	struct {
		uint64_t enable                      : 1;
		uint64_t reserved                    : 11;
		uint64_t vp_assist_page_base_address : 52;
	} __packed;
};

#endif /* VSM_HV_MSR_H */
