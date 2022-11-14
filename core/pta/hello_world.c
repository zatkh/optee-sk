#include <compiler.h>
#include <string.h>
#include <kernel/pseudo_ta.h>
#include <pta_hello_world.h>
#include <tee_api_defines.h>
#include <tee_api_types.h>
#include <kernel/tee_time.h>

#define TA_NAME		"hello_world.pta"

static TEE_Result hw_say_hi(void)
{
	DMSG("Hello from \"%s\"!", TA_NAME);

	return TEE_SUCCESS;
}

static TEE_Result hw_test_no_params(uint32_t param_types)
{
	const uint32_t expected_param_types = TEE_PARAM_TYPES(
		TEE_PARAM_TYPE_NONE,
		TEE_PARAM_TYPE_NONE,
		TEE_PARAM_TYPE_NONE,
		TEE_PARAM_TYPE_NONE);

	if (param_types != expected_param_types) {
		EMSG("Bad parameter types, expected %u but got %u",
			expected_param_types, param_types);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	DMSG("Parameter types OK");
	return TEE_SUCCESS;
}

static TEE_Result hw_test_in_value_params(uint32_t param_types,
	TEE_Param params[TEE_NUM_PARAMS])
{
	const uint32_t expected_param_types = TEE_PARAM_TYPES(
		TEE_PARAM_TYPE_VALUE_INPUT,
		TEE_PARAM_TYPE_VALUE_INPUT,
		TEE_PARAM_TYPE_VALUE_INPUT,
		TEE_PARAM_TYPE_VALUE_INPUT);

	if (param_types != expected_param_types) {
		EMSG("Bad parameter types, expected %u but got %u",
			expected_param_types, param_types);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	DMSG("Input parameter values (as uint32_t):");
	DMSG("   0: %u, %u", params[0].value.a, params[0].value.b);
	DMSG("   1: %u, %u", params[1].value.a, params[1].value.b);
	DMSG("   2: %u, %u", params[2].value.a, params[2].value.b);
	DMSG("   3: %u, %u", params[3].value.a, params[3].value.b);

	return TEE_SUCCESS;
}

static TEE_Result hw_test_in_memref_params(uint32_t param_types,
	TEE_Param params[TEE_NUM_PARAMS])
{
	const uint32_t expected_param_types = TEE_PARAM_TYPES(
		TEE_PARAM_TYPE_MEMREF_INPUT,
		TEE_PARAM_TYPE_MEMREF_INPUT,
		TEE_PARAM_TYPE_MEMREF_INPUT,
		TEE_PARAM_TYPE_MEMREF_INPUT);

	if (param_types != expected_param_types) {
		EMSG("Bad parameter types, expected %u but got %u",
			expected_param_types, param_types);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	DMSG("Input parameter values (as string):");
	DMSG("   0: %s", (char *)params[0].memref.buffer);
	DMSG("   1: %s", (char *)params[1].memref.buffer);
	DMSG("   2: %s", (char *)params[2].memref.buffer);
	DMSG("   3: %s", (char *)params[3].memref.buffer);

	return TEE_SUCCESS;
}

static TEE_Result hw_test_out_value_params(uint32_t param_types,
	TEE_Param params[TEE_NUM_PARAMS])
{
	const uint32_t expected_param_types = TEE_PARAM_TYPES(
		TEE_PARAM_TYPE_VALUE_OUTPUT,
		TEE_PARAM_TYPE_VALUE_OUTPUT,
		TEE_PARAM_TYPE_VALUE_OUTPUT,
		TEE_PARAM_TYPE_VALUE_OUTPUT);

	if (param_types != expected_param_types) {
		EMSG("Bad parameter types, expected %u but got %u",
			expected_param_types, param_types);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	params[0].value.a = 8; params[0].value.b = 7;
	params[1].value.a = 6; params[1].value.b = 5;
	params[2].value.a = 4; params[2].value.b = 3;
	params[3].value.a = 2; params[3].value.b = 1;

	DMSG("Output parameter values (as uint32_t):");
	DMSG("   0: %u, %u", params[0].value.a, params[0].value.b);
	DMSG("   1: %u, %u", params[1].value.a, params[1].value.b);
	DMSG("   2: %u, %u", params[2].value.a, params[2].value.b);
	DMSG("   3: %u, %u", params[3].value.a, params[3].value.b);

	return TEE_SUCCESS;
}

static TEE_Result hw_test_out_memref_params(uint32_t param_types,
	TEE_Param params[TEE_NUM_PARAMS])
{
	const uint32_t expected_param_types = TEE_PARAM_TYPES(
		TEE_PARAM_TYPE_MEMREF_OUTPUT,
		TEE_PARAM_TYPE_MEMREF_OUTPUT,
		TEE_PARAM_TYPE_MEMREF_OUTPUT,
		TEE_PARAM_TYPE_MEMREF_OUTPUT);

	if (param_types != expected_param_types) {
		EMSG("Bad parameter types, expected %u but got %u",
			expected_param_types, param_types);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	const char *test_param1 = "Let";
	const char *test_param2 = "there";
	const char *test_param3 = "be";
	const char *test_param4 = "light";

	memcpy(params[0].memref.buffer, test_param1, strlen(test_param1) + 1);
	memcpy(params[1].memref.buffer, test_param2, strlen(test_param2) + 1);
	memcpy(params[2].memref.buffer, test_param3, strlen(test_param3) + 1);
	memcpy(params[3].memref.buffer, test_param4, strlen(test_param4) + 1);

	DMSG("Output parameter values (as string):");
	DMSG("   0: %s", (char *)params[0].memref.buffer);
	DMSG("   1: %s", (char *)params[1].memref.buffer);
	DMSG("   2: %s", (char *)params[2].memref.buffer);
	DMSG("   3: %s", (char *)params[3].memref.buffer);

	return TEE_SUCCESS;
}

static TEE_Result hw_test_get_ree_time(void)
{
	TEE_Result res;
    TEE_Time time;
	DMSG("Test OCALL to get REE time");
    res = tee_time_get_ree_time(&time);
	if (res == TEE_SUCCESS) {
		DMSG("Time in seconds is %d", time.seconds);
		return TEE_SUCCESS;
	} else {
		DMSG("Did not successfully get the time");
		return TEE_ERROR_GENERIC;
	}   
}

/*
 * Trusted Application Entry Points
 */

static TEE_Result create_ta(void)
{
	DMSG("create entry point for pseudo TA \"%s\"", TA_NAME);
	return TEE_SUCCESS;
}

static void destroy_ta(void)
{
	DMSG("destroy entry point for pseudo ta \"%s\"", TA_NAME);
}

static TEE_Result open_session(uint32_t nParamTypes __unused,
		TEE_Param pParams[TEE_NUM_PARAMS] __unused,
		void **ppSessionContext __unused)
{
	DMSG("open entry point for pseudo ta \"%s\"", TA_NAME);
	return TEE_SUCCESS;
}

static void close_session(void *pSessionContext __unused)
{
	DMSG("close entry point for pseudo ta \"%s\"", TA_NAME);
}

static TEE_Result invoke_command(void *context __unused,
		uint32_t cmd_id, uint32_t param_types,
		TEE_Param params[TEE_NUM_PARAMS])
{
	DMSG("command entry point for pseudo ta \"%s\"", TA_NAME);

	switch (cmd_id) {
	case PTA_HELLO_WORLD_SAY_HI:
		return hw_say_hi();
	case PTA_HELLO_WORLD_TEST_NO_PARAMS:
		return hw_test_no_params(param_types);
	case PTA_HELLO_WORLD_TEST_IN_VALUE_PARAMS:
		return hw_test_in_value_params(param_types, params);
	case PTA_HELLO_WORLD_TEST_IN_MEMREF_PARAMS:
		return hw_test_in_memref_params(param_types, params);
	case PTA_HELLO_WORLD_TEST_OUT_VALUE_PARAMS:
		return hw_test_out_value_params(param_types, params);
	case PTA_HELLO_WORLD_TEST_OUT_MEMREF_PARAMS:
		return hw_test_out_memref_params(param_types, params);
	case PTA_HELLO_WORLD_TEST_GET_REE_TIME:
		return hw_test_get_ree_time();
	default:
		return TEE_ERROR_NOT_IMPLEMENTED;
	}

}

pseudo_ta_register(.uuid = PTA_HELLO_WORLD_UUID, .name = TA_NAME,
		   .flags = PTA_DEFAULT_FLAGS | TA_FLAG_SECURE_DATA_PATH |
			    TA_FLAG_CONCURRENT,
		   .create_entry_point = create_ta,
		   .destroy_entry_point = destroy_ta,
		   .open_session_entry_point = open_session,
		   .close_session_entry_point = close_session,
		   .invoke_command_entry_point = invoke_command);
