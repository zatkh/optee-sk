// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2016-2018 Texas Instruments Incorporated - http://www.ti.com/
 *	Andrew F. Davis <afd@ti.com>
 */

#include <console.h>
#include <drivers/gic.h>
#include <drivers/serial8250_uart.h>
#include <drivers/ti_sci.h>
#include <kernel/boot.h>
#include <kernel/panic.h>
#include <kernel/pm_stubs.h>
#include <mm/core_memprot.h>
#include <mm/tee_pager.h>
#include <platform_config.h>
#include <stdint.h>
#include <tee/entry_fast.h>
#include <tee/entry_std.h>

static struct serial8250_uart_data console_data;

register_phys_mem_pgdir(MEM_AREA_IO_SEC, GICC_BASE, GICC_SIZE);
register_phys_mem_pgdir(MEM_AREA_IO_SEC, GICD_BASE, GICD_SIZE);
register_phys_mem_pgdir(MEM_AREA_IO_NSEC, CONSOLE_UART_BASE,
		  SERIAL8250_UART_REG_SIZE);

void primary_init_intc(void)
{
	gic_init(GICC_BASE, GICD_BASE);
}

void main_secondary_init_intc(void)
{
	gic_cpu_init();
}

static const struct thread_handlers handlers = {
	.cpu_on = cpu_on_handler,
	.cpu_off = pm_do_nothing,
	.cpu_suspend = pm_do_nothing,
	.cpu_resume = pm_do_nothing,
	.system_off = pm_do_nothing,
	.system_reset = pm_do_nothing,
};

const struct thread_handlers *generic_boot_get_handlers(void)
{
	return &handlers;
}

void console_init(void)
{
	serial8250_uart_init(&console_data, CONSOLE_UART_BASE,
			     CONSOLE_UART_CLK_IN_HZ, CONSOLE_BAUDRATE);
	register_serial_console(&console_data.chip);
}

static TEE_Result init_ti_sci(void)
{
	TEE_Result ret = TEE_SUCCESS;

	ret = k3_sec_proxy_init();
	if (ret != TEE_SUCCESS)
		return ret;

	ret = ti_sci_init();
	if (ret)
		return TEE_ERROR_GENERIC;

	return TEE_SUCCESS;
}

service_init(init_ti_sci);

static TEE_Result secure_boot_information(void)
{
	uint32_t keycnt = 0;
	uint32_t keyrev = 0;
	uint32_t swrev = 0;

	if (!ti_sci_get_swrev(&swrev))
		IMSG("Secure Board Configuration Software: Rev %"PRIu32,
		     swrev);

	if (!ti_sci_get_keycnt_keyrev(&keycnt, &keyrev))
		IMSG("Secure Boot Keys: Count %"PRIu32 ", Rev %"PRIu32,
		     keycnt, keyrev);

	return TEE_SUCCESS;
}

service_init_late(secure_boot_information);

TEE_Result tee_otp_get_hw_unique_key(struct tee_hw_unique_key *hwkey)
{
	uint8_t dkek[SA2UL_DKEK_KEY_LEN] = { };
	int ret = 0;

	assert(SA2UL_DKEK_KEY_LEN >= HW_UNIQUE_KEY_LENGTH);

	ret = ti_sci_get_dkek(0, "OP-TEE", "DKEK", dkek);
	if (ret) {
		EMSG("Could not get HUK");
		return TEE_ERROR_SECURITY;
	}

	memcpy(&hwkey->data[0], dkek, sizeof(hwkey->data));
	memzero_explicit(&dkek, sizeof(dkek));

	IMSG("HUK Initialized");

	return TEE_SUCCESS;
}
