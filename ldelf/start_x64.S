/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <asm.S>
#include <elf_common.h>

/*
 * _start() - Entry of ldelf
 *
 * See include/ldelf.h for details on TEE Core interaction.
 *
 * void start(struct ldelf_arg *arg);
 */
FUNC _ldelf_start , :
	/*
	 * First ldelf needs to be relocated. The binary is compiled to
	 * contain only a minimal number of R_X86_64_RELATIVE relocations
	 * in read/write memory, leaving read-only and executeble memory
	 * untouched.
	 */
	lea reloc_begin_rel(%rip), %rdx
	movq reloc_begin_rel(%rip), %rcx
	movq reloc_end_rel(%rip), %r8
	add %rdx, %rcx
	add %rdx, %r8
	cmp %r8, %rcx
	je 2f

	/* Get the load offset */
	lea _ldelf_start(%rip), %rdx

	/* Loop over the relocations (Elf64_Rela) and process all entries */
1:	movq (%rcx), %r9    /* r9 = r_offset */
	add $8, %rcx       
	movq (%rcx), %r10   /* r10 = r_info */
	add $8, %rcx       
	movq (%rcx), %r11   /* r11 = r_addend */
	add $8, %rcx
	andl $0xffffffff, %r10d
	cmp $R_X86_64_RELATIVE, %r10
	jne 3f

	/*
	* Update the pointer at r_offset + load_offset with r_addend +
	* load_offset.
	*/
	add %rdx, %r9
	add %rdx, %r11
	movq %r11, (%r9)

	cmp %r8, %rcx
	jb 1b 

2:	call ldelf
	movq $0, %rdi
	call utee_return
3:	movq $0, %rdi
	call utee_panic
reloc_begin_rel:
	.quad __reloc_begin - reloc_begin_rel
reloc_end_rel:
	.quad __reloc_end - reloc_end_rel
END_FUNC _ldelf_start
