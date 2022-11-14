/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_HV_ASSIST_H
#define VSM_HV_ASSIST_H

#include <compiler.h>
#include <stdint.h>

#include <sm/paging.h>

/* Format of the APIC assist page */
union hv_virtual_apic_assist {
	uint32_t apic_flags;

	struct {
		uint32_t no_eoi_required : 1;
	};
};

/* This indicates the reason why a VTL was entered */
enum hv_vtl_entry_reason {
	/* This reason is reserved and is not used */
	HvVtlEntryReserved  = 0,

	/* Indicates entry due to a VTL call from a lower VTL */
	HvVtlEntryVtlCall   = 1,

	/* Indicates entry due to an interrupt targeted to the VTL */
	HvVtlEntryInterrupt = 2
};

struct hv_vp_vtl_control {
	/*
	 * The hypervisor updates the entry reason with an indication as to why the
	 * VTL was entered on the virtual processor.
	 */
	enum hv_vtl_entry_reason entry_reason;

	/* This flag determines whether the VINA interrupt line is asserted */
	union {
		uint8_t as_uint8;
	
		struct {
			uint8_t vina_asserted   : 1;
			uint8_t vina_reserved_z : 7;
		};
	} vina_status;

	uint8_t  reserved_z0;
	uint16_t reserved_z1;

	/*
	 * A guest updates the vtl_return* fields to provide the register values to
	 * restore on VTL return.
	 */
	uint64_t vtl_return_x64_rax;
	uint64_t vtl_return_x64_rcx;
};

/*
 * Control structure that allows a hypervisor to indicate to its parent
 * hypervisor which nested enlightenment privileges are to be granted to the
 * current nested guest context.
 */
struct hv_nested_enlightenments_control {
	union {
		uint32_t as_uint32;

		struct {
			uint32_t direct_hypercall          :1;
			uint32_t virtualization_exception : 1;
			uint32_t reserved                 : 30;
		};
	} features;

	union {
		uint32_t as_uint32;

		struct {
			uint32_t inter_partition_communication : 1;
			uint32_t reserved                      : 31;
		};
	} hypercall_controls;
};

/*
 * The virtualization fault information area contains the current fault code and
 * fault parameters for the VP. It must be 16 byte aligned.
 */
 union hv_virtualization_fault_information {
	struct {
		uint16_t parameter0;
		uint16_t reserved0;
		uint32_t code;
		uint64_t parameter1;
	};

	/*
	 * Reserved for future use when we add support for Intel's architecturally-
	 * defined #VE information area.
	 */
	struct {
		uint8_t ve_information_area[34];

		/*
		 * Explicit padding to make the whole structure 40 bytes in size. These
		 * last 6 bytes are not included in save state.
		 */
		uint8_t reserved1[6];
	};
};

/* Internal structure of the VP Assist page */
union hv_vp_assist_page {
	struct {
		/* APIC assist for optimized EOI processing */
		union hv_virtual_apic_assist apic_assist;

		uint32_t reserved_z0;

		/* VP VTL control information */
		struct hv_vp_vtl_control vtl_control;
		struct hv_nested_enlightenments_control
			nested_enlightenments_control;
		uint8_t enlighten_vm_entry;
		uint8_t reserved_z1[7];
		uint64_t current_nested_vmcs;
		uint8_t synthetic_time_unhalted_timer_expired;
		uint8_t reserved_z2[7];

		/* Must be 16 byte aligned */
		union hv_virtualization_fault_information
			virtualization_fault_information;
	};

	uint8_t reserved_z_byte_padding[VSM_PAGE_SIZE];
};

#endif /* VSM_HV_ASSIST_H */
