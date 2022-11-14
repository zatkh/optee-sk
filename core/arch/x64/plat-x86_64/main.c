#include <kernel/thread.h>
#include <kernel/panic.h>
#include <kernel/pm_stubs.h>
#include <platform_config.h>
#include <stdint.h>
#include <console.h>
#include <drivers/x64_uart.h>

static const struct thread_handlers handlers = {
	.cpu_on = pm_panic,
	.cpu_off = pm_panic,
	.cpu_suspend = pm_panic,
	.cpu_resume = pm_panic,
	.system_off = pm_panic,
	.system_reset = pm_panic,
};

const struct thread_handlers *generic_boot_get_handlers(void)
{
	return &handlers;
}

static struct uart_data console_data __nex_bss;

void console_init(void)
{
	uart_init(&console_data, CONSOLE_UART_BASE2);

	register_serial_console(&console_data.chip);	
	IMSG("TRACE INITIALIZED\n");
}
