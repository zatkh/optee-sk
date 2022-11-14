ifeq ($(CFG_GENERIC_BOOT),y)
srcs-$(CFG_X64_core) += generic_entry_x64.S
endif

srcs-y += atomic.c
srcs-y += descriptor.c
srcs-y += fault.c
srcs-y += fpu.c
srcs-y += gdt.S
srcs-y += generic_boot.c
srcs-y += idt.S
srcs-y += misc.c
srcs-y += spinlock.c
srcs-y += thread_64.S
srcs-y += unwind.c
