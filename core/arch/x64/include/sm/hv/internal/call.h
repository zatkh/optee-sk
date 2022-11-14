/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_HV_CALL_H
#define VSM_HV_CALL_H

#include <compiler.h>
#include <stdint.h>

/* Hypercall Input & Output Constants */
#define HV_HYPERCALL_FAST_BIT          (1 << 16)
#define HV_HYPERCALL_VARHEAD_OFFSET	   17
#define HV_HYPERCALL_REP_COMP_OFFSET   32
#define HV_HYPERCALL_REP_START_OFFSET  48
#define HV_HYPERCALL_REP_START_MASK    (((uint64_t)0xFFF) << 48)
#define HV_HYPERCALL_REP_COMP_MASK     (((uint64_t)0xFFF) << 32)
#define HV_HYPERCALL_RESULT_MASK       0xFFFF

#define HV_X64_MAX_HYPERCALL_ELEMENTS ((1 << 12) - 1)

/* Hypercall Identifiers */
enum hv_call_code {
	HvCallGetVpRegisters          = 0x0050,
	HvCallSetVpRegisters          = 0x0051,
	HvCallModifyVtlProtectionMask = 0x000c
};

/* Page Access Flags */
#define HV_PAGE_ACCESS_NONE               0x00
#define HV_PAGE_ACCESS_READ               0x01
#define HV_PAGE_ACCESS_WRITE              0x02
#define HV_PAGE_ACCESS_EXECUTE_KERNEL     0x04
#define HV_PAGE_ACCESS_EXECUTE_USER       0x08
#define HV_PAGE_ACCESS_ALLOW_MMIO         0x8000
#define HV_PAGE_ACCESS_ALL \
	(HV_PAGE_ACCESS_READ | HV_PAGE_ACCESS_WRITE | \
	 HV_PAGE_ACCESS_EXECUTE_KERNEL | HV_PAGE_ACCESS_EXECUTE_USER)

/* vAPIC Constants */
#define HV_VIRTUAL_APIC_NO_EOI_REQUIRED 0x0

/* Internal structure of the hypercall input value (i.e., the control) */
union hv_hypercall_input {
	uint64_t as_uint64;

	/* TLFS 3.7 */
	struct {
		uint32_t call_code         : 16;
		uint32_t is_fast           : 1;
		uint32_t reserved1         : 14;
		uint32_t is_nested         : 1;
		uint32_t count_of_elements : 12;
		uint32_t reserved2         : 4;
		uint32_t rep_start_index   : 12;
		uint32_t reserved3         : 4;
	};
};

/* Internal structure of the hypercall output value (i.e., RAX, upon return) */
union hv_hypercall_output {
	uint64_t as_uint64;

	/* TLFS 3.8 */
	struct {
		uint16_t call_status;
		uint16_t reserved1;
		uint32_t elements_processed : 12;
		uint32_t reserved2          : 20;
	};
};

/* Describes the target VTL for a hypercall */
union hv_input_vtl {
	uint8_t as_u8;

	struct {
		uint8_t target_vtl : 4;
		uint8_t use_target_vtl : 1;
		uint8_t reserved_z : 3;
	};
};

/*
 * Types for the HvGetVpRegisters hypercall
 */

enum hv_register_name {
	HvRegisterVsmCodePageOffsets = 0x000D0002,
	HvRegisterVsmPartitionConfig = 0x000D0007
};

union hv_register_value {
	__uint128_t as_u128;
	uint64_t as_u64;
	uint32_t as_u32;
	uint16_t as_u16;
	uint8_t as_u8;
};

struct hv_register_assoc {
	enum hv_register_name name;
	uint32_t pad;
	union hv_register_value value;
};

struct hv_input_get_vp_registers {
	uint64_t partition_id;
	uint32_t vp_index;
	union hv_input_vtl input_vtl;
	uint8_t reserved8_z;
	uint16_t reserved16_z;
	__aligned(8) enum hv_register_name names[1];
};

struct hv_input_set_vp_registers {
	uint64_t partition_id;
	uint32_t vp_index;
	union hv_input_vtl input_vtl;
	uint8_t reserved8_z;
	uint16_t reserved16_z;
	__aligned(8) struct hv_register_assoc elements[1];
};

/*
 * Types for the the HvModifyVtlProtectionMask hypercall
 */

struct hv_input_modify_vtl_protection_mask {
	uint64_t partition_id;
	uint32_t map_flags;
	union hv_input_vtl target_vtl;
	uint8_t reserved8_z;
	uint16_t reserved16_z;
	__aligned(8) uint64_t gpa_page_list[];
};

#endif /* VSM_HV_CALL_H */
