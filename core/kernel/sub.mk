srcs-y += abort.c
srcs-$(CFG_CORE_SANITIZE_KADDRESS) += asan.c
cflags-remove-asan.c-y += $(cflags_kasan)
srcs-y += assert.c
srcs-y += console.c
srcs-$(CFG_DT) += dt.c
srcs-$(CFG_DT) += dt_driver.c
srcs-y += pm.c
srcs-y += handle.c
srcs-y += interrupt.c
ifeq ($(CFG_WITH_USER_TA),y)
srcs-y += ldelf_loader.c
srcs-y += ldelf_syscalls.c
srcs-y += scall.c
endif
srcs-$(CFG_LOCKDEP) += lockdep.c
srcs-$(CFG_CORE_DYN_SHM) += msg_param.c
srcs-y += panic.c
srcs-y += trace_ext.c
srcs-y += refcount.c
srcs-y += delay.c
srcs-y += tee_time.c
srcs-$(CFG_SECURE_TIME_SOURCE_REE) += tee_time_ree.c
srcs-y += otp_stubs.c
srcs-y += tee_misc.c
srcs-y += tee_ta_manager.c
srcs-$(CFG_CORE_SANITIZE_UNDEFINED) += ubsan.c
srcs-y += scattered_array.c
srcs-y += huk_subkey.c
srcs-$(CFG_SHOW_CONF_ON_BOOT) += show_conf.c
srcs-y += user_mode_ctx.c
srcs-$(CFG_CORE_TPM_EVENT_LOG) += tpm.c
srcs-y += thread.c
srcs-y += thread_optee_smc.c
srcs-y += mutex.c
srcs-$(CFG_LOCKDEP) += mutex_lockdep.c
srcs-y += pseudo_ta.c
srcs-y += wait_queue.c
srcs-y += notif.c
srcs-y += thread.c

ifeq ($(CFG_WITH_USER_TA),y)
srcs-y += user_ta.c
srcs-$(CFG_REE_FS_TA) += ree_fs_ta.c
endif
srcs-y += tee_time.c
srcs-y += otp_stubs.c
srcs-$(CFG_SECURE_TIME_SOURCE_REE) += tee_time_ree.c
srcs-$(CFG_TEE_CORE_DEBUG) += spin_lock_debug.c
srcs-y += link_dummies.c
srcs-y += trace_ext.c
srcs-y += wait_queue.c
srcs-$(CFG_PM_STUBS) += pm_stubs.c
