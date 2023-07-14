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

// #include "qlog.h"
#include <stdbool.h>
#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif

enum level{
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_BUTT
};
typedef enum level level_t;
typedef struct logger logger_t;

#define qlog_err(tag, fmt, ...) do{\
    qlog(tag, LOG_LEVEL_ERROR, "[%s:%d](#%s) " fmt, \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
} while(0)

#define qlog_warn(tag, fmt, ...) do{\
    qlog(tag, LOG_LEVEL_WARNING, "[%s:%d](#%s) " fmt, \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
} while(0)

#define qlog_info(tag, fmt, ...) do{\
    qlog(tag, LOG_LEVEL_INFO, "[%s:%d](#%s) " fmt, \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
} while(0)

#define qlog_dbg(tag, fmt, ...) do{\
    qlog(tag, LOG_LEVEL_DEBUG, "[%s:%d](#%s) " fmt, \
        __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
} while(0)

#undef TAG_NAME
#undef loge
#undef logi
#undef logd
#undef logw

#define loge(fmt, ...) \
    qlog_err(TAG_NAME, fmt, ##__VA_ARGS__)

#define logw(fmt, ...) \
    qlog_warn(TAG_NAME, fmt, ##__VA_ARGS__)

#define logi(fmt, ...) \
    qlog_info(TAG_NAME, fmt, ##__VA_ARGS__)

#define logd(fmt, ...) \
    qlog_dbg(TAG_NAME, fmt, ##__VA_ARGS__)
    

/**
 * @brief   initialise the unique logger.
 *
 * @param   level is the global level of log.
 * @param   color means wether to output log with color.
 * @param   timestamp means wether to output log with timestamp.
 * @param   tagcount is the number of tag. 
 * @return  the unique logger.     
 */
logger_t *qlog_init(level_t level, bool color, bool timestamp, size_t tag_count);

/**
 * @brief   append a tag to filter list.
 * @param   tag is pointer to the tag.
 * @param   level is level of the tag.
 * @note    logs with this tag will be filtered later.
 */
void qlog_filter(const char *tag, level_t level);

/**
 * @brief   qlog api for output log. 
 * @param   tag is tag of current log.
 * @param   level is level of current log.
 * @param   format is format string.  
 */
void qlog(const char *tag, level_t level, const char *format, ...) __attribute__((format(printf, 3, 4)));

void qlog_setConsoleWriter(bool enable);
void qlog_setFileWriter(bool enable);
void qlog_registerWriter(void *writer);
void qlog_registerFileWriter(const char *name, const char *dir, int numberOfFiles, int sizeOfFile);


#ifdef __cplusplus
}
#endif

#endif	//  __QLOG_API_H
