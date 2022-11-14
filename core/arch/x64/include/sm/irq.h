/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_IRQ_H
#define VSM_IRQ_H

#include <stdint.h>

static inline void sk_irq_disable(void)
{
	asm volatile("cli": : :"memory");
}

static inline void sk_irq_enable(void)
{
	asm volatile("sti": : :"memory");
}

static inline uint64_t sk_irq_save(void)
{
	uint64_t flags;

	asm volatile(
		"pushf ; pop %0"
		: "=rm" (flags)
		:
		: "memory");

	sk_irq_disable();

	return flags;
}

static inline void sk_irq_restore(uint64_t flags)
{
	asm volatile(
		"push %0 ; popf"
		:
		: "g" (flags)
		: "memory", "cc");
}

#endif /* VSM_IRQ_H */
