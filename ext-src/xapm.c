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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xapm.h"
#include "string.h"
#include "time.h"
#include "unistd.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "tdata.pb-c.h"

ZEND_DECLARE_MODULE_GLOBALS(xapm)


PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("xapm.server_ip",      "127.0.0.1", PHP_INI_ALL, OnUpdateString, server_ip, zend_xapm_globals, xapm_globals)
    STD_PHP_INI_ENTRY("xapm.server_port",      "8010", PHP_INI_ALL, OnUpdateLong, server_port, zend_xapm_globals, xapm_globals)
    STD_PHP_INI_ENTRY("xapm.log_path",      "/tmp/trace.log", PHP_INI_ALL, OnUpdateString, log_path, zend_xapm_globals, xapm_globals)
    STD_PHP_INI_ENTRY("xapm.log_remote",      "1", PHP_INI_ALL, OnUpdateLong, log_remote, zend_xapm_globals, xapm_globals)
PHP_INI_END()


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_xapm_compiled(string arg)
   Return a string to confirm that the module is compiled in */

PHP_FUNCTION (confirm_xapm_compiled) {
    char *arg = NULL;
    size_t arg_len, len;
    zend_string *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
        return;
    }

    strg = strpprintf(0,
                      "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.",
                      "xapm", arg);

    RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/

