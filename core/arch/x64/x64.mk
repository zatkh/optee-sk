CFG_LTC_OPTEE_THREAD ?= y
CFG_LPAE_ADDR_SPACE_SIZE ?= (1ull << 32)

CFG_MMAP_REGIONS ?= 13
CFG_RESERVED_VASPACE_SIZE ?= (1024 * 1024 * 10)

CFG_KERN_LINKER_FORMAT ?= elf64-x86-64
CFG_KERN_LINKER_ARCH ?= i386:x86-64

CFG_WITH_VFP ?= n

CFG_CORE_RWDATA_NOEXEC ?= y
CFG_CORE_RODATA_NOEXEC ?= n
ifeq ($(CFG_CORE_RODATA_NOEXEC),y)
$(call force,CFG_CORE_RWDATA_NOEXEC,y)
endif

CFG_CORE_LARGE_PHYS_ADDR ?= y
CFG_WITH_LPAE ?= y

CFG_CORE_UNMAP_CORE_AT_EL0 ?= n

CFG_SM_NO_CYCLE_COUNTING ?= y


core-platform-cppflags	+= -I$(arch-dir)/include
core-platform-subdirs += \
	$(addprefix $(arch-dir)/, kernel crypto mm tee) $(platform-dir)

core-platform-subdirs += $(arch-dir)/sm

x64-platform-cppflags += -DX64=1  -D__LP64__=1

platform-cflags-generic ?= -ffunction-sections -fdata-sections -pipe
platform-aflags-generic ?= -g -pipe -D__ASSEMBLER__

ifeq ($(DEBUG),1)
$(call force,CFG_CC_OPTIMIZE_FOR_SIZE,n)
$(call force,CFG_DEBUG_INFO,y)
endif

CFG_CC_OPTIMIZE_FOR_SIZE ?= y
ifeq ($(CFG_CC_OPTIMIZE_FOR_SIZE),y)
platform-cflags-optimization ?= -Os
else
platform-cflags-optimization ?= -O0
endif

CFG_DEBUG_INFO ?= y
ifeq ($(CFG_DEBUG_INFO),y)
platform-cflags-debug-info ?= -g3
platform-aflags-debug-info ?= -g
endif

core-platform-cflags += $(platform-cflags-optimization)
core-platform-cflags += $(platform-cflags-generic)
core-platform-cflags += $(platform-cflags-debug-info)

core-platform-aflags += $(platform-aflags-generic)
core-platform-aflags += $(platform-aflags-debug-info)

ifeq ($(CFG_CORE_ASLR),y)
core-platform-cflags += -fpie
endif

arch-bits-core := 64
core-platform-cppflags += $(x64-platform-cppflags)
core-platform-cflags += $(x64-platform-cflags)
core-platform-cflags += $(x64-platform-cflags-generic)
core-platform-cflags += $(x64-platform-cflags-no-hard-float)
core-platform-aflags += $(x64-platform-aflags)

ifeq (,$(supported-ta-targets))
supported-ta-targets = ta_x64
endif

ta-targets := $(if $(CFG_USER_TA_TARGETS),$(filter $(supported-ta-targets),$(CFG_USER_TA_TARGETS)),$(supported-ta-targets))
unsup-targets := $(filter-out $(ta-targets),$(CFG_USER_TA_TARGETS))
ifneq (,$(unsup-targets))
$(error CFG_USER_TA_TARGETS contains unsupported value(s): $(unsup-targets). Valid values: $(supported-ta-targets))
endif

ifneq ($(filter ta_x64,$(ta-targets)),)
# Variables for ta-target/sm "ta_x64"
CFG_X64_ta_x64 := y
arch-bits-ta_x64 := 64
ta_x64-platform-cppflags += $(x64-platform-cppflags)
ta_x64-platform-cflags += $(x64-platform-cflags)
ta_x64-platform-cflags += $(platform-cflags-optimization)
ta_x64-platform-cflags += $(platform-cflags-debug-info)
ta_x64-platform-cflags += -fpic -fno-stack-protector
ta_x64-platform-cflags += $(x64-platform-cflags-generic)
ta_x64-platform-aflags += $(platform-aflags-generic)
ta_x64-platform-aflags += $(platform-aflags-debug-info)
ta_x64-platform-aflags += $(x64-platform-aflags)

ta-mk-file-export-vars-ta_x64 += CFG_X64_ta_x64
ta-mk-file-export-vars-ta_x64 += ta_x64-platform-cppflags
ta-mk-file-export-vars-ta_x64 += ta_x64-platform-cflags
ta-mk-file-export-vars-ta_x64 += ta_x64-platform-aflags

ta-mk-file-export-add-ta_x64 += CROSS_COMPILE64 ?= $$(CROSS_COMPILE)_nl_
ta-mk-file-export-add-ta_x64 += CROSS_COMPILE_ta_x64 ?= $$(CROSS_COMPILE64)_nl_
ta-mk-file-export-add-ta_x64 += COMPILER ?= gcc_nl_
ta-mk-file-export-add-ta_x64 += COMPILER_ta_x64 ?= $$(COMPILER)_nl_
endif
