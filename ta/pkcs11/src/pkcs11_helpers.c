// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2018-2020, Linaro Limited
 */

#include <pkcs11_ta.h>
#include <string.h>
#include <tee_internal_api.h>
#include <util.h>

#include "pkcs11_helpers.h"

static const char __maybe_unused unknown[] = "<unknown-identifier>";

struct any_id {
	uint32_t id;
#if CFG_TEE_TA_LOG_LEVEL > 0
	const char *string;
#endif
};

/*
 * Macro PKCS11_ID() can be used to define cells in ID list arrays
 * or ID/string conversion arrays.
 */
#if CFG_TEE_TA_LOG_LEVEL > 0
#define PKCS11_ID(_id)		{ .id = _id, .string = #_id }
#else
#define PKCS11_ID(_id)		{ .id = _id }
#endif

#define ID2STR(id, table, prefix)	\
	id2str(id, table, ARRAY_SIZE(table), prefix)

#if CFG_TEE_TA_LOG_LEVEL > 0
/* Convert a PKCS11 ID into its label string */
static const char *id2str(uint32_t id, const struct any_id *table,
			  size_t count, const char *prefix)
{
	size_t n = 0;
	const char *str = NULL;

	for (n = 0; n < count; n++) {
		if (id != table[n].id)
			continue;

		str = table[n].string;

		/* Skip prefix provided matches found */
		if (prefix && !TEE_MemCompare(str, prefix, strlen(prefix)))
			str += strlen(prefix);

		return str;
	}

	return unknown;
}
#endif /* CFG_TEE_TA_LOG_LEVEL > 0 */

/*
 * TA command IDs: used only as ID/string conversion for debug trace support
 */
static const struct any_id __maybe_unused string_ta_cmd[] = {
	PKCS11_ID(PKCS11_CMD_PING),
};

/*
 * Conversion between PKCS11 TA and GPD TEE return codes
 */
enum pkcs11_rc tee2pkcs_error(TEE_Result res)
{
	switch (res) {
	case TEE_SUCCESS:
		return PKCS11_CKR_OK;

	case TEE_ERROR_BAD_PARAMETERS:
		return PKCS11_CKR_ARGUMENTS_BAD;

	case TEE_ERROR_CIPHERTEXT_INVALID:
		return PKCS11_CKR_ENCRYPTED_DATA_INVALID;

	case TEE_ERROR_OUT_OF_MEMORY:
		return PKCS11_CKR_DEVICE_MEMORY;

	case TEE_ERROR_SHORT_BUFFER:
		return PKCS11_CKR_BUFFER_TOO_SMALL;

	case TEE_ERROR_MAC_INVALID:
	case TEE_ERROR_SIGNATURE_INVALID:
		return PKCS11_CKR_SIGNATURE_INVALID;

	default:
		return PKCS11_CKR_GENERAL_ERROR;
	}
}

/*
 * Helper functions to analyse PKCS11 identifiers
 */

/* Check attribute ID is known and size matches if fixed */
bool valid_pkcs11_attribute_id(uint32_t id, uint32_t size)
{
	size_t n = 0;

	for (n = 0; n < ARRAY_SIZE(attr_ids); n++)
		if (id == attr_ids[n].id)
			return !attr_ids[n].size || size == attr_ids[n].size;

	return false;
}

size_t pkcs11_attr_is_type(uint32_t attribute_id)
{
	enum pkcs11_attr_id id = attribute_id;

	switch (id) {
	case PKCS11_CKA_KEY_TYPE:
	case PKCS11_CKA_MECHANISM_TYPE:
	case PKCS11_CKA_KEY_GEN_MECHANISM:
		return sizeof(uint32_t);
	default:
		return 0;
	}
}

bool pkcs11_attr_has_indirect_attributes(uint32_t attribute_id)
{
	switch (attribute_id) {
	case PKCS11_CKA_WRAP_TEMPLATE:
	case PKCS11_CKA_UNWRAP_TEMPLATE:
	case PKCS11_CKA_DERIVE_TEMPLATE:
		return true;
	default:
		return false;
	}
}

bool pkcs11_class_has_type(uint32_t class)
{
	enum pkcs11_class_id class_id = class;

	switch (class_id) {
	case PKCS11_CKO_CERTIFICATE:
	case PKCS11_CKO_PUBLIC_KEY:
	case PKCS11_CKO_PRIVATE_KEY:
	case PKCS11_CKO_SECRET_KEY:
	case PKCS11_CKO_MECHANISM:
	case PKCS11_CKO_HW_FEATURE:
		return true;
	default:
		return false;
	}
}

bool pkcs11_attr_class_is_key(uint32_t class)
{
	enum pkcs11_class_id class_id = class;

	switch (class_id) {
	case PKCS11_CKO_SECRET_KEY:
	case PKCS11_CKO_PUBLIC_KEY:
	case PKCS11_CKO_PRIVATE_KEY:
		return true;
	default:
		return false;
	}
}

