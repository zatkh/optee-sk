global-incdirs-y += include
srcs-$(CFG_ARM32_$(sm)) += start_a32.S
srcs-$(CFG_ARM64_$(sm)) += start_a64.S
srcs-$(CFG_X64_$(sm)) += start_x64.S
srcs-y += dl.c
srcs-y += main.c
srcs-y += sys.c
srcs-y += ta_elf.c
srcs-y += ta_elf_rel.c
ifneq ($(ARCH),x64)
srcs-$(CFG_UNWIND) += unwind_arm32.c
srcs-$(CFG_UNWIND) += unwind_arm64.c
endif
srcs-$(CFG_FTRACE_SUPPORT) += ftrace.c
