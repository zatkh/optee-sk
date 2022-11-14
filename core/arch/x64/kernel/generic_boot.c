// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2015-2018, Linaro Limited
 * Copyright (c) 2018, Intel Corporation
 */

#include <assert.h>
#include <compiler.h>
#include <console.h>
#include <crypto/crypto.h>
#include <inttypes.h>
#include <keep.h>
#include <kernel/asan.h>
#include <kernel/fpu.h>
#include <kernel/generic_boot.h>
#include <kernel/linker.h>
#include <kernel/misc.h>
#include <kernel/panic.h>
#include <kernel/tee_misc.h>
#include <kernel/thread.h>
#include <malloc.h>
#include <mm/core_memprot.h>
#include <mm/core_mmu.h>
#include <mm/tee_mm.h>
#include <mm/tee_mmu.h>
#include <sm/tee_mon.h>
#include <stdio.h>
#include <trace.h>
#include <utee_defines.h>
#include <util.h>

#include <platform_config.h>

static void init_runtime(void)
{
	/*
	 * By default whole OP-TEE uses malloc, so we need to initialize
	 * it early. But, when virtualization is enabled, malloc is used
	 * only by TEE runtime, so malloc should be initialized later, for
	 * every virtual partition separately. Core code uses nex_malloc
	 * instead.
	 */
#ifdef CFG_VIRTUALIZATION
	nex_malloc_add_pool(__nex_heap_start, __nex_heap_end -
						  __nex_heap_start);
#else
	malloc_add_pool(__heap1_start, __heap1_end - __heap1_start);
#endif

	IMSG_RAW("\n");
}

/* What this function is using is needed each time another CPU is started */
KEEP_PAGER(generic_boot_get_handlers);

void init_tee_runtime(void)
{
#ifdef CFG_VIRTUALIZATION
	/* We need to initialize pool for every virtual guest partition */
	malloc_add_pool(__heap1_start, __heap1_end - __heap1_start);
#endif

#ifndef CFG_WITH_PAGER
	/* Pager initializes TA RAM early */
	teecore_init_ta_ram();
#endif
	if (init_teecore() != TEE_SUCCESS)
		panic();
}

void generic_boot_init_primary(void)
{
	IMSG("Welcome to OP-TEE\n");

	fpu_init();
	init_runtime();

#ifndef CFG_VIRTUALIZATION
	thread_init_boot_thread();
#endif
	thread_init_primary(generic_boot_get_handlers());
	thread_init_per_cpu();

	IMSG("OP-TEE version: %s", core_v_str);

#ifndef CFG_VIRTUALIZATION
	init_tee_runtime();
#endif
	DMSG("Primary CPU switching to normal world boot\n");
}
