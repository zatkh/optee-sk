/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <asm.S>
#include <keep.h>

.extern vsm_process_vtl_call

FUNC vsm_return_to_normal_mode , :

	# RAX: Holds the VTL Switch Input (Control)
	# RCX: Holds the VM Function ID
	#
	# The Control value must be zero.
	# The Function ID for VTLRETURN is 0x12.

	addl $1, entry_count(%rip)	#increment the entry counter
	movl entry_count(%rip), %r11d	#Pass it back to VTL0 in r11
	mov     $0x0,  %rax
	mov     $0x12, %rcx

	#
	# VTL Return
	#

	vmcall

	ret

END_FUNC vsm_return_to_normal_mode
KEEP_INIT vsm_return_to_normal_mode

FUNC vsm_dispatch_loop , :

DispatchLoop:
	#
	# On entry into VTL 1, the state of the registers is as follows:
	#
	#  EDI: A0
	#  ESI: A1
	#  EDX: A2
	#  EBX: A3
	#  R8D: A4
	#  R9D: A5
	# R10D: A6
	# R11D: A7
	#

	#
	# Keep a copy of RBP to restore it later, then copy RSP into RBP for easy
	# computation of register-relative addresses below.
	#

	push    %rbp
	mov     %rsp, %rbp

	#
	# Reserve 32 (0x20) bytes on the stack, which is enough to store the eight
	# 32-bit values used to communicate with VTL 0.
	#

	sub     $0x20, %rsp

	#
	# Copy each of the 32-bit values from the registers onto the stack at 8-byte
	# intervals. When done, the layout of the stack matches the layout of the
	# vsm_vtl_params structure.
	#

	mov     %edi,  -0x20(%rbp)     # A0
	mov     %esi,  -0x1c(%rbp)     # A1
	mov     %edx,  -0x18(%rbp)     # A2
	mov     %ebx,  -0x14(%rbp)     # A3
	mov     %r8d,  -0x10(%rbp)     # A4
	mov     %r9d,  -0x0c(%rbp)     # A5
	mov     %r10d, -0x08(%rbp)     # A6
	mov     %r11d, -0x04(%rbp)     # A7

	#
	# Compute the address of the first element pushed onto the stack and place
	# it in RDI, which holds the value of the first and only parameter to
	# vsm_dispatch_entry, according to the System V AMD64 ABI calling
	# convention.
	#

	lea     -0x20(%rbp), %rdi

	#
	# Dispatch the VTL entry.
	#

	call    vsm_dispatch_entry

	#
	# Since the dispatch routine was called with a pointer to the first element
	# on the stack, the former is free to modify the values. Now, copy the
	# possibly updated values back into the registers used to communicate with
	# VTL 0.
	#

	mov     -0x20(%rbp), %edi      # A0
	mov     -0x1c(%rbp), %esi      # A1
	mov     -0x18(%rbp), %edx      # A2
	mov     -0x14(%rbp), %ebx      # A3
	mov     -0x10(%rbp), %r8d      # A4
	mov     -0x0c(%rbp), %r9d      # A5
	mov     -0x08(%rbp), %r10d     # A6
	mov     -0x04(%rbp), %r11d     # A7

	#
	# Restore RSP and RBP.
	#

	mov     %rbp, %rsp
	pop     %rbp

	# Return to VTL 0.
	call    vsm_return_to_normal_mode

	# Subsequent VTLCALLs land here, so restart the dispatch loop.
	jmp     DispatchLoop

END_FUNC vsm_dispatch_loop
KEEP_INIT vsm_dispatch_loop


FUNC vsm_return_to_normal_mode_from_init , :

	leaq active_vtl(%rip), %rdi
	leaq enable_vtl_prot(%rip), %rsi
	leaq default_prot_mask(%rip), %rdx

	call hv_get_vsm_partition_info

	movzbl active_vtl(%rip), %edi		#A0
	movzbl enable_vtl_prot(%rip), %esi	#A1
	movzbl default_prot_mask(%rip), %edx	#A2

	call vsm_return_to_normal_mode

	call vsm_dispatch_loop

END_FUNC vsm_return_to_normal_mode_from_init
KEEP_INIT vsm_return_to_normal_mode_from_init

.data
.align 8

	active_vtl:
		.byte 0xff

	enable_vtl_prot:
		.byte 0xff

	default_prot_mask:
		.byte 0xff

	.globl entry_count
	entry_count:
		.long 0
