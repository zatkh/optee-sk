srcs-$(CFG_WITH_USER_TA) += fs_htree.c
ifndef CFG_X64_core
srcs-y += interrupt.c
srcs-y += invoke.c
endif
srcs-$(CFG_LOCKDEP) += lockdep.c
srcs-y += misc.c
cflags-misc.c-y += -fno-builtin
srcs-y += mutex.c
srcs-y += aes_perf.c
srcs-$(CFG_DT_DRIVER_EMBEDDED_TEST) += dt_driver_test.c