/*
static void php_xapm_init_globals(zend_xapm_globals *xapm_globals) {
    xapm_globals->global_value = 0;
    xapm_globals->global_string = NULL;
}
*/

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION (xapm) {
    REGISTER_INI_ENTRIES();
    // save old execute
    ori_execute_ex = zend_execute_ex;
    ori_execute_internal = zend_execute_internal;
    // replace hook execute
    zend_execute_ex = xapm_execute_ex;
    zend_execute_internal = xapm_execute_internal;
    // get request logid
    logid = get_request_logid();
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION (xapm) {
    UNREGISTER_INI_ENTRIES();
    zend_execute_ex = ori_execute_ex;
    zend_execute_internal = ori_execute_internal;
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION (xapm) {
#if defined(COMPILE_DL_XAPM) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    write_log("xapm_trace_start", 0, 1);
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION (xapm) {
    write_log("xapm_trace_end", 0, 1);
    // 复杂性的计算放到server处理
    char *ret = read_log();
    if (ret && XAPM_G(log_remote)) {
        php_printf("data_transfer_start\n");
        // 发送到服务器
        int sock = create_socket(XAPM_G(server_ip), XAPM_G(server_port));
        write(sock, ret, buf_size);
        close(sock);
        // 清空文件
        FILE *pFile = fopen(XAPM_G(log_path), "w");
        fclose(pFile);
        free(ret);
        php_printf("data_transfer_end\n");
    }
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION (xapm) {
    php_info_print_table_start();
    php_info_print_table_header(2, "xapm support", "enabled");
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ xapm_functions[]
 *
 * Every user visible function must have an entry in xapm_functions[].
 */
const zend_function_entry xapm_functions[] = {
        PHP_FE(confirm_xapm_compiled, NULL)        /* For testing, remove later. */
        PHP_FE_END    /* Must be the last line in xapm_functions[] */
};
/* }}} */

/* {{{ xapm_module_entry
 */
zend_module_entry xapm_module_entry = {
        STANDARD_MODULE_HEADER,
        "xapm",
        xapm_functions,
        PHP_MINIT(xapm),
        PHP_MSHUTDOWN(xapm),
        PHP_RINIT(xapm),        /* Replace with NULL if there's nothing to do at request start */
        PHP_RSHUTDOWN(xapm),    /* Replace with NULL if there's nothing to do at request end */
        PHP_MINFO(xapm),
        PHP_XAPM_VERSION,
        STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XAPM
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(xapm)
#endif


ZEND_API void xapm_execute_ex(zend_execute_data *execute_data) {
    xapm_execute_core(0, execute_data, NULL);
}

ZEND_API void xapm_execute_internal(zend_execute_data *execute_data, zval *return_value) {
    xapm_execute_core(1, execute_data, return_value);
}

ZEND_API void xapm_execute_core(int internal, zend_execute_data *execute_data, zval *return_value) {
    /* class and function name */
    char *func_name = get_function(internal, execute_data, NULL TSRMLS_CC);
    if (func_name) {
        //php_printf("%s\n", func_name);
        write_log(func_name, 0, 1);
    }

    long inc_time = pt_time_msec();

    // Call original under zend_try
    if (internal) {
        if (ori_execute_internal) {
            ori_execute_internal(execute_data, return_value);
        } else {
            execute_internal(execute_data, return_value);
        }
    } else {
        ori_execute_ex(execute_data);
    }
    /*
    if(return_value) {
        php_printf("ret:%s\n", repr_zval(return_value));
    }else{
        if (execute_data->return_value) {
            php_printf("ret:%s\n", repr_zval(execute_data->return_value));
        }
    }
    */
    if (func_name) {
        inc_time = pt_time_msec() - inc_time;
        write_log(inc_time, 1, 1);
    }
}

static long get_function(zend_bool internal, zend_execute_data *ex, zend_op_array *op_array TSRMLS_DC) {
    zend_function *zf = ex->func;
    zval retval;
    char result[256];
    char input[128] = "";
    int arg_count = 0;
    int i = 0;
    char *argc_tmp = NULL;
    char *argc_list[20];
    arg_count = ZEND_CALL_NUM_ARGS(ex);
    if(arg_count){
        zval *p = ZEND_CALL_ARG(ex, 1);
        if (ex->func->type == ZEND_USER_FUNCTION) {
            uint32_t first_extra_arg = ex->func->op_array.num_args;

            if (first_extra_arg && arg_count > first_extra_arg) {
                while (i < first_extra_arg) {
                    argc_tmp = repr_zval(p++);
                    if(argc_tmp) {
                        argc_list[i] = argc_tmp;
                    }
                    i++;
                }
                p = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var + ex->func->op_array.T);
            }
        }
        while(i < arg_count) {
            argc_tmp = repr_zval(p++);
            if(argc_tmp) {
                argc_list[i] = argc_tmp;
            }
            i++;
        }
        // input argc
        for(i=0;i<arg_count;i++){
            if(i==0){
                strcpy(input,argc_list[i]);
            }else{
                strcat(input, "|");
                strcat(input, argc_list[i]);
            }
        }
    }

    // class::function
    if (zf->common.scope && zf->common.function_name) {
        if (zf->common.scope && zf->common.scope->trait_aliases) {
            sprintf(result, "%s:%d*%s::%s*%s", zend_get_executed_filename(TSRMLS_C), zend_get_executed_lineno(TSRMLS_C),
                    P7_STR(zf->common.scope->name),
                    P7_STR(zend_resolve_method_name(P7_EX_OBJ(ex) ? P7_EX_OBJCE(ex) : zf->common.scope, zf)),input);
        } else {
            sprintf(result, "%s:%d*%s::%s*%s", zend_get_executed_filename(TSRMLS_C), zend_get_executed_lineno(TSRMLS_C),
                    P7_STR(zf->common.scope->name), P7_STR(zf->common.function_name),input);
        }
        return result;
    }
    // function
    if (zf->common.function_name) {
        if (zf->common.scope && zf->common.scope->trait_aliases) {
            sprintf(result, "%s:%d*%s*%s", zend_get_executed_filename(TSRMLS_C), zend_get_executed_lineno(TSRMLS_C),
                    P7_STR(zend_resolve_method_name(P7_EX_OBJ(ex) ? P7_EX_OBJCE(ex) : zf->common.scope, zf)),input);
        } else {
            sprintf(result, "%s:%d*%s*%s", zend_get_executed_filename(TSRMLS_C), zend_get_executed_lineno(TSRMLS_C),
                    P7_STR(zf->common.function_name),input);
        }
        return result;
    }
    return NULL;
}


static char* repr_zval(zval *zv) {
    char buf[100];
    switch (Z_TYPE_P(zv)) {
        case IS_STRING:
            return Z_STRVAL_P(zv);
        case IS_TRUE:
            return "true";
        case IS_FALSE:
            return "false";
        case IS_LONG:
            snprintf(buf, sizeof(buf), "%ld", Z_LVAL_P(zv));
            return buf;
        case IS_DOUBLE:
            snprintf(buf, sizeof(buf), "%.*G", (int) EG(precision), Z_DVAL_P(zv));
            return buf;
        default:
            return NULL;
    }
    return NULL;
}

static int create_socket(char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);
    connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    return sock;
}

static long pt_time_msec() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (long) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static long get_request_logid() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (long) (tv.tv_sec * 1000000 + tv.tv_usec);
}

void write_log(char *log_info, int type, int newline) {
    time_t timer;
    char buffer[26];
    struct tm *tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    FILE *pFile = fopen(XAPM_G(log_path), "a+");
    if (pFile) {
        switch (type) {
            case 0:
                if(newline){
                    fprintf(pFile, "%s\t%s\n", buffer, log_info);
                }
                else{
                    fprintf(pFile, "%s\t%s", buffer, log_info);
                }
                break;
            case 1:
                fprintf(pFile, "%s\t%ld\n", buffer, log_info);
                break;
            default:
                break;
        }
        fclose(pFile);
    }
}

char *read_log() {
    long lSize = 0;
    char *buffer = NULL;
    size_t result = 0;
    FILE *pFile = fopen(XAPM_G(log_path), "r");
    // 文件打开失败
    if (pFile == NULL) {
        return NULL;
    }
    // 获取文件大小
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);
    // 文件大小为0
    if (!lSize) {
        return NULL;
    }
    // 空间分配失败
    buffer = (char *) malloc(sizeof(char) * (lSize + 1));
    if (buffer == NULL) {
        return NULL;
    }
    result = fread(buffer, 1, lSize, pFile);
    if (result != lSize) {
        return NULL;
    }
    buffer[lSize] = '\0';
    fclose(pFile);
    buf_size = lSize;
    return buffer;
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
