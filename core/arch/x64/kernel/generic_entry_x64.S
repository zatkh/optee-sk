/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2018, Intel Corporation
 */

#include <platform_config.h>
#include <descriptor.h>
#include <asm.S>
#include <keep.h>
#include <mm/core_mmu.h>

.extern sk_init_boot_processor

.extern vsm_return_to_normal_mode
.extern vsm_dispatch_loop

/*
 * This function is only valid for the boot processor (i.e., the first processor
 * to enter VTL1).
 */

FUNC _start , :
	/* Load the GDT  */
	lgdt _gdtr

	/* Load the data selectors */
	movw $DATA_SELECTOR, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	/*
	 * Load the temporary stack.
	 *
	 * N.B.: The C global variable stack_tmp_export holds the address of the top
	 *       of the stack. The instruction below reads the value of that
	 *       variable and makes that the current stack pointer.
	 */
	mov stack_tmp_export, %rsp

	/* Jump to the 64 bit CS */
	push $CODE_64_SELECTOR
	push $.initialize
	ret

.initialize:
	/* Clear BSS */
bss_setup:
	movl $__bss_start, %eax /* Start address of the BSS */
	movl $__bss_end, %ecx   /* Find length of BSS in bytes */
	subl %eax, %ecx
	/* Convert to 32-bit word, since the BSS is aligned anyway */
	shrl $2, %ecx
.Lzero_bss_sec:
	movl $0, (%eax)
	addl $4, %eax
	loop .Lzero_bss_sec

#ifdef CFG_VIRTUALIZATION
clear_nex_bss:
	movl $__nex_bss_start, %eax
	movl $__nex_bss_end, %ecx
	subl %eax, %ecx
	/* Convert to 32-bit word, since the BSS is aligned anyway */
	shrl $2, %ecx
.Lzero_nex_bss_sec:
	movl $0, (%eax)
	addl $4, %eax
	loop .Lzero_nex_bss_sec
#endif

paging_setup:
	/* Set up the IDT */
	call setup_idt

	/* Enable Console */
	call console_init

	/* Create MMU tables from OP-TEE memory structures */
	call core_mmu_init

	/* Init memory map structures related to OP-TEE operations */
	call core_init_mmu_map

	/* Call the init module */
	call generic_boot_init_primary

	/*
	* Clear current thread id now to allow the thread to be reused on
	* next entry. Matches the thread_init_boot_thread() in
	* generic_boot.c.
	*/
	call thread_clr_boot_thread

	/* Initialize OP-TEE on this processor */
	call sk_init_boot_processor

	/*
	 * Clear shared state registers.
	 *
	 *TODO: Clear the full list, according to the Hyper-V TLFS.
	 */

	mov $0, %edi
	mov $0, %esi
	mov $0, %edx
	mov $0, %ebx
	mov $0, %r8d
	mov $0, %r9d
	mov $0, %r10d
	mov $0, %r11d

	/* Return to VTL 0. */
	call vsm_return_to_normal_mode

	/* Enter the dispatch loop. */
	call vsm_dispatch_loop

	/* We should never reach here. */
	jmp .

END_FUNC _start
KEEP_INIT _start
