/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <sm/hv/hyperv.h>
#include <sm/sk.h>

/* Initialize OP-TEE for the bootstrap processor (BSP) */
void sk_init_boot_processor(void)
{
	/* Initialize hypervisor layer */
	hv_init();
}
