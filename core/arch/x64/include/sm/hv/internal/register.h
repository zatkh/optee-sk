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

union hv_register_vsm_vp_status {
	uint64_t as_u64;

	struct {
		uint64_t active_vtl : 4;
		uint64_t active_mbec_enabled : 1;
		uint64_t reserved_z0 : 11;
		uint64_t enabled_vtl_set : 16;
		uint64_t reserved_z1 : 32;
	};
};

union hv_register_vsm_capabilities {
	uint64_t as_u64;

	struct {
		uint64_t dr_shared : 1;
		uint64_t mbec_vtl_mask : 16;
		uint64_t deny_lower_vtl_startup : 1;
		uint64_t reserved_z : 46;
	};
};

enum t_hv_register_name {
	HvRegisterVsmVpSecureConfigVtl0 = 0x000D0010,
	HvRegisterVsmVpSecureConfigVtl1 = 0x000D0011,
};

union t_hv_register_value {
	uint64_t as_u64;
	uint32_t as_u32;
	uint16_t as_u16;
	uint8_t as_u8;
};

union hv_register_vsm_vp_secure_vtl_config {
	uint64_t as_u64;

	struct {
		uint64_t mbec_enabled : 1;
		uint64_t tlb_locked : 1;
		uint64_t reserved: 62;
	};
};

#endif /* VSM_HV_REGISTER_H */
