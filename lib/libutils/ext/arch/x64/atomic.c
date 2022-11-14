/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2020, Microsoft Corporation
 */

#include <atomic.h>

uint32_t atomic_inc32(volatile uint32_t *v)
{
	return __atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST);
}

uint32_t atomic_dec32(volatile uint32_t *v)
{
	return __atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST);
}
