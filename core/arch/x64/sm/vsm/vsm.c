/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2019, Microsoft Corporation
 */

#include <platform_config.h>

#include <kernel/thread_private.h>
#include <sm/hv/hyperv.h>
#include <sm/optee_smc.h>
#include <sm/vsm/vsm.h>
#include <trace.h>

static void vsm_dispatch_call(struct vsm_vtl_params *params)
{
	uint32_t ret;
	TEE_Result res;

	uint32_t smc_func_id;
	struct thread_smc_args args;

	DMSG("Dispatch call id 0x%X", params->a0);

#ifdef CFG_LVBS_KERNEL_HVCI
	/**
	 *  For now use params->a7 to pass back status to VTL0 since console
	 *  support is not yet enabled
	 */
	switch(params->a0) {
		case VSM_VTL_CALL_FUNC_ID_PROTECT_MEMORY:
			/* Temporary to ensure we have landed here */
			vsm_restrict_memory(params->a1, params->a2, params->a3);
			params->a6 = VSM_VTL_CALL_FUNC_ID_PROTECT_MEMORY;
			break;
		case VSM_VTL_CALL_FUNC_ID_LOCK_CR:
			/* Temporary to ensure we have landed here */
			params->a6 = VSM_VTL_CALL_FUNC_ID_LOCK_CR;
			break;
		default:
			params->a6 = TEE_ERROR_BAD_PARAMETERS;
			break;
	}
	return;
#endif
	/**
	 * Code inherited from Mariner code base.
	 * Keeping it till we understand the full scope of LVBS and whethe
	 * we need to carry any of this forward
	 */
	if (params->a0 == VSM_VTL_CALL_FUNC_ID_PROTECT_OPTEE_PAGES) {
		res = vsm_protect_optee_pages();
		params->a7 = res;
	} else {
		DMSG("Call is a generic SMC");

		/*
		 * A0 contains the SMC Function ID. Note that on x86 arch there is no SMC instruction.
		 * 
		 * See ARM DEN 0028B, table 2-1 for the meaning of its bits.
		 */
		smc_func_id = params->a0;

		/*
		 * The ARM SMCCC allows for a wholly 32-bit interface or for a wholly
		 * 64-bit interface. The first allows one to pass up to six 32-bit
		 * numbers, while the latter allows up to six 64-bit numbers.
		 * 
		 * OP-TEE and the Linux OP-TEE driver thus far only use the 32-bit
		 * interface. Hence, assert that the call just received is 32-bit.
		 * 
		 * See ARM DEN 0028B, 2.4.
		 */
		if (OPTEE_SMC_IS_64(smc_func_id)) {
			EMSG("The call was 64-bit, not supported");
			params->a0 = OPTEE_SMC_RETURN_ENOTAVAIL;
			return;
		}

		/*
		 * TODO: Rewrite entry/exit code to deal with 64-bit values.
		 * 
		 * This is awkward: the Linux OP-TEE driver uses 32-bit numbers even on
		 * ARM64. Yet, OP-TEE defines the thread_smc_args structure as holding
		 * 64-bit numbers on ARM64.
		 * 
		 * This discrepancy can be worked around by pushing and popping the
		 * 32-bit numbers as 64-bit numbers onto and off the stack (i.e., using
		 * 64-bit instructions on 32-bit operands). (Would that even compile?)
		 */
		args.a0 = params->a0;
		args.a1 = params->a1;
		args.a2 = params->a2;
		args.a3 = params->a3;
		args.a4 = params->a4;
		args.a5 = params->a5;
		args.a6 = params->a6;
		args.a7 = params->a7;

		/*
		 * Dispatch the function call depending on its type
		 * 
		 * N.B.: The fast call handler always returns, the standard call handler
		 *       only returns on error.
		 */
		if (OPTEE_SMC_IS_FAST_CALL(smc_func_id)) {
			DMSG("Got a FAST call");

			/* This call puts its return value into a0 on error. */
			thread_handle_fast_smc(&args);
		} else {
			DMSG("Got an STD call");

			ret = thread_handle_std_smc(params->a0, params->a1, params->a2,
				params->a3, params->a4, params->a5, params->a6, params->a7);
			args.a0 = ret;
		}

		/* TODO: See above, these copies should be removed. */
		params->a0 = args.a0;
		params->a1 = args.a1;
		params->a2 = args.a2;
		params->a3 = args.a3;
		params->a4 = args.a4;
		params->a5 = args.a5;
		params->a6 = args.a6;
		params->a7 = args.a7;
	}
}

void vsm_dispatch_entry(struct vsm_vtl_params *params)
{
	union hv_vp_assist_page *vp_assist_info = hv_get_vp_assist_page();

	switch (vp_assist_info->vtl_control.entry_reason)
	{
	case HvVtlEntryVtlCall:
		vsm_dispatch_call(params);
		break;

	case HvVtlEntryInterrupt:
		/* Test outputs */
		params->a0 = 20;
		params->a1 = 21;
		params->a2 = 22;
		params->a3 = 23;
		params->a4 = 24;
		params->a5 = 25;
		params->a6 = 26;
		params->a7 = 27;
		break;

	default:
		/* Test outputs */
		params->a0 = 30;
		params->a1 = 31;
		params->a2 = 32;
		params->a3 = 33;
		params->a4 = 34;
		params->a5 = 35;
		params->a6 = 36;
		params->a7 = 37;
		break;
	}
}
