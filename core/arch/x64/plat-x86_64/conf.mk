$(call force,CFG_GENERIC_BOOT,y)
$(call force,CFG_APIC,n)
$(call force,CFG_X64_UART,y)
$(call force,CFG_PM_STUBS,y)
$(call force,CFG_SECURE_TIME_SOURCE_REE,y)

$(call force,CFG_WITH_LPAE,y)

CFG_WITH_STACK_CANARIES ?= n
# CFG_WITH_STATS ?= y

# TODO: thread.c needs a value for nuber of cores. 
# In arm implementation each plat-conf sets a fixed value. Need to edit this implementation for x64
CFG_TEE_CORE_NB_CORE = 1
# TODO: will fix multi threads issue for x86
CFG_NUM_THREADS ?= 1

# TODO: will investigate this if this can be enabled for x86
CFG_TA_DYNLINK ?= n

# TODO: not support mbedtls yet for x86, will enable it in the future.
CFG_TA_MBEDTLS_SELF_TEST ?= n

# TODO: enable of for x64
CFG_CORE_ASLR = n

CFG_WITH_PAGER ?= n
CFG_ARM_GICV3 ?= n
CFG_WITH_USER_TA ?= y

CFG_DRAM_START ?= 0x40000000
CFG_DRAM_SIZE  ?= 0x600000
CFG_SHMEM_START  ?= ($(CFG_DRAM_START) + $(CFG_DRAM_SIZE))
CFG_SHMEM_SIZE   ?= 0x200000
CFG_TEE_RAM_VA_SIZE ?= 0x200000

supported-ta-targets = ta_x64

$(call force,CFG_X64_core,y)
$(call force,CFG_BOOT_SECONDARY_REQUEST,n)
$(call force,CFG_DT,n)
