/**
 * @file    qlog_api.h
 * @author  qufeiyan
 * @brief   Exposed to api.
 * @version 1.0.0
 * @date    2023/06/11 19:53:01
 * @version Copyright (c) 2023
 */

/* Define to prevent recursive inclusion ---------------------------------------------------*/
#ifndef __QLOG_API_H
#define __QLOG_API_H
/* Include ---------------------------------------------------------------------------------*/

#include "qlog.h"
#ifdef __cplusplus
extern "C" {
#endif

#define COUNT_OF_TAG        (32)    //! maximum number of the filter tag.


logger_t *qlog_init(level_t level, bool color, bool timestamp, size_t tag_count);
void qlog(const char *tag, level_t level, const char *format, ...);

void console_puts(const char *str);


#ifdef __cplusplus
}
#endif

#endif	//  __QLOG_API_H