PLATFORM_FLAVOR ?= zcu102

include core/arch/arm/cpu/cortex-armv8-0.mk

$(call force,CFG_TEE_CORE_NB_CORE,4)
$(call force,CFG_CDNS_UART,y)
$(call force,CFG_GENERIC_BOOT,y)
$(call force,CFG_GIC,y)
$(call force,CFG_PM_STUBS,y)
$(call force,CFG_SECURE_TIME_SOURCE_CNTPCT,y)
$(call force,CFG_WITH_ARM_TRUSTED_FW,y)

ifeq ($(CFG_ARM64_core),y)
$(call force,CFG_WITH_LPAE,y)
else
$(call force,CFG_ARM32_core,y)
endif

ifneq (,$(filter $(PLATFORM_FLAVOR),zcu102 zcu104 zcu106 zc1751_dc1 zc1751_dc2))

CFG_UART_BASE ?= UART0_BASE
CFG_UART_IT ?= IT_UART0
CFG_UART_CLK_HZ ?= UART0_CLK_IN_HZ

# ZCU102 features 4 GiB of DDR
ifeq ($(CFG_ARM64_core),y)
CFG_DDR_SIZE ?= 0x100000000
else
# On 32 bit build limit to 2 GiB of RAM
CFG_DDR_SIZE ?= 0x80000000
endif
endif

ifneq (,$(filter $(PLATFORM_FLAVOR),ultra96))

CFG_UART_BASE ?= UART1_BASE
CFG_UART_IT ?= IT_UART1
CFG_UART_CLK_HZ ?= UART1_CLK_IN_HZ

# Ultra96 features 2 GiB of DDR
CFG_DDR_SIZE ?= 0x80000000
endif

# By default use DT address as specified by Xilinx
CFG_DT_ADDR ?= 0x100000

CFG_TZDRAM_START ?= 0x60000000
CFG_TZDRAM_SIZE  ?= 0x10000000
CFG_SHMEM_START  ?= 0x70000000
CFG_SHMEM_SIZE   ?= 0x10000000

CFG_WITH_STATS ?= y
CFG_CRYPTO_WITH_CE ?= y
