ifeq ($(CFG_WITH_USER_TA),y)
srcs-$(CFG_EARLY_TA) += early_ta.c
srcs-$(CFG_SECSTOR_TA) += secstor_ta.c
endif

srcs-y += delay.c

srcs-$(CFG_SECURE_TIME_SOURCE_CNTPCT) += tee_time_arm_cntpct.c
srcs-$(CFG_ARM64_core) += timer_a64.c

srcs-$(CFG_ARM32_core) += spin_lock_a32.S
srcs-$(CFG_ARM64_core) += spin_lock_a64.S
srcs-$(CFG_ARM32_core) += tlb_helpers_a32.S
srcs-$(CFG_ARM64_core) += tlb_helpers_a64.S
srcs-$(CFG_ARM64_core) += cache_helpers_a64.S
srcs-$(CFG_ARM32_core) += cache_helpers_a32.S
srcs-$(CFG_PL310) += tz_ssvce_pl310_a32.S
srcs-$(CFG_PL310) += tee_l2cc_mutex.c

srcs-$(CFG_ARM32_core) += thread_a32.S
srcs-$(CFG_ARM64_core) += thread_a64.S
srcs-$(CFG_ARM32_core) += thread_optee_smc_a32.S
srcs-$(CFG_ARM64_core) += thread_optee_smc_a64.S
srcs-$(CFG_WITH_VFP) += vfp.c
ifeq ($(CFG_WITH_VFP),y)
srcs-$(CFG_ARM32_core) += vfp_a32.S
srcs-$(CFG_ARM64_core) += vfp_a64.S
endif
srcs-$(CFG_ARM32_core) += misc_a32.S
srcs-$(CFG_ARM64_core) += misc_a64.S

srcs-$(CFG_GENERIC_BOOT) += generic_boot.c
ifeq ($(CFG_GENERIC_BOOT),y)
srcs-$(CFG_ARM32_core) += generic_entry_a32.S
srcs-$(CFG_ARM64_core) += generic_entry_a64.S
endif

ifeq ($(CFG_UNWIND),y)
srcs-y += unwind_arm32.c
srcs-$(CFG_ARM64_core) += unwind_arm64.c
endif

srcs-$(CFG_VIRTUALIZATION) += virtualization.c

asm-defines-y += asm-defines.c

ifeq ($(CFG_SYSCALL_FTRACE),y)
# We would not like to profile thread.c file as it provide common APIs
# that are needed for ftrace framework to trace syscalls. So profiling
# this file could create an incorrect cyclic behaviour.
cflags-remove-thread.c-y += -pg
cflags-remove-spin_lock_debug.c-$(CFG_TEE_CORE_DEBUG) += -pg
# Tracing abort dump files corrupts the stack trace. So exclude them
# from profiling.
cflags-remove-abort.c-y += -pg
ifeq ($(CFG_UNWIND),y)
cflags-remove-unwind_arm32.c-y += -pg
cflags-remove-unwind_arm64.c-$(CFG_ARM64_core) += -pg
endif
endif
