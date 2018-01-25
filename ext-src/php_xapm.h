/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: tanhp@outlook.com                                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_XAPM_H
#define PHP_XAPM_H

extern zend_module_entry xapm_module_entry;
#define phpext_xapm_ptr &xapm_module_entry

#define PHP_XAPM_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_XAPM_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_XAPM_API __attribute__ ((visibility("default")))
#else
#	define PHP_XAPM_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/

ZEND_BEGIN_MODULE_GLOBALS(xapm)
    zend_long server_port;
    char *server_ip;
    char *log_path;
    int log_remote;
ZEND_END_MODULE_GLOBALS(xapm)


/* Always refer to the globals in your function as XAPM_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define XAPM_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(xapm, v)
#define P7_EX_OBJ(ex)   (Z_TYPE(ex->This) == IS_OBJECT ? Z_OBJ(ex->This) : NULL)
#define P7_EX_OBJCE(ex) Z_OBJCE(ex->This)
#define P7_EX_OPARR(ex) (&(ex->func->op_array))
#define P7_STR(v)       ZSTR_VAL(v)
#define P7_STR_LEN(v)   ZSTR_LEN(v)

#if defined(ZTS) && defined(COMPILE_DL_XAPM)
ZEND_TSRMLS_CACHE_EXTERN()
#endif


static int le_xapm;
static int buf_size = 0;
static long logid = 0;

static void (*ori_execute_ex)(zend_execute_data *execute_data);
static void (*ori_execute_internal)(zend_execute_data *execute_data, zval *return_value);

ZEND_API void xapm_execute_ex(zend_execute_data *execute_data);
ZEND_API void xapm_execute_internal(zend_execute_data *execute_data, zval *return_value);
ZEND_API void xapm_execute_core(int internal, zend_execute_data *execute_data, zval *return_value);

static long get_function(zend_bool internal, zend_execute_data *ex, zend_op_array *op_array TSRMLS_DC);
static int create_socket(char *ip, int port);
static long pt_time_msec();
static long get_request_logid();
static char* repr_zval(zval *zv);

void write_log(char *log_info, int type, int newline);
char *read_log();

#endif    /* PHP_XAPM_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
