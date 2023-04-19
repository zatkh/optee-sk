/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_VSM_H
#define VSM_VSM_H

#include <compiler.h>
#include <stdint.h>

/**
 * This is from the code inherrited from Mariner.
 * Keeping this till we determine if we need it or not.
 */
#define VSM_VTL_CALL_FUNC_ID_PROTECT_OPTEE_PAGES  0x1FF

#ifdef CFG_LVBS_KERNEL_HVCI
/**
 * Recommended parameter
 * a0 : Call id i.e. VSM_VTL_CALL_FUNC_ID_PROTECT_MEMORY
 * a1 : GPA
 * a2 : size
 * a3 : Protection
 * a4 : whether it is a hard protection? Do we need this ? This would mean once a protection is set for the range of GPA it cannot be changed for this boot. might be useful for some kernel sections like SE linux parameter.
 * a5: Empty
 * a6: Right now return value back to VTL0. Can change.
 * a7: Right now vtl return passes the entry count back to vtl0. Again only for debug purpose till console is enabled.
 */
#define VSM_VTL_CALL_FUNC_ID_PROTECT_MEMORY	0x1FFF

/**
 * ToDo: define the call parameters
 */
#define VSM_VTL_CALL_FUNC_ID_LOCK_CR		0x1FFFF

#endif

struct vsm_vtl_params {
	uint32_t a0;
	uint32_t a1;
	uint32_t a2;
	uint32_t a3;
	uint32_t a4;
	uint32_t a5;
	uint32_t a6;
	uint32_t a7;
} __packed;

void vsm_dispatch_entry(struct vsm_vtl_params *params);

TEE_Result vsm_protect_optee_pages(void);

#ifdef CFG_LVBS_KERNEL_HVCI
TEE_Result vsm_restrict_memory(uint64_t start, size_t size, uint32_t permissions);
#endif

#endif /* VSM_VSM_H */
