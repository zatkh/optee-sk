global-incdirs-y += include

srcs-y += snprintk.c
srcs-y += strlcat.c
srcs-y += strlcpy.c
srcs-y += trace.c
srcs-y += mempool.c
srcs-y += nex_strdup.c
srcs-y += consttime_memcmp.c
srcs-y += memzero_explicit.c

subdirs-$(arch_arm) += arch/$(ARCH)
subdirs-$(arch_x64) += arch/$(ARCH)
subdirs-y += ftrace
