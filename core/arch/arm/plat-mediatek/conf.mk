PLATFORM_FLAVOR ?= mt8173

include core/arch/arm/cpu/cortex-armv8-0.mk

$(call force,CFG_TEE_CORE_NB_CORE,4)
CFG_TZDRAM_START ?= 0xbe000000
CFG_TZDRAM_SIZE ?= 0x01e00000
CFG_SHMEM_START ?= 0xbfe00000
CFG_SHMEM_SIZE ?= 0x00200000

$(call force,CFG_8250_UART,y)
$(call force,CFG_GENERIC_BOOT,y)
$(call force,CFG_PM_STUBS,y)
$(call force,CFG_SECURE_TIME_SOURCE_CNTPCT,y)
$(call force,CFG_WITH_ARM_TRUSTED_FW,y)

ifeq ($(CFG_ARM64_core),y)
$(call force,CFG_WITH_LPAE,y)
else
$(call force,CFG_ARM32_core,y)
endif

CFG_WITH_STACK_CANARIES ?= y

ifeq ($(PLATFORM_FLAVOR),mt8173)
# 2**1 = 2 cores per cluster
$(call force,CFG_CORE_CLUSTER_SHIFT,1)
endif

ifeq ($(PLATFORM_FLAVOR),mt8195)
$(call force,CFG_TEE_CORE_NB_CORE,8)
$(call force,CFG_CORE_CLUSTER_SHIFT,2)
$(call force,CFG_ARM_GICV3,y)
$(call force,CFG_GIC,y)
$(call force,CFG_CORE_ARM64_PA_BITS,36)
CFG_TZDRAM_START ?= 0x43200000
CFG_TZDRAM_SIZE ?=  0x00a00000
CFG_SHMEM_START ?= ($(CFG_TZDRAM_START) + $(CFG_TZDRAM_SIZE))
CFG_SHMEM_SIZE ?= 0x00200000
endif
