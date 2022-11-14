/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <mm/generic_ram_layout.h>

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

//Fixed IO addresses for com1 and com 2 for x64
#define CONSOLE_UART_BASE1	0x3f8
#define CONSOLE_UART_BASE2	0x2f8

#ifdef CFG_TEE_LOAD_ADDR
#define TEE_LOAD_ADDR			CFG_TEE_LOAD_ADDR
#else
#define TEE_LOAD_ADDR			TEE_RAM_START
#endif

/* FROM Intel: TA user mode virtual address */
// ToDo: How to prevent overlapping to other virtual addresses (self check)
#define TA_USER_BASE_VA 0xF0000000

#endif /* PLATFORM_CONFIG_H */
