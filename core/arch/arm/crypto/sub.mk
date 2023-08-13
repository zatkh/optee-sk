ifeq ($(CFG_CRYPTO_WITH_CE),y)
srcs-$(CFG_ARM64_core) += ghash-ce-core_a64.S
srcs-$(CFG_ARM32_core) += ghash-ce-core_a32.S
endif

ifeq ($(CFG_CRYPTO_SHA512_ARM_CE),y)
srcs-y += sha512_armv8a_ce.c
srcs-$(CFG_ARM64_core) += sha512_armv8a_ce_a64.S
endif

ifeq ($(CFG_CRYPTO_SHA3_ARM_CE),y)
srcs-y += sha3_armv8a_ce.c
srcs-$(CFG_ARM64_core) += sha3_armv8a_ce_a64.S
endif

ifeq ($(CFG_CRYPTO_SM3_ARM_CE),y)
srcs-y += sm3_armv8a_ce.c
srcs-$(CFG_ARM64_core) += sm3_armv8a_ce_a64.S
endif

ifeq ($(CFG_CRYPTO_SM4_ARM_CE),y)
srcs-$(CFG_ARM64_core) += sm4_armv8a_ce.c
srcs-$(CFG_ARM64_core) += sm4_armv8a_ce_a64.S
else ifeq ($(CFG_CRYPTO_SM4_ARM_AESE),y)
srcs-$(CFG_ARM64_core) += sm4_armv8a_neon.c
srcs-$(CFG_ARM64_core) += sm4_armv8a_aese_a64.S
endif
