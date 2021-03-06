
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  |          ZhuZongXin <dreamsxin@qq.com>                                 |
  +------------------------------------------------------------------------+
*/

#include "validation/validator/alpha.h"
#include "validation/validator.h"
#include "validation/validatorinterface.h"
#include "validation/message.h"
#include "validation/exception.h"
#include "validation.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/concat.h"
#include "kernel/operators.h"
#include "kernel/string.h"

#include "interned-strings.h"

/**
 * Phalcon\Validation\Validator\Alpha
 *
 * Check for alphanumeric character(s)
 *
 *<code>
 *use Phalcon\Validation\Validator\Alpha as AlphaValidator;
 *
 *$validator->add('username', new AlphaValidator(array(
 *   'message' => 'The username is not valid'
 *)));
 *</code>
 */
zend_class_entry *phalcon_validation_validator_alpha_ce;

PHP_METHOD(Phalcon_Validation_Validator_Alpha, validate);
PHP_METHOD(Phalcon_Validation_Validator_Alpha, valid);

static const zend_function_entry phalcon_validation_validator_alpha_method_entry[] = {
	PHP_ME(Phalcon_Validation_Validator_Alpha, validate, arginfo_phalcon_validation_validatorinterface_validate, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Validation_Validator_Alpha, valid, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\Validation\Validator\Alpha initializer
 */
PHALCON_INIT_CLASS(Phalcon_Validation_Validator_Alpha){

	PHALCON_REGISTER_CLASS_EX(Phalcon\\Validation\\Validator, Alpha, validation_validator_alpha, phalcon_validation_validator_ce, phalcon_validation_validator_alpha_method_entry, 0);

	zend_class_implements(phalcon_validation_validator_alpha_ce TSRMLS_CC, 1, phalcon_validation_validatorinterface_ce);

	return SUCCESS;
}

/**
 * Executes the validation
 *
 * @param Phalcon\Validation $validator
 * @param string $attribute
 * @return boolean
 */
PHP_METHOD(Phalcon_Validation_Validator_Alpha, validate){

	zval *validator, *attribute, *value = NULL, *allow_empty, *valid = NULL, *label;
	zval *pairs, *message_str, *code, *prepared = NULL, *message;
	zend_class_entry *ce = Z_OBJCE_P(getThis());

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 2, 0, &validator, &attribute);
	
	PHALCON_VERIFY_CLASS_EX(validator, phalcon_validation_ce, phalcon_validation_exception_ce, 1);

	PHALCON_CALL_METHOD(&value, validator, "getvalue", attribute);
	
	PHALCON_OBS_VAR(allow_empty);
	RETURN_MM_ON_FAILURE(phalcon_validation_validator_getoption_helper(ce, &allow_empty, getThis(), phalcon_interned_allowEmpty TSRMLS_CC));
	if (zend_is_true(allow_empty) && phalcon_validation_validator_isempty_helper(value)) {
		RETURN_MM_TRUE;
	}

	PHALCON_CALL_SELF(&valid, "valid", value);

	if (PHALCON_IS_FALSE(valid)) {
		PHALCON_OBS_VAR(label);
		RETURN_MM_ON_FAILURE(phalcon_validation_validator_getoption_helper(ce, &label, getThis(), phalcon_interned_label TSRMLS_CC));
		if (!zend_is_true(label)) {
			PHALCON_CALL_METHOD(&label, validator, "getlabel", attribute);
			if (!zend_is_true(label)) {
				PHALCON_CPY_WRT(label, attribute);
			}
		}

		PHALCON_ALLOC_GHOST_ZVAL(pairs);
		array_init_size(pairs, 1);
		Z_ADDREF_P(label); add_assoc_zval_ex(pairs, SS(":field"), label);

		PHALCON_OBS_VAR(message_str);
		RETURN_MM_ON_FAILURE(phalcon_validation_validator_getoption_helper(ce, &message_str, getThis(), phalcon_interned_message TSRMLS_CC));
		if (!zend_is_true(message_str)) {
			PHALCON_OBSERVE_OR_NULLIFY_VAR(message_str);
			RETURN_MM_ON_FAILURE(phalcon_validation_getdefaultmessage_helper(Z_OBJCE_P(validator), &message_str, validator, "Alpha" TSRMLS_CC));
		}
	
		PHALCON_OBS_VAR(code);
		RETURN_MM_ON_FAILURE(phalcon_validation_validator_getoption_helper(ce, &code, getThis(), phalcon_interned_code TSRMLS_CC));
		if (Z_TYPE_P(code) == IS_NULL) {
			ZVAL_LONG(code, 0);
		}

		PHALCON_CALL_FUNCTION(&prepared, "strtr", message_str, pairs);

		message = phalcon_validation_message_construct_helper(prepared, attribute, "Alpha", code TSRMLS_CC);
		Z_DELREF_P(message);
	
		PHALCON_CALL_METHOD(NULL, validator, "appendmessage", message);
		RETURN_MM_FALSE;
	}
	
	RETURN_MM_TRUE;
}

/**
 * Executes the validation
 *
 * @param string $value
 * @return boolean
 */
PHP_METHOD(Phalcon_Validation_Validator_Alpha, valid){

	zval *value, *regex, *valid;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &value);

	PHALCON_INIT_VAR(regex);
	ZVAL_STRING(regex, "/[^[:alpha:]]/imu", 1);

	PHALCON_INIT_VAR(valid);
	RETURN_MM_ON_FAILURE(phalcon_preg_match(valid, regex, value, NULL TSRMLS_CC));

	if (zend_is_true(valid)) {
		RETURN_MM_FALSE;
	}
	
	RETURN_MM_TRUE;
}
