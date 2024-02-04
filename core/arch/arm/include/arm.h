/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2015, Linaro Limited
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 */
#ifndef ARM_H
#define ARM_H

#include <util.h>

/* MIDR definitions */
#define MIDR_PRIMARY_PART_NUM_SHIFT	4
#define MIDR_PRIMARY_PART_NUM_WIDTH	12
#define MIDR_PRIMARY_PART_NUM_MASK	(BIT(MIDR_PRIMARY_PART_NUM_WIDTH) - 1)

#define MIDR_IMPLEMENTER_SHIFT		24
#define MIDR_IMPLEMENTER_WIDTH		8
#define MIDR_IMPLEMENTER_MASK		(BIT(MIDR_IMPLEMENTER_WIDTH) - 1)
#define MIDR_IMPLEMENTER_ARM		0x41

#define CORTEX_A7_PART_NUM		0xC07
#define CORTEX_A8_PART_NUM		0xC08
#define CORTEX_A9_PART_NUM		0xC09
#define CORTEX_A15_PART_NUM		0xC0F
#define CORTEX_A17_PART_NUM		0xC0E
#define CORTEX_A57_PART_NUM		0xD07
#define CORTEX_A72_PART_NUM		0xD08
#define CORTEX_A73_PART_NUM		0xD09
#define CORTEX_A75_PART_NUM		0xD0A

/* MPIDR definitions */
#define MPIDR_AFFINITY_BITS	8
#define MPIDR_AFFLVL_MASK	0xff
#define MPIDR_AFF0_SHIFT	0
#define MPIDR_AFF0_MASK		(MPIDR_AFFLVL_MASK << MPIDR_AFF0_SHIFT)
#define MPIDR_AFF1_SHIFT	8
#define MPIDR_AFF1_MASK		(MPIDR_AFFLVL_MASK << MPIDR_AFF1_SHIFT)
#define MPIDR_AFF2_SHIFT	16
#define MPIDR_AFF2_MASK		(MPIDR_AFFLVL_MASK << MPIDR_AFF2_SHIFT)

#define MPIDR_MT_SHIFT		24
#define MPIDR_MT_MASK		BIT(MPIDR_MT_SHIFT)

#define MPIDR_CPU_MASK		MPIDR_AFF0_MASK
#define MPIDR_CLUSTER_SHIFT	MPIDR_AFF1_SHIFT
#define MPIDR_CLUSTER_MASK	MPIDR_AFF1_MASK

/* CLIDR definitions */
#define CLIDR_LOUIS_SHIFT	21
#define CLIDR_LOC_SHIFT		24
#define CLIDR_FIELD_WIDTH	3

/* CSSELR definitions */
#define CSSELR_LEVEL_SHIFT	1

/* CTR definitions */
#define CTR_CWG_SHIFT		24
#define CTR_CWG_MASK		0xf
#define CTR_ERG_SHIFT		20
#define CTR_ERG_MASK		0xf
#define CTR_DMINLINE_SHIFT	16
#define CTR_DMINLINE_WIDTH	4
#define CTR_DMINLINE_MASK	((1 << 4) - 1)
#define CTR_L1IP_SHIFT		14
#define CTR_L1IP_MASK		0x3
#define CTR_IMINLINE_SHIFT	0
#define CTR_IMINLINE_MASK	0xf
#define CTR_WORD_SIZE		4

#define ARM32_CPSR_MODE_MASK	0x1f
#define ARM32_CPSR_MODE_USR	0x10
#define ARM32_CPSR_MODE_FIQ	0x11
#define ARM32_CPSR_MODE_IRQ	0x12
#define ARM32_CPSR_MODE_SVC	0x13
#define ARM32_CPSR_MODE_MON	0x16
#define ARM32_CPSR_MODE_ABT	0x17
#define ARM32_CPSR_MODE_UND	0x1b
#define ARM32_CPSR_MODE_SYS	0x1f

#define ARM32_CPSR_T		(1 << 5)
#define ARM32_CPSR_F_SHIFT	6
#define ARM32_CPSR_F		(1 << 6)
#define ARM32_CPSR_I		(1 << 7)
#define ARM32_CPSR_A		(1 << 8)
#define ARM32_CPSR_E		(1 << 9)
#define ARM32_CPSR_FIA		(ARM32_CPSR_F | ARM32_CPSR_I | ARM32_CPSR_A)
#define ARM32_CPSR_IT_MASK	(ARM32_CPSR_IT_MASK1 | ARM32_CPSR_IT_MASK2)
#define ARM32_CPSR_IT_MASK1	0x06000000
#define ARM32_CPSR_IT_MASK2	0x0000fc00

/* ARM Generic timer definitions */
#define CNTKCTL_PL0PCTEN	BIT(0) /* physical counter el0 access enable */
#define CNTKCTL_PL0VCTEN	BIT(1) /* virtual counter el0 access enable */

#ifdef ARM32
#include <arm32.h>
#endif

#ifdef ARM64
#include <arm64.h>
#endif

#ifndef __ASSEMBLER__
static inline __noprof uint64_t barrier_read_counter_timer(void)
{
	isb();
#ifdef CFG_CORE_SEL2_SPMC
	return read_cntvct();
#else
	return read_cntpct();
#endif
}

static inline bool feat_bti_is_implemented(void)
{
#ifdef ARM32
	return false;
#else
	return ((read_id_aa64pfr1_el1() & ID_AA64PFR1_EL1_BT_MASK) ==
		FEAT_BTI_IMPLEMENTED);
#endif
}

static inline unsigned int feat_mte_implemented(void)
{
#ifdef ARM32
	return 0;
#else
	return (read_id_aa64pfr1_el1() >> ID_AA64PFR1_EL1_MTE_SHIFT) &
	       ID_AA64PFR1_EL1_MTE_MASK;
#endif
}

static inline bool feat_crc32_implemented(void)
{
#ifdef ARM32
	return false;
#else
	return ((read_id_aa64isar0_el1() >> ID_AA64ISAR0_EL1_CRC32_SHIFT) &
		ID_AA64ISAR0_EL1_CRC32_MASK) == FEAT_CRC32_IMPLEMENTED;
#endif
}

static inline bool feat_pauth_is_implemented(void)
{
#ifdef ARM32
	return false;
#else
	uint64_t mask =
		SHIFT_U64(ID_AA64ISAR1_GPI_MASK, ID_AA64ISAR1_GPI_SHIFT) |
		SHIFT_U64(ID_AA64ISAR1_GPA_MASK, ID_AA64ISAR1_GPA_SHIFT) |
		SHIFT_U64(ID_AA64ISAR1_API_MASK, ID_AA64ISAR1_API_SHIFT) |
		SHIFT_U64(ID_AA64ISAR1_APA_MASK, ID_AA64ISAR1_APA_SHIFT);

	/* If any of the fields is not zero, PAuth is implemented by arch */
	return (read_id_aa64isar1_el1() & mask) != 0U;
#endif
}

#endif

#endif /*ARM_H*/
