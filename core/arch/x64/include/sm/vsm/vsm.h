/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_VSM_H
#define VSM_VSM_H

#include <compiler.h>
#include <stdint.h>

#define VSM_VTL_CALL_FUNC_ID_PROTECT_OPTEE_PAGES  0x1FF

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

#endif /* VSM_VSM_H */