bool key_type_is_symm_key(uint32_t id)
{
	enum pkcs11_key_type key_type = id;

	switch (key_type) {
	case PKCS11_CKK_AES:
	case PKCS11_CKK_GENERIC_SECRET:
	case PKCS11_CKK_MD5_HMAC:
	case PKCS11_CKK_SHA_1_HMAC:
	case PKCS11_CKK_SHA224_HMAC:
	case PKCS11_CKK_SHA256_HMAC:
	case PKCS11_CKK_SHA384_HMAC:
	case PKCS11_CKK_SHA512_HMAC:
		return true;
	default:
		return false;
	}
}

bool key_type_is_asymm_key(uint32_t id)
{
	enum pkcs11_key_type key_type = id;

	switch (key_type) {
	case PKCS11_CKK_EC:
	case PKCS11_CKK_EC_EDWARDS:
	case PKCS11_CKK_RSA:
		return true;
	default:
		return false;
	}
}

/*
 * Returns shift position or -1 on error.
 * Mainly used when PKCS11_SHEAD_WITH_BOOLPROPS is enabled
 */
int pkcs11_attr2boolprop_shift(uint32_t attr)
{
	static const uint32_t bpa[] = {
		[BPA_TOKEN]		= PKCS11_CKA_TOKEN,
		[BPA_PRIVATE]		= PKCS11_CKA_PRIVATE,
		[BPA_TRUSTED]		= PKCS11_CKA_TRUSTED,
		[BPA_SENSITIVE]		= PKCS11_CKA_SENSITIVE,
		[BPA_ENCRYPT]		= PKCS11_CKA_ENCRYPT,
		[BPA_DECRYPT]		= PKCS11_CKA_DECRYPT,
		[BPA_WRAP]		= PKCS11_CKA_WRAP,
		[BPA_UNWRAP]		= PKCS11_CKA_UNWRAP,
		[BPA_SIGN]		= PKCS11_CKA_SIGN,
		[BPA_SIGN_RECOVER]	= PKCS11_CKA_SIGN_RECOVER,
		[BPA_VERIFY]		= PKCS11_CKA_VERIFY,
		[BPA_VERIFY_RECOVER]	= PKCS11_CKA_VERIFY_RECOVER,
		[BPA_DERIVE]		= PKCS11_CKA_DERIVE,
		[BPA_EXTRACTABLE]	= PKCS11_CKA_EXTRACTABLE,
		[BPA_LOCAL]		= PKCS11_CKA_LOCAL,
		[BPA_NEVER_EXTRACTABLE]	= PKCS11_CKA_NEVER_EXTRACTABLE,
		[BPA_ALWAYS_SENSITIVE]	= PKCS11_CKA_ALWAYS_SENSITIVE,
		[BPA_MODIFIABLE]	= PKCS11_CKA_MODIFIABLE,
		[BPA_COPYABLE]		= PKCS11_CKA_COPYABLE,
		[BPA_DESTROYABLE]	= PKCS11_CKA_DESTROYABLE,
		[BPA_ALWAYS_AUTHENTICATE] = PKCS11_CKA_ALWAYS_AUTHENTICATE,
		[BPA_WRAP_WITH_TRUSTED] = PKCS11_CKA_WRAP_WITH_TRUSTED,
	};
	size_t pos = 0;

	for (pos = 0; pos < ARRAY_SIZE(bpa); pos++)
		if (bpa[pos] == attr)
			return (int)pos;

	return -1;
}

/* Initialize a TEE attribute for a target PKCS11 TA attribute in an object */
bool pkcs2tee_load_attr(TEE_Attribute *tee_ref, uint32_t tee_id,
			struct pkcs11_object *obj,
			enum pkcs11_attr_id pkcs11_id)
{
	void *a_ptr = NULL;
	uint8_t *der_ptr = NULL;
	uint32_t a_size = 0;
	uint32_t data32 = 0;
	size_t hsize = 0;
	size_t qsize = 0;

