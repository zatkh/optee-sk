/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <kernel/spinlock.h>

unsigned int __cpu_spin_trylock(unsigned int *lock)
{
	(void)lock;

	return 0;
}

void __cpu_spin_lock(unsigned int *lock)
{
	(void)lock;
}

void __cpu_spin_unlock(unsigned int *lock)
{
	(void)lock;
}
