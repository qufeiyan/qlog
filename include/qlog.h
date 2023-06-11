/**
 * @file    qlog.h
 * @author  qufeiyan
 * @brief   Define basic data tructrue for qlog.c
 * @version 1.0.0
 * @date    2023/06/09 03:08:05
 * @version Copyright (c) 2023
 */

/* Define to prevent recursive inclusion ---------------------------------------------------*/
#ifndef __QLOG_H
#define __QLOG_H
/* Include ---------------------------------------------------------------------------------*/
#include "mempool.h"
#include "qlog_def.h"
#include "qlog_slist.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

enum level{
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_BUTT
};

typedef enum level level_t;


struct filter_tag{
    char tag[SIZE_OF_NAME];
    level_t level;
    slist_t list_tag_current;
};
typedef struct filter_tag filter_tag_t;

struct filter{
    memoryPool_t *tagMemoryPool;  //! used to alloc memory for filter_tag.
    slist_t list_tag;  //! all the tags to be filtered.
    char *buffer;      //! pointer to the log buffer.
    level_t level;     //! global level.
    //! append tag to be filtered to the list_tag.
    void (*append)(struct filter *, const char *tag, level_t level);  
    //! filter tag.
    bool (*invoke)(struct filter *, const char *tag, level_t level);
};
typedef struct filter filter_t;

struct formatter{
    bool timestamp;
    bool color;
    char *buffer;  //! pointer to the log buffer.

    void (*invoke)(struct formatter *formatter, const char *tag, level_t level, const char *format, va_list args);
};
typedef struct formatter formatter_t;

struct writer{
    char name[SIZE_OF_NAME];
    char *buffer;                   //! pointer to the log buffer.

    void (*init)(struct writer*);
    void (*deInit)(struct writer*);
    void (*write)(struct writer*);
    void (*flush)(struct writer*);

    struct writer *next;
};
typedef struct writer writer_t;

struct logger{
    level_t level;
    char buffer[SIZE_OF_LOG_BUFFER];

    void (*run)(struct logger *logger, const char *tag, level_t level, const char *fmt, va_list args);

    formatter_t *formatter;
    filter_t *filter;

    writer_t *writer;
};

typedef struct logger logger_t;

void loggerInit(logger_t *logger, level_t level, formatter_t *formatter, writer_t *writer, filter_t *filter);
void loggerDeInit(logger_t *logger);

void filterInit(struct filter *filter, memoryPool_t *mp, char *buffer, level_t level);
void formatterInit(struct formatter *formatter, bool color, bool timestamp, char *buffer);
void consoleWriterInit(struct writer *writer, char *buffer);

#ifdef __cplusplus
}
#endif

#endif	//  __QLOG_H