/**
 * @file    qlog_api.c
 * @author  qufeiyan
 * @brief   
 * @version 1.0.0
 * @date    2023/06/11 19:56:26
 * @version Copyright (c) 2023
 */

/* Includes --------------------------------------------------------------------------------*/
#include "qlog_api.h"
#include "mempool.h"
#include "qlog.h"
#include "qlog_port.h"
#include <assert.h>
#include <stdarg.h>

#define SIZE_OF_TAG         sizeof(filter_tag_t)
#define SIZE_OF_TAG_BLOCK   (sizeof(uint8_t *) + SIZE_OF_TAG)
#define SIZE_OF_TAG_POOL    (COUNT_OF_TAG * SIZE_OF_TAG_BLOCK)

static logger_t *logger_unique; //! global unique logger.

/**
 * @brief   initialise the unique logger.
 *
 * @param   level is the global level of log.
 * @param   color means wether to output log with color.
 * @param   timestamp means wether to output log with timestamp.
 * @param   tagcount is the number of tag. 
 * @return  the unique logger.  
 * @see     
 */
logger_t *qlog_init(level_t level, bool color, bool timestamp, size_t tag_count){
    static logger_t logger;
    static filter_t filter;
    static formatter_t formatter;
    static writer_t writer;
    static memoryPool_t mp;
    static char tag_pool[SIZE_OF_TAG_POOL];
    static locker_t locker; 

    assert(level < LOG_LEVEL_BUTT);
    assert(tag_count <= SIZE_OF_TAG_BLOCK);
    
    //! initialise a locker for logger.
    lockerInit(&locker, locker_init(NULL));

    memoryPoolInit(&mp, "tag_pool", tag_count, sizeof(filter_tag_t), &tag_pool);
    filterInit(&filter, &mp, logger.buffer, level);
    formatterInit(&formatter, color, timestamp, logger.buffer);
    consoleWriterInit(&writer, logger.buffer);
    loggerInit(&logger, level, &formatter, &writer, &filter, &locker);
    logger_unique = &logger;
    return &logger;
}

/**
 * @brief   qlog api for output log. 
 * @param   tag is tag of current log.
 * @param   level is level of current log.
 * @param   format is format string.  
 * @note    
 * @see     
 */
void qlog(const char *tag, level_t level, const char *format, ...){
    assert(logger_unique != NULL);
    logger_t *logger = logger_unique;

    va_list args;
    
    /* args point to the first variable parameter */
    va_start(args, format);
    logger->run(logger, tag, level, format, args);
    va_end(args);
}

/**
 * @brief   append a tag to filter list.
 * @param   tag is pointer to the tag.
 * @param   level is level of the tag.
 * @note    logs with this tag will be filtered later.
 */
void qlog_filter(const char *tag, level_t level){
    assert(logger_unique != NULL);
    assert(tag != NULL);

    logger_t *logger = logger_unique;
    filter_t *filter = logger->filter;
    assert(filter != NULL);
    assert(filter->append != NULL);

    filter->append(filter, tag, level);
}

