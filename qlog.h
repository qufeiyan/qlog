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
#include "qlog_def.h"
#include "qlog_slist.h"
#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif



struct filter{
    slist_t list_tag;  //! all the tags to be filtered.
    
    void (*append)(struct filter *, const char *tag);  //! append tag to be filtered to the list_tag.
    bool (*invoke)(struct filter *, const char *tag);
};
typedef struct filter filter_t;

struct formatter{
    bool timestamp;
    bool color;
    
    char *(*invoke)(struct formatter*, char *, uint32_t);
};
typedef struct formatter formatter_t;

struct writer{
    char name[SIZE_OF_NAME];
    void (*init)(struct writer*);
    void (*deInit)(struct writer*);
    void (*write)(struct writer*);
    void (*flush)(struct writer*);

    slist_t list;
};
typedef struct writer writer_t;

struct logger{
    level_t level;
    char buffer[SIZE_OF_LOG_BUFFER];

    void (*run)(struct logger *, const char *tag, level_t level, char *log);

    formatter_t *formatter;
    writer_t *writer;
    filter_t *filter;
};

typedef struct logger logger_t;

#ifdef __cplusplus
}
#endif

#endif	//  __QLOG_H