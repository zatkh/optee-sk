/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2018, Intel Corporation
 * Copyright (c) 2015-2017, Linaro Limited
 */

#include <asm.S>
#include <kernel/thread_defs.h>

/* void thread_resume(struct thread_ctx_regs *regs) */
FUNC thread_resume , :
    /*
     * This function's single parameter is passed in RDI. It holds the address
     * of the top of the stack to use for the thread that was selected for this
     * entry into secure mode.
     */

    /* Switch to the new thread's stack */
    movq %rdi, %rsp

    /*
     * In thread.c, the init_regs function placed these values on the new
     * thread's stack, in order:
     *
     * The new RFLAGS
     * The new in-thread RIP
     * The new value of A0
     * The new value of A1
     * The new value of A2
     * The new value of A3
     * The new value of A4 (assumed 0)
     * The new value of A5 (assumed 0)
     */

    /* Load the new value of RFLAGS from this thread's stack */
    popfq

    /* Jump to the RIP as set on this thread's stack */
    retq
END_FUNC thread_resume

/* void thread_rpc_resume(vaddr_t sp, uint32_t a0, uint32_t a1, uint32_t a2,
				uint32_t a3) */
FUNC thread_rpc_resume , :
    /*
     * RDI holds the pointer to the stack of the thread we are resuming, so make
     * that our current stack now.
     */
    movq %rdi, %rsp

    /*
     * Just prior to the RPC, these registers were saved on the stack. Restore
     * them to their previous values.
     *
     * N.B.: These pops match the corresponding push sequence in thread_rpc.
     */
    popfq
    popq %rdi
    popq %rbx
    popq %rbp

    /*
     * Before the RPC, RDI held a pointer to the RPC parameters array. Since RDI
     * has been restored, it does so again. RSI, RDX, RCX and R8 hold the last
     * four parameters to this function, corresponding to A0 ... A4,
     * respectively. Update the RPC parameters array with their new values.
     *
     * TODO: Is this strictly necessary?
     */
    mov %esi, (%rdi)
    mov %edx, 4(%rdi)
    mov %ecx, 8(%rdi)
    mov %r8d, 12(%rdi)

    /* Return to where the thread was just prior to the RPC */
    retq
END_FUNC thread_rpc_resume

FUNC thread_std_smc_entry , :
    /*
     * thread_resume has already popped RFLAGS and RIP from the thread stack,
     * which we are now on.
     *
     * Pop A0 through A5 into the corresponding C-parameter registers, according
     * to the System V AMD64 ABI calling convention.
     */
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %r8
    popq %r9

    /* Handle this thread entry */
    call __thread_std_smc_entry

    /* Keep the return value around */
    movq %rax, %r10

    /* Disable interrupts */
    cli

    /* Fetch this core's temporary stack */
    call thread_get_tmp_sp

    /*
     * Compute its top
     *
     * TODO: Move this constant into a #define.
     */
    add  $2048, %rax

    /*
     * Set the top of the temporary stack as our current stack.
     *
     * N.B. This effectively blows away anything that was previously on the
     *      temporary stack!
     */
    movq %rax, %rsp

    /*
     * Push our default RFLAGS
     *
     * TODO: Find a better way to do this.
     */
    push $0x2

    /* Apply the new RFLAGS */
    popfq

    /* Mark the current thread as free for any subsequent entries */
    call thread_state_free

    /* Load the VTL0 communication registers */
    mov %r10d, %edi      # A0
    mov $0,    %esi      # A1
    mov $0,    %edx      # A2
    mov $0,    %ebx      # A3
    mov $0,    %r8d      # A4
    mov $0,    %r9d      # A5
    mov $0,    %r10d     # A6
    mov $0,    %r11d     # A7

    /* Go back to VTL 0 */
    call vsm_return_to_normal_mode

    /*
     * Upon re-entry into VTL 1, we land here, so invoke the dispatch
     * loop.
     */
    call vsm_dispatch_loop

    /* The dispatch loop does not return */
    jmp .
END_FUNC thread_std_smc_entry

/* void thread_rpc(uint32_t rv[THREAD_RPC_NUM_ARGS]) */
FUNC thread_rpc , :
    /*
     * RDI holds a pointer to the RPC parameters array: four 32-bit numbers. The
     * first value in the array is the RPC return code, while the second and
     * third are the two 32-bit parts of the 64-bit physical address that
     * corresponds to the location of the RPC parameters structure. The fourth
     * value is not used.
     *
     * The first element effectively corresponds to A0, and the second and third
     * to A1 and A2, respectively. Pull them out into individual registers, at
     * 4 byte (32-bit) intervals.
     */
    movl  (%rdi), %r8d
    movl 4(%rdi), %r9d
    movl 8(%rdi), %r10d

    /* 
     * According to the System V AMD64 ABI Calling Convention, RBP and RBX must
     * be preserved by the called function, so push these onto the stack. These
     * will be popped upon resumption when the RPC returns from non-secure mode.
     */
    pushq %rbp
    pushq %rbx

    /* 
     * Save the pointer to the RPC parameters array on the stack, too. The
     * value will be updated upon return from non-secure mode as well.
     */
    pushq %rdi

    /* Save our current RFLAGS, also restored on RPC return */
    pushfq

    /*
     * Push our default RFLAGS
     *
     * TODO: Find a better way to do this.
     */
    push $0x2

    /* Apply the new RFLAGS for OP-TEE core code */
    popfq

    /* Set up the parameters for the call to thread_state_suspend */
    movq $THREAD_FLAGS_COPY_ARGS_ON_RETURN, %rdi
    movq %rsp, %rsi

    /* These registers may be clobbered by the next call, so save them */
    pushq %r8
    pushq %r9
    pushq %r10

    /* Disable interrupts */
    cli

    /*
     * Suspend this thread and release it from the current CPU
     *
     * N.B.: The return value of this function, in RAX, is the current thread
     *       ID. A3 must hold this value so that when normal world asks us to
     *       resume from RPC, we know which thread we are supposed to resume.
     */
    call thread_state_suspend

    /* Restore the values of A0, A1 and A2 */
    popq %r10
    popq %r9
    popq %r8

    /* Clear the VTL0 communication registers */
    xorq %rdi, %rdi
    xorq %rsi, %rsi
    xorq %rdx, %rdx
    xorq %rbx, %rbx
    xorq %r11, %r11

    /* Load the VTL0 communication registers */
    mov %r8d,  %edi      # A0 (RPC return code)
    mov %r9d,  %esi      # A1 (32-bit part of 64-bit RPC arg struct PA)
    mov %r10d, %edx      # A2 (32-bit part of 64-bit RPC arg struct PA)
    mov %eax,  %ebx      # A3 (thread ID)
    mov $0,    %r8d      # A4 (unused)
    mov $0,    %r9d      # A5 (unused)
    mov $0,    %r10d     # A6 (unused)
    mov $0,    %r11d     # A7 (unused)

    /* Go back to VTL 0 */
    call vsm_return_to_normal_mode

    /*
     * Upon re-entry into VTL 1, we land here, so invoke the dispatch
     * loop.
     */
    call vsm_dispatch_loop

    /* The dispatch loop does not return */
    jmp .
END_FUNC thread_rpc

/*
 * uint32_t __thread_enter_user_mode(struct tee_usr_args* args,
 *               unsigned long user_sp, unsigned long user_func,
 *               uint32_t *exit_status0, uint32_t *exit_status1)
 *
 */
FUNC __thread_enter_user_mode , :
    pushfq

    pushq %rbp

    pushq %rcx

    pushq %r8

    movq %rdi, %r13

    movq %rsi, %r14

    movq %rdx, %r15

    movq %rsp, %rdi

    movq %r9, %rsi

    cli
    call thread_state_save

    pushq $0x5b   //USER_DATA_64_SELECTOR | USER_RPL
    pushq %r14
    pushq $0x3202 //IOPL_MASK | IF_MASK | EFLAGS_RESERVED
    pushq $0x53   //USER_CODE_64_SELECTOR | USER_RPL
    pushq %r15
    pushq $0x5b   //USER_DATA_64_SELECTOR | USER_RPL
    popq %rax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movq (%r13),   %rdi
    movq 8(%r13),  %rsi
    movq 16(%r13), %rdx
    movq 24(%r13), %rcx

    xorq %rax, %rax
    xorq %rbx, %rbx
    xorq %rbp, %rbp
    xorq %r8, %r8
    xorq %r9, %r9
    xorq %r10, %r10
    xorq %r11, %r11
    xorq %r12, %r12
    xorq %r13, %r13
    xorq %r14, %r14
    xorq %r15, %r15

    iretq
END_FUNC __thread_enter_user_mode
