/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <asm.S>
#include <keep.h>

.extern hv_hypercall_page

#
# uint64_t hv_hypercall_raw(uint64_t control, void *input_page_pa,
#     void *output_page_pa);
#

FUNC hv_hypercall_raw , :

	#
	# When called from C code, the compiler places the parameters as follows:
	#
	# RDI <- Control
	# RSI <- Input Page Physical Address
	# RDX <- Output Page Physical Address
	#

	#
	# Hyper-V expects:
	#
	# RCX <- Control
	# RDX <- Input Page Physical Address
	# R8  <- Output Page Physical Address
	#
	# See Hyper-V TLFS 3.7, 3.8.
	#

	mov     %rdi, %rcx     # Place control
	mov     %rdx, %r8      # Place output
	mov     %rsi, %rdx     # Place input

	call    hv_hypercall_page

	#
	# RAX contains the hypercall's return value, which becomes this function's
	# return value, too.
	#

	ret

END_FUNC hv_hypercall_raw
KEEP_INIT hv_hypercall_raw
