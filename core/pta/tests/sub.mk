srcs-$(CFG_WITH_USER_TA) += fs_htree.c
ifndef CFG_X64_core
srcs-y += interrupt.c
srcs-y += invoke.c
endif
srcs-$(CFG_LOCKDEP) += lockdep.c
srcs-y += misc.c
cflags-misc.c-y += -fno-builtin
srcs-y += mutex.c
