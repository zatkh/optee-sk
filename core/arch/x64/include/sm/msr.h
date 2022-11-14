/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#ifndef VSM_MSR_H
#define VSM_MSR_H

#include <stdint.h>

static inline void sk_write_msr(uint64_t msr, uint64_t value)
{
	uint32_t low = value & 0xFFFFFFFF;
	uint32_t high = value >> 32;

	asm volatile (
		"wrmsr"
		:
		: "c"(msr), "a"(low), "d"(high)
	);
}

static inline uint64_t sk_read_msr(uint64_t msr)
{
	uint32_t low, high;

	asm volatile (
		"rdmsr"
		: "=a"(low), "=d"(high)
		: "c"(msr)
	);

	return ((uint64_t)high << 32) | low;
}

#endif /* VSM_MSR_H */
