/**
 * @file    qlog_port.c
 * @author  qufeiyan
 * @brief   
 * @version 1.0.0
 * @date    2023/06/11 20:47:44
 * @version Copyright (c) 2023
 */

/* Includes --------------------------------------------------------------------------------*/

#include "qlog_api.h"
#include "qlog_def.h"
#include <stdio.h>

/**
 * @brief Default output function.
 * @note If user do not overload it, fputs in libc will be the default. 
 *
 * @param fmt
 */
__weak void console_puts(const char *str){
    fputs(str, stdout);
}


