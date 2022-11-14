/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <asm.S>
#include <keep.h>

FUNC sk_flush_tlb , :

	#
	# Save RAX on the stack.
	#

	push    %rax

	#
	# Reload CR3 to flush the TLB.
	#

	mov     %cr3, %rax
	mov     %rax, %cr3

	#
	# Restore RAX.
	#

	pop     %rax

	ret

END_FUNC sk_flush_tlb
KEEP_INIT sk_flush_tlb

FUNC sk_wait_for_debugger , :

	mov     $0,  %r8
1:  test    %r8, %r8
	jz      1b

	ret

END_FUNC sk_wait_for_debugger
KEEP_INIT sk_wait_for_debugger
