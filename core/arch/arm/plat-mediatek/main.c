// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2015, Linaro Limited
 */

#include <console.h>
#include <drivers/serial8250_uart.h>
#include <kernel/generic_boot.h>
#include <kernel/panic.h>
#include <kernel/pm_stubs.h>
#include <mm/core_memprot.h>
#include <platform_config.h>
#include <stdint.h>
#include <tee/entry_std.h>
#include <tee/entry_fast.h>

register_phys_mem_pgdir(MEM_AREA_IO_NSEC,
			CONSOLE_UART_BASE, SERIAL8250_UART_REG_SIZE);

static const struct thread_handlers handlers = {
	.cpu_on = cpu_on_handler,
	.cpu_off = pm_do_nothing,
	.cpu_suspend = pm_do_nothing,
	.cpu_resume = pm_do_nothing,
	.system_off = pm_do_nothing,
	.system_reset = pm_do_nothing,
};

static struct serial8250_uart_data console_data;

register_ddr(CFG_DRAM_BASE, CFG_DRAM_SIZE);

#ifdef CFG_GIC
register_phys_mem_pgdir(MEM_AREA_IO_SEC, GIC_BASE + GICD_OFFSET,
			CORE_MMU_PGDIR_SIZE);
register_phys_mem_pgdir(MEM_AREA_IO_SEC, GIC_BASE + GICC_OFFSET,
			CORE_MMU_PGDIR_SIZE);

void primary_init_intc(void)
{
	gic_init(GIC_BASE + GICC_OFFSET, GIC_BASE + GICD_OFFSET);
}
#endif

void console_init(void)
{
	serial8250_uart_init(&console_data, CONSOLE_UART_BASE,
			     CONSOLE_UART_CLK_IN_HZ, CONSOLE_BAUDRATE);
	register_serial_console(&console_data.chip);
}
