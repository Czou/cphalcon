/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2013 Phalcon Team (http://www.phalconphp.com)       |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "php_open_temporary_file.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/exception.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/hash.h"
#include "kernel/concat.h"
#include "kernel/operators.h"
#include "kernel/string.h"
#include "kernel/output.h"

#include "chart/captcha.h"
#include "chart/exception.h"

/**
 * Phalcon\Chart\Captcha
 *
 *<code>
 * header('Content-Type: image/png');
 * $captcha = new \Phalcon\Chart\Captcha(NULL, NULL, 30, 150, 50);
 * echo $captcha = $qr->render('Phalcon', 15, -10);
 *</code>
 */
zend_class_entry *phalcon_chart_captcha_ce;

PHP_METHOD(Phalcon_Chart_Captcha, __construct);
PHP_METHOD(Phalcon_Chart_Captcha, setFont);
PHP_METHOD(Phalcon_Chart_Captcha, setFontSize);
PHP_METHOD(Phalcon_Chart_Captcha, render);
PHP_METHOD(Phalcon_Chart_Captcha, save);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_chart_captcha_setfont, 0, 0, 1)
	ZEND_ARG_INFO(0, font)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_chart_captcha_setfontsize, 0, 0, 1)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_chart_captcha_render, 0, 0, 1)
	ZEND_ARG_INFO(0, word)
	ZEND_ARG_INFO(0, margin)
	ZEND_ARG_INFO(0, foreground)
	ZEND_ARG_INFO(0, background)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_chart_captcha_save, 0, 0, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_chart_captcha_method_entry[] = {
	PHP_ME(Phalcon_Chart_Captcha, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_Chart_Captcha, setFont, arginfo_phalcon_chart_captcha_setfont, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Chart_Captcha, setFontSize, arginfo_phalcon_chart_captcha_setfontsize, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Chart_Captcha, render, arginfo_phalcon_chart_captcha_render, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Chart_Captcha, save, arginfo_phalcon_chart_captcha_save, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\Chart\Captcha initializer
 */
PHALCON_INIT_CLASS(Phalcon_Chart_Captcha){

	PHALCON_REGISTER_CLASS(Phalcon\\Chart, Captcha, chart_captcha, phalcon_chart_captcha_method_entry, 0);

	zend_declare_property_null(phalcon_chart_captcha_ce, SL("_imagick"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_chart_captcha_ce, SL("_word"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(phalcon_chart_captcha_ce, SL("_font"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(phalcon_chart_captcha_ce, SL("_fontSize"), 40, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(phalcon_chart_captcha_ce, SL("_width"), 150, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(phalcon_chart_captcha_ce, SL("_height"), 50, ZEND_ACC_PROTECTED TSRMLS_CC);

	return SUCCESS;
}

/**
 * Phalcon\Chart\Captcha constructor
 *
 *     $qr = new \Phalcon\Chart\Captcha;
 *     $qr->generate('Phalcon is a web framework');
 *     $qr->save('qr.png');
 */
PHP_METHOD(Phalcon_Chart_Captcha, __construct){

	zval *word = NULL, *font = NULL, *font_size = NULL, *width = NULL, *height = NULL;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 0, 5, &word, &font, &font_size, &width, &height);

	if (!phalcon_class_exists(SL("imagick"), 0 TSRMLS_CC)) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_chart_exception_ce, "Imagick is not installed, or the extension is not loaded");
		return;
	}

	if (word && Z_TYPE_P(word) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_word"), word TSRMLS_CC);
	}

	if (font && Z_TYPE_P(font) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_font"), font TSRMLS_CC);
	}

	if (font_size && Z_TYPE_P(font_size) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_fontSize"), font_size TSRMLS_CC);
	}

	if (width && Z_TYPE_P(width) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_width"), width TSRMLS_CC);
	}

	if (height && Z_TYPE_P(height) != IS_NULL) {
		phalcon_update_property_this(this_ptr, SL("_height"), height TSRMLS_CC);
	}

	PHALCON_MM_RESTORE();
}

/**
 * Sets a font
 *
 * @param string $font
 * @return boolean
 */
PHP_METHOD(Phalcon_Chart_Captcha, setFont){

	zval *font;

	phalcon_fetch_params(0, 1, 0, &font);

	phalcon_update_property_this(this_ptr, SL("_font"), font TSRMLS_CC);
	RETURN_THISW();
}

/**
 * Sets a font size
 *
 * @param string $fontSize
 * @return boolean
 */
PHP_METHOD(Phalcon_Chart_Captcha, setFontSize){

	zval *font_size;

	phalcon_fetch_params(0, 1, 0, &font_size);

	phalcon_update_property_this(this_ptr, SL("_fontSize"), font_size TSRMLS_CC);
	RETURN_THISW();
}

/**
 * Generate Captcha data
 *
 *<code>
 *     $qr = new \Phalcon\Chart\Captcha;
 *     $qr->reander('Phalcon is a web framework');
 *</code>
 *
 * @param string $word
 * @param string $margin
 * @return String
 */
PHP_METHOD(Phalcon_Chart_Captcha, render){

	zval *word = NULL, *offset_x = NULL, *offset_y = NULL, *foreground = NULL, *background = NULL, *width = NULL, *height = NULL;
	zval *font, *font_size, *imagick, *draw, *imagickpixel, *gravity;
	zval *min, *max, *roll1 = NULL, *roll2 = NULL, *corner1 = NULL, *corner2 = NULL;
	zval *format;
	zend_class_entry  *imagick_ce, *imagickpixel_ce, *draw_ce;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 0, 6, &word, &offset_x, &offset_y, &foreground, &background, &width, &height);

	if (!word || Z_TYPE_P(word) == IS_NULL) {
		word  = phalcon_fetch_nproperty_this(this_ptr, SL("_word"), PH_NOISY TSRMLS_CC);
	}

	if (!offset_x || Z_TYPE_P(offset_x) == IS_NULL) {
		offset_x  = PHALCON_GLOBAL(z_zero);
	}

	if (!offset_y || Z_TYPE_P(offset_y) == IS_NULL) {
		offset_y  = PHALCON_GLOBAL(z_zero);
	}

	if (!foreground || Z_TYPE_P(foreground) == IS_NULL) {
		PHALCON_INIT_VAR(foreground);
		ZVAL_STRING(foreground, "#ffffff", 1);
	}

	if (!background || Z_TYPE_P(background) == IS_NULL) {
		PHALCON_INIT_VAR(background);
		ZVAL_STRING(background, "#000000", 1);
	}

	if (!width || Z_TYPE_P(width) == IS_NULL) {
		width  = phalcon_fetch_nproperty_this(this_ptr, SL("_width"), PH_NOISY TSRMLS_CC);
	}

	if (!height || Z_TYPE_P(height) == IS_NULL) {
		height  = phalcon_fetch_nproperty_this(this_ptr, SL("_height"), PH_NOISY TSRMLS_CC);
	}

	imagick_ce = zend_fetch_class(SL("Imagick"), ZEND_FETCH_CLASS_AUTO TSRMLS_CC);
	imagickpixel_ce = zend_fetch_class(SL("ImagickPixel"), ZEND_FETCH_CLASS_AUTO TSRMLS_CC);
	draw_ce = zend_fetch_class(SL("ImagickDraw"), ZEND_FETCH_CLASS_AUTO TSRMLS_CC);

	PHALCON_INIT_VAR(imagickpixel);
	object_init_ex(imagickpixel, imagickpixel_ce);
	PHALCON_CALL_METHOD(NULL, imagickpixel, "__construct", background);

	PHALCON_INIT_VAR(imagick);
	object_init_ex(imagick, imagick_ce);
	if (phalcon_has_constructor(imagick TSRMLS_CC)) {
		PHALCON_CALL_METHOD(NULL, imagick, "__construct");
	}

	PHALCON_CALL_METHOD(NULL, imagick, "newimage", width,  height, imagickpixel);

	PHALCON_INIT_VAR(draw);
	object_init_ex(draw, draw_ce);
	if (phalcon_has_constructor(draw TSRMLS_CC)) {
		PHALCON_CALL_METHOD(NULL, draw, "__construct");
	}

	font  = phalcon_fetch_nproperty_this(this_ptr, SL("_font"), PH_NOISY TSRMLS_CC);
	if (font && Z_TYPE_P(font) != IS_NULL) {
		PHALCON_CALL_METHODW(NULL, draw, "setfont", font);
	}

	font_size  = phalcon_fetch_nproperty_this(this_ptr, SL("_fontSize"), PH_NOISY TSRMLS_CC);
	PHALCON_CALL_METHODW(NULL, draw, "setfontsize", font_size);

	PHALCON_INIT_NVAR(imagickpixel);
	object_init_ex(imagickpixel, imagickpixel_ce);
	PHALCON_CALL_METHOD(NULL, imagickpixel, "__construct", foreground);
	PHALCON_CALL_METHOD(NULL, draw, "setfillcolor", imagickpixel);

	PHALCON_INIT_VAR(gravity);
	phalcon_get_class_constant(gravity, imagick_ce, SS("GRAVITY_CENTER") TSRMLS_CC);

	PHALCON_CALL_METHOD(NULL, draw, "setgravity", gravity);
	PHALCON_CALL_METHOD(NULL, imagick, "annotateimage", draw, offset_x, offset_y, PHALCON_GLOBAL(z_zero), word);

	PHALCON_INIT_VAR(min);
	ZVAL_LONG(min, 20);

	PHALCON_INIT_VAR(max);
	ZVAL_LONG(max, 50);

	PHALCON_CALL_FUNCTION(&roll1, "rand", min, max);

	PHALCON_CALL_METHOD(NULL, imagick, "rollimage", roll1,  PHALCON_GLOBAL(z_zero));

	ZVAL_LONG(min, 30);

	PHALCON_CALL_FUNCTION(&corner1, "rand", min, max);

	ZVAL_LONG(corner1, -Z_LVAL_P(corner1));

	PHALCON_CALL_METHOD(NULL, imagick, "swirlimage", corner1);

	PHALCON_INIT_VAR(roll2);
	ZVAL_LONG(roll2, -Z_LVAL_P(roll1)*2);

	PHALCON_CALL_METHOD(NULL, imagick, "rollimage", roll2,  PHALCON_GLOBAL(z_zero));

	PHALCON_CALL_FUNCTION(&corner2, "rand", min, max);
	PHALCON_CALL_METHOD(NULL, imagick, "swirlimage", corner2);

	PHALCON_CALL_METHOD(NULL, imagick, "rollimage", roll1,  PHALCON_GLOBAL(z_zero));

	PHALCON_INIT_VAR(format);
	ZVAL_STRING(format, "png", 1);

	PHALCON_CALL_METHOD(NULL, imagick, "setImageFormat", format);
	PHALCON_CALL_METHOD(NULL, imagick, "stripImage");

	PHALCON_RETURN_CALL_METHOD(imagick, "getImageBlob");
	phalcon_update_property_this(this_ptr, SL("_imagick"), imagick TSRMLS_CC);

	RETURN_MM();
}

/**
 * Save the image
 *
 *<code>
 *     $qr = new \Phalcon\Chart\Captcha;
 *     $qr->reander('Phalcon is a web framework');
 *     $qr->save('captcha.png');
 *</code>
 *
 * @param filename $filename
 * @return boolean
 */
PHP_METHOD(Phalcon_Chart_Captcha, save){

	zval *filename, *imagick;

	PHALCON_MM_GROW();

	phalcon_fetch_params(1, 1, 0, &filename);

	imagick = phalcon_fetch_nproperty_this(this_ptr, SL("_imagick"), PH_NOISY TSRMLS_CC);

	PHALCON_RETURN_CALL_METHOD(imagick, "writeImage", filename);

	RETURN_MM();
}