	switch (tee_id) {
	case TEE_ATTR_ECC_PUBLIC_VALUE_X:
	case TEE_ATTR_ECC_PUBLIC_VALUE_Y:
	case TEE_ATTR_ECC_CURVE:
		if (get_attribute_ptr(obj->attributes, PKCS11_CKA_EC_PARAMS,
				      &a_ptr, &a_size) || !a_ptr) {
			EMSG("Missing EC_PARAMS attribute");
			return false;
		}

		if (tee_id == TEE_ATTR_ECC_CURVE) {
			data32 = ec_params2tee_curve(a_ptr, a_size);
			TEE_InitValueAttribute(tee_ref, TEE_ATTR_ECC_CURVE,
					       data32, 0);
			return true;
		}

		data32 = (ec_params2tee_keysize(a_ptr, a_size) + 7) / 8;

		if (get_attribute_ptr(obj->attributes, PKCS11_CKA_EC_POINT,
				      &a_ptr, &a_size)) {
			/*
			 * Public X/Y is required for both TEE keypair and
			 * public key, so abort if EC_POINT is not provided
			 * during object import.
			 */

			EMSG("Missing EC_POINT attribute");
			return false;
		}

		der_ptr = (uint8_t *)a_ptr;

		if (der_ptr[0] != 0x04) {
			EMSG("Unsupported DER type");
			return false;
		}

		if ((der_ptr[1] & 0x80) == 0) {
			/* DER short definitive form up to 127 bytes */
			qsize = der_ptr[1] & 0x7F;
			hsize = 2 /* der */ + 1 /* point compression */;
		} else if (der_ptr[1] == 0x81) {
			/* DER long definitive form up to 255 bytes */
			qsize = der_ptr[2];
			hsize = 3 /* der */ + 1 /* point compression */;
		} else {
			EMSG("Unsupported DER long form");
			return false;
		}

		if (der_ptr[hsize - 1] != 0x04) {
			EMSG("Unsupported EC_POINT compression");
			return false;
		}

		if (a_size != (hsize - 1) + qsize) {
			EMSG("Invalid EC_POINT attribute");
			return false;
		}

		if (a_size != hsize + 2 * data32) {
			EMSG("Invalid EC_POINT attribute");
			return false;
		}

		if (tee_id == TEE_ATTR_ECC_PUBLIC_VALUE_X)
			TEE_InitRefAttribute(tee_ref, tee_id,
					     der_ptr + hsize, data32);
		else
			TEE_InitRefAttribute(tee_ref, tee_id,
					     der_ptr + hsize + data32,
					     data32);

		return true;

	default:
		break;
	}

	if (get_attribute_ptr(obj->attributes, pkcs11_id, &a_ptr, &a_size))
		return false;

	TEE_InitRefAttribute(tee_ref, tee_id, a_ptr, a_size);

	return true;
}

/*
 * Initialize a TEE attribute with hash of a target PKCS11 TA attribute
 * in an object.
 */
enum pkcs11_rc pkcs2tee_load_hashed_attr(TEE_Attribute *tee_ref,
					 uint32_t tee_id,
					 struct pkcs11_object *obj,
					 enum pkcs11_attr_id pkcs11_id,
					 uint32_t tee_algo, void *hash_ptr,
					 uint32_t *hash_size)
{
	TEE_OperationHandle handle = TEE_HANDLE_NULL;
	void *a_ptr = NULL;
	uint32_t a_size = 0;
	enum pkcs11_rc rc = PKCS11_CKR_OK;
	TEE_Result res = TEE_ERROR_GENERIC;
	size_t tmp_sz = 0;

	rc = get_attribute_ptr(obj->attributes, pkcs11_id, &a_ptr, &a_size);
	if (rc)
		return rc;

	res = TEE_AllocateOperation(&handle, tee_algo, TEE_MODE_DIGEST, 0);
	if (res) {
		EMSG("TEE_AllocateOperation() failed %#"PRIx32, tee_algo);
		return tee2pkcs_error(res);
	}

	tmp_sz = *hash_size;
	res = TEE_DigestDoFinal(handle, a_ptr, a_size, hash_ptr, &tmp_sz);
	*hash_size = tmp_sz;
	TEE_FreeOperation(handle);
	if (res) {
		EMSG("TEE_DigestDoFinal() failed %#"PRIx32, tee_algo);
		return PKCS11_CKR_FUNCTION_FAILED;
	}

	TEE_InitRefAttribute(tee_ref, tee_id, hash_ptr, *hash_size);

	return PKCS11_CKR_OK;
}

/* Easy conversion between PKCS11 TA function of TEE crypto mode */
void pkcs2tee_mode(uint32_t *tee_id, enum processing_func function)
{
	switch (function) {
	case PKCS11_FUNCTION_ENCRYPT:
		*tee_id = TEE_MODE_ENCRYPT;
		break;
	case PKCS11_FUNCTION_DECRYPT:
		*tee_id = TEE_MODE_DECRYPT;
		break;
	case PKCS11_FUNCTION_SIGN:
		*tee_id = TEE_MODE_SIGN;
		break;
	case PKCS11_FUNCTION_VERIFY:
		*tee_id = TEE_MODE_VERIFY;
		break;
	case PKCS11_FUNCTION_DERIVE:
		*tee_id = TEE_MODE_DERIVE;
		break;
	case PKCS11_FUNCTION_DIGEST:
		*tee_id = TEE_MODE_DIGEST;
		break;
	default:
		TEE_Panic(function);
	}
}

#if CFG_TEE_TA_LOG_LEVEL > 0
const char *id2str_ta_cmd(uint32_t id)
{
	return ID2STR(id, string_ta_cmd, NULL);
}
#endif /*CFG_TEE_TA_LOG_LEVEL*/
