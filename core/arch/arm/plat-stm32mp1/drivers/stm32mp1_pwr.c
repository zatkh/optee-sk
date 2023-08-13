// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2018-2019, STMicroelectronics
 */

#include <mm/core_memprot.h>
#include <platform_config.h>
#include <drivers/stm32mp1_pwr.h>

vaddr_t stm32_pwr_base(void)
{
	static struct io_pa_va base = { .pa = PWR_BASE };

	return io_pa_or_va_secure(&base, 1);
}

unsigned int stm32mp1_pwr_regulator_mv(enum pwr_regulator id)
{
	assert(id < PWR_REGU_COUNT);

	return pwr_regulators[id].level_mv;
}

void stm32mp1_pwr_regulator_set_state(enum pwr_regulator id, bool enable)
{
	uintptr_t cr3 = stm32_pwr_base() + PWR_CR3_OFF;
	uint32_t enable_mask = pwr_regulators[id].cr3_enable_mask;

	assert(id < PWR_REGU_COUNT);

	if (enable) {
		uint32_t ready_mask = pwr_regulators[id].cr3_ready_mask;
		uint64_t to = 0;

		io_setbits32(cr3, enable_mask);

		to = timeout_init_us(10 * 1000);
		while (!timeout_elapsed(to))
			if (io_read32(cr3) & ready_mask)
				break;

		if (!(io_read32(cr3) & ready_mask))
			panic();
	} else {
		io_clrbits32(cr3, enable_mask);
	}
}

bool stm32mp1_pwr_regulator_is_enabled(enum pwr_regulator id)
{
	assert(id < PWR_REGU_COUNT);

	return io_read32(stm32_pwr_base() + PWR_CR3_OFF) &
	       pwr_regulators[id].cr3_enable_mask;
}
