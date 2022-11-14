/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_HV_REGISTER_H
#define VSM_HV_REGISTER_H

#include <compiler.h>
#include <stdint.h>

/* Internal structure of the HvRegisterVsmCodePageOffsets virtual register */
union hv_register_vsm_code_page_offsets {
	uint64_t as_u64;

	struct {
		uint64_t vtl_call_offset   : 12;
		uint64_t vtl_return_offset : 12;
		uint64_t reserved_z        : 40;
	};
};

/* Internal structure of the HvRegisterVsmParitionConfig virtual register */
union hv_register_vsm_partition_config {
	uint64_t as_u64;

	struct {
		uint64_t enable_vtl_protection           : 1;
		uint64_t default_vtl_protection_mask     : 4;
		uint64_t zero_memory_on_reset            : 1;
		uint64_t deny_lower_vtl_startup          : 1;
		uint64_t intercept_acceptance            : 1;
		uint64_t intercept_enable_vtl_protection : 1;
		uint64_t intercept_vp_startup            : 1;
		uint64_t intercept_cpuid_unimplemented   : 1;
		uint64_t reserved_z                      : 53;
	};

};

#endif /* VSM_HV_REGISTER_H */
