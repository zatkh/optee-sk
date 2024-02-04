// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2017, Fuzhou Rockchip Electronics Co., Ltd.
 * Copyright (C) 2019, Theobroma Systems Design und Consulting GmbH
 */

#include <console.h>
#include <drivers/gic.h>
#include <drivers/serial8250_uart.h>
#include <io.h>
#include <kernel/generic_boot.h>
#include <kernel/panic.h>
#include <kernel/pm_stubs.h>
#include <mm/core_memprot.h>
#include <platform_config.h>
#include <stdint.h>

#if defined(CFG_EARLY_CONSOLE)
static struct serial8250_uart_data early_console_data;
register_phys_mem_pgdir(MEM_AREA_IO_NSEC,
			CFG_EARLY_CONSOLE_BASE, CFG_EARLY_CONSOLE_SIZE);
#endif

register_phys_mem_pgdir(MEM_AREA_IO_SEC, GIC_BASE, GIC_SIZE);

static const struct thread_handlers handlers = {
#if defined(CFG_WITH_ARM_TRUSTED_FW)
	.cpu_on = cpu_on_handler,
	.cpu_off = pm_do_nothing,
	.cpu_suspend = pm_do_nothing,
	.cpu_resume = pm_do_nothing,
	.system_off = pm_do_nothing,
	.system_reset = pm_do_nothing,
#else
	.cpu_on = pm_do_nothing,
	.cpu_off = pm_do_nothing,
	.cpu_suspend = pm_do_nothing,
	.cpu_resume = pm_do_nothing,
	.system_off = pm_do_nothing,
	.system_reset = pm_do_nothing,
#endif
};

void primary_init_intc(void)
{
	gic_init(GICC_BASE, GICD_BASE);
}

void main_secondary_init_intc(void)
{
	gic_cpu_init();
}

void console_init(void)
{
#if defined(CFG_EARLY_CONSOLE)
	/*
	 * Console devices can vary a lot between devices and
	 * OP-TEE will switch to the DT-based real console later,
	 * based on DT-devices and the systems chosen node.
	 * So early console is only needed for early debugging.
	 */
	serial8250_uart_init(&early_console_data,
			     CFG_EARLY_CONSOLE_BASE,
			     CFG_EARLY_CONSOLE_CLK_IN_HZ,
			     CFG_EARLY_CONSOLE_BAUDRATE);
	register_serial_console(&early_console_data.chip);
#endif
}
