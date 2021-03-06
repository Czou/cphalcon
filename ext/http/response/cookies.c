
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
  +------------------------------------------------------------------------+
*/

#include "http/response/cookies.h"
#include "http/response/cookiesinterface.h"
#include "http/response/exception.h"
#include "http/cookie/exception.h"
#include "http/cookie.h"
#include "http/responseinterface.h"
#include "diinterface.h"
#include "di/injectable.h"

#include <main/SAPI.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "kernel/exception.h"
#include "kernel/array.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"

#include "interned-strings.h"

/**
 * Phalcon\Http\Response\Cookies
 *
 * This class is a bag to manage the cookies
 * A cookies bag is automatically registered as part of the 'response' service in the DI
 */
zend_class_entry *phalcon_http_response_cookies_ce;

PHP_METHOD(Phalcon_Http_Response_Cookies, __construct);
PHP_METHOD(Phalcon_Http_Response_Cookies, useEncryption);
PHP_METHOD(Phalcon_Http_Response_Cookies, isUsingEncryption);
PHP_METHOD(Phalcon_Http_Response_Cookies, set);
PHP_METHOD(Phalcon_Http_Response_Cookies, get);
PHP_METHOD(Phalcon_Http_Response_Cookies, has);
PHP_METHOD(Phalcon_Http_Response_Cookies, delete);
PHP_METHOD(Phalcon_Http_Response_Cookies, send);
PHP_METHOD(Phalcon_Http_Response_Cookies, reset);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_http_response_cookies___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, expire)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, secure)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, httpOnly)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_http_response_cookies_method_entry[] = {
	PHP_ME(Phalcon_Http_Response_Cookies, __construct, arginfo_phalcon_http_response_cookies___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_Http_Response_Cookies, useEncryption, arginfo_phalcon_http_response_cookiesinterface_useencryption, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Response_Cookies, isUsingEncryption, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Response_Cookies, set, arginfo_phalcon_http_response_cookiesinterface_set, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Response_Cookies, get, arginfo_phalcon_http_response_cookiesinterface_get, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Response_Cookies, has, arginfo_phalcon_http_response_cookiesinterface_has, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Response_Cookies, delete, arginfo_phalcon_http_response_cookiesinterface_delete, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Response_Cookies, send, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Http_Response_Cookies, reset, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\Http\Response\Cookies initializer
 */
PHALCON_INIT_CLASS(Phalcon_Http_Response_Cookies){

	PHALCON_REGISTER_CLASS_EX(Phalcon\\Http\\Response, Cookies, http_response_cookies, phalcon_di_injectable_ce, phalcon_http_response_cookies_method_entry, 0);

	zend_declare_property_bool(phalcon_http_response_cookies_ce, SL("_registered"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(phalcon_http_response_cookies_ce, SL("_useEncryption"), 1, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_http_response_cookies_ce, SL("_cookies"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_http_response_cookies_ce, SL("_expire"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(phalcon_http_response_cookies_ce, SL("_path"), "/", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_http_response_cookies_ce, SL("_secure"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_http_response_cookies_ce, SL("_domain"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(phalcon_http_response_cookies_ce, SL("_httpOnly"), 1, ZEND_ACC_PROTECTED TSRMLS_CC);

	zend_class_implements(phalcon_http_response_cookies_ce TSRMLS_CC, 1, phalcon_http_response_cookiesinterface_ce);

	return SUCCESS;
}

/**
 * Phalcon\Http\Response\Cookie constructor
 *
 * @param int $expire
 * @param string $path
 * @param boolean $secure
 * @param string $domain
 * @param boolean $httpOnly
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, __construct){

	zval *expire = NULL, *path = NULL, *secure = NULL, *domain = NULL, *http_only = NULL;

	phalcon_fetch_params(0, 0, 5, &expire, &path, &secure, &domain, &http_only);

	if (expire && Z_TYPE_P(expire) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_expire"), expire TSRMLS_CC);
	}

	if (path && Z_TYPE_P(path) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_path"), path TSRMLS_CC);
	}

	if (secure && Z_TYPE_P(secure) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_secure"), secure TSRMLS_CC);
	}

	if (domain && Z_TYPE_P(domain) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_domain"), domain TSRMLS_CC);
	}

	if (http_only && Z_TYPE_P(http_only) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_httpOnly"), http_only TSRMLS_CC);
	}

	phalcon_update_property_empty_array(this_ptr, SL("_cookies") TSRMLS_CC);
}

/**
 * Set if cookies in the bag must be automatically encrypted/decrypted
 *
 * @param boolean $useEncryption
 * @return Phalcon\Http\Response\Cookies
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, useEncryption){

	zval *use_encryption;

	phalcon_fetch_params(0, 1, 0, &use_encryption);

	phalcon_update_property_this(this_ptr, SL("_useEncryption"), use_encryption TSRMLS_CC);
	RETURN_THISW();
}

/**
 * Returns if the bag is automatically encrypting/decrypting cookies
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, isUsingEncryption){


	RETURN_MEMBER(this_ptr, "_useEncryption");
}

/**
 * Sets a cookie to be sent at the end of the request
 * This method overrides any cookie set before with the same name
 *
 * @param string $name
 * @param mixed $value
 * @param int $expire
 * @param string $path
 * @param boolean $secure
 * @param string $domain
 * @param boolean $httpOnly
 * @return Phalcon\Http\Response\Cookies
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, set){

	zval *name, *value = NULL, *expire = NULL, *path = NULL, *secure = NULL, *domain = NULL;
	zval *http_only = NULL, *cookies, *encryption, *dependency_injector = NULL;
	zval *cookie = NULL, *registered, *service, *response = NULL;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 6, &name, &value, &expire, &path, &secure, &domain, &http_only);

	if (Z_TYPE_P(name) != IS_STRING) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_http_cookie_exception_ce, "The cookie name must be string");
		return;
	}

	if (!value) {
		value = PHALCON_GLOBAL(z_null);
	}

	if (!expire) {
		PHALCON_OBS_VAR(expire);
		phalcon_read_property_this(&expire, this_ptr, SL("_expire"), PH_NOISY TSRMLS_CC);
	}

	if (!path) {
		PHALCON_OBS_VAR(path);
		phalcon_read_property_this(&path, this_ptr, SL("_path"), PH_NOISY TSRMLS_CC);
	}

	if (!secure) {
		PHALCON_OBS_VAR(secure);
		phalcon_read_property_this(&secure, this_ptr, SL("_secure"), PH_NOISY TSRMLS_CC);
	}

	if (!domain) {
		PHALCON_OBS_VAR(domain);
		phalcon_read_property_this(&domain, this_ptr, SL("_domain"), PH_NOISY TSRMLS_CC);
	}

	if (!http_only) {
		PHALCON_OBS_VAR(http_only);
		phalcon_read_property_this(&http_only, this_ptr, SL("_httpOnly"), PH_NOISY TSRMLS_CC);
	}

	PHALCON_OBS_VAR(cookies);
	phalcon_read_property_this(&cookies, this_ptr, SL("_cookies"), PH_NOISY TSRMLS_CC);

	PHALCON_OBS_VAR(encryption);
	phalcon_read_property_this(&encryption, this_ptr, SL("_useEncryption"), PH_NOISY TSRMLS_CC);

	PHALCON_CALL_METHOD(&dependency_injector, this_ptr, "getdi");

	/** 
	 * Check if the cookie needs to be updated or 
	 */
	if (!phalcon_array_isset(cookies, name)) {
		PHALCON_INIT_VAR(cookie);
		object_init_ex(cookie, phalcon_http_cookie_ce);
		PHALCON_CALL_METHOD(NULL, cookie, "__construct", name, value, expire, path, secure, domain, http_only);

		/** 
		 * Pass the DI to created cookies
		 */
		PHALCON_CALL_METHOD(NULL, cookie, "setdi", dependency_injector);

		/** 
		 * Enable encryption in the cookie
		 */
		if (zend_is_true(encryption)) {
			PHALCON_CALL_METHOD(NULL, cookie, "useencryption", encryption);
		}

		phalcon_update_property_array(this_ptr, SL("_cookies"), name, cookie TSRMLS_CC);
	} else {
		PHALCON_OBS_NVAR(cookie);
		phalcon_array_fetch(&cookie, cookies, name, PH_NOISY);

		/** 
		 * Override any settings in the cookie
		 */
		PHALCON_CALL_METHOD(NULL, cookie, "setvalue", value);
		PHALCON_CALL_METHOD(NULL, cookie, "setexpiration", expire);
		PHALCON_CALL_METHOD(NULL, cookie, "setpath", path);
		PHALCON_CALL_METHOD(NULL, cookie, "setsecure", secure);
		PHALCON_CALL_METHOD(NULL, cookie, "setdomain", domain);
		PHALCON_CALL_METHOD(NULL, cookie, "sethttponly", http_only);
	}

	/** 
	 * Register the cookies bag in the response
	 */
	PHALCON_OBS_VAR(registered);
	phalcon_read_property_this(&registered, this_ptr, SL("_registered"), PH_NOISY TSRMLS_CC);
	if (PHALCON_IS_FALSE(registered)) {
		PHALCON_INIT_VAR(service);
		PHALCON_ZVAL_MAYBE_INTERNED_STRING(service, phalcon_interned_response);

		PHALCON_CALL_METHOD(&response, dependency_injector, "getshared", service);
		PHALCON_VERIFY_INTERFACE(response, phalcon_http_responseinterface_ce);

		/** 
		 * Pass the cookies bag to the response so it can send the headers at the of the
		 * request
		 */
		PHALCON_CALL_METHOD(NULL, response, "setcookies", this_ptr);
	}

	RETURN_THIS();
}

/**
 * Gets a cookie from the bag
 *
 * @param string $name
 * @return Phalcon\Http\Cookie
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, get){

	zval *name, *cookies, *cookie = NULL, *dependency_injector = NULL;
	zval *encryption;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &name);

	if (Z_TYPE_P(name) != IS_STRING) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_http_cookie_exception_ce, "The cookie name must be string");
		return;
	}

	PHALCON_OBS_VAR(cookies);
	phalcon_read_property_this(&cookies, this_ptr, SL("_cookies"), PH_NOISY TSRMLS_CC);
	if (phalcon_array_isset_fetch(&cookie, cookies, name)) {
		RETURN_CTOR(cookie);
	}

	/** 
	 * Create the cookie if the it does not exist
	 */
	object_init_ex(return_value, phalcon_http_cookie_ce);
	PHALCON_CALL_METHOD(NULL, return_value, "__construct", name);
	
	PHALCON_CALL_METHOD(&dependency_injector, this_ptr, "getdi");

	/** 
	 * Pass the DI to created cookies
	 */
	PHALCON_CALL_METHOD(NULL, return_value, "setdi", dependency_injector);

	PHALCON_OBS_VAR(encryption);
	phalcon_read_property_this(&encryption, this_ptr, SL("_useEncryption"), PH_NOISY TSRMLS_CC);

	/** 
	 * Enable encryption in the cookie
	 */
	if (zend_is_true(encryption)) {
		PHALCON_CALL_METHOD(NULL, return_value, "useencryption", encryption);
	}

	phalcon_update_property_array(this_ptr, SL("_cookies"), name, return_value TSRMLS_CC);

	PHALCON_MM_RESTORE();
}

/**
 * Check if a cookie is defined in the bag or exists in the $_COOKIE superglobal
 *
 * @param string $name
 * @return boolean
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, has){

	zval *name, *cookies, *_COOKIE;

	phalcon_fetch_params(0, 1, 0, &name);

	cookies = phalcon_fetch_nproperty_this(this_ptr, SL("_cookies"), PH_NOISY TSRMLS_CC);

	/* Check the internal bag */
	if (phalcon_array_isset(cookies, name)) {
		RETURN_TRUE;
	}

	/* Check the superglobal */
	_COOKIE = phalcon_get_global(SS("_COOKIE") TSRMLS_CC);
	RETURN_BOOL(phalcon_array_isset(_COOKIE, name));
}

/**
 * Deletes a cookie by its name
 * This method does not removes cookies from the $_COOKIE superglobal
 *
 * @param string $name
 * @return boolean
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, delete){

	zval *name, *cookies, *cookie;
	zval *_COOKIE, *dependency_injector = NULL;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &name);

	PHALCON_OBS_VAR(cookies);
	phalcon_read_property_this(&cookies, this_ptr, SL("_cookies"), PH_NOISY TSRMLS_CC);

	/** 
	 * Check the internal bag
	 */
	if (phalcon_array_isset(cookies, name)) {
		PHALCON_OBS_VAR(cookie);
		phalcon_array_fetch(&cookie, cookies, name, PH_NOISY);
		PHALCON_CALL_METHOD(NULL, cookie, "delete");
		RETURN_MM_TRUE;
	}

	_COOKIE = phalcon_get_global(SS("_COOKIE") TSRMLS_CC);
	if (phalcon_array_isset(_COOKIE, name)) {		
		PHALCON_CALL_METHOD(&dependency_injector, this_ptr, "getdi");
	
		PHALCON_INIT_VAR(cookie);
		object_init_ex(cookie, phalcon_http_cookie_ce);
	
		PHALCON_CALL_METHOD(NULL, cookie, "__construct", name);
		PHALCON_CALL_METHOD(NULL, cookie, "setdi", dependency_injector);
		PHALCON_CALL_METHOD(NULL, cookie, "restore");
		PHALCON_CALL_METHOD(NULL, cookie, "delete");
		RETURN_MM_TRUE;
	}

	RETURN_MM_FALSE;
}

/**
 * Sends the cookies to the client
 * Cookies aren't sent if headers are sent in the current request
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, send){

	zval *cookies, *cookie = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;

	PHALCON_MM_GROW();

	if (!SG(headers_sent)) {

		PHALCON_OBS_VAR(cookies);
		phalcon_read_property_this(&cookies, this_ptr, SL("_cookies"), PH_NOISY TSRMLS_CC);

		phalcon_is_iterable(cookies, &ah0, &hp0, 0, 0);

		while (zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) == SUCCESS) {

			PHALCON_GET_HVALUE(cookie);

			PHALCON_CALL_METHOD(NULL, cookie, "send");

			zend_hash_move_forward_ex(ah0, &hp0);
		}

		RETURN_MM_TRUE;
	}

	RETURN_MM_FALSE;
}

/**
 * Reset set cookies
 *
 * @return Phalcon\Http\Response\Cookies
 */
PHP_METHOD(Phalcon_Http_Response_Cookies, reset){

	phalcon_update_property_empty_array(this_ptr, SL("_cookies") TSRMLS_CC);
	RETURN_THISW();
}
