/**
 * @file    qlog.c
 * @author  qufeiyan
 * @brief   Define a logger structure.
 * @version 1.0.0
 * @date    2023/06/06 21:25:23
 * @version Copyright (c) 2023
 */

/* Includes --------------------------------------------------------------------------------*/
#include "qlog.h"
#include "mempool.h"
#include "qlog_port.h"
#include "qlog_def.h"
#include "qlog_slist.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


#define LOG_COLOR_START      "\x1B["
#define LOG_COLOR_END        "\x1B[0;m"
#define LOG_COLOR_RED        "4;91m"
#define LOG_COLOR_GREEN      "1;92m"
#define LOG_COLOR_YELLOW     "3;93m"
#define LOG_COLOR_BLUE       "0;94m"
#define LOG_COLOR_MAGENTA    "4;95m"
#define LOG_COLOR_CYAN       "0;96m"

#define LOG_COLOR_DEBUG      LOG_COLOR_GREEN
#define LOG_COLOR_INFO       LOG_COLOR_CYAN
#define LOG_COLOR_WARN       LOG_COLOR_YELLOW
#define LOG_COLOR_ERROR      LOG_COLOR_MAGENTA
#define LOG_COLOR_FATAL      LOG_COLOR_RED

// #define LOG_CTRL_LEVEL_ERROR (1)
// #define LOG_CTRL_LEVEL_WARN  (2)
// #define LOG_CTRL_LEVEL_INFO  (3)
// #define LOG_CTRL_LEVEL_DEBUG (4)    

static const char * const color_info[] = {
    LOG_COLOR_FATAL,
    LOG_COLOR_ERROR,
    LOG_COLOR_WARN,
    LOG_COLOR_INFO,
    LOG_COLOR_DEBUG
};

/* level output info */
static const char * const level_info[] = {
    "F/",
    "E/",
    "W/",
    "I/",
    "D/",
};

/**
 * @brief  provide lock api.
 * @param  locker is pointer to a locker. 
 * @note    
 * @see     
 */
void _locker_lock(struct locker *locker){
    assert(locker != NULL);
    assert(locker->locker != NULL);
    locker_lock(locker->locker);
}

/**
 * @brief   provide unlock api.
 * @param   locker is pointer to a locker.
 * @note    
 * @see     
 */
void _locker_unlock(struct locker *locker){
    assert(locker != NULL);
    assert(locker->locker != NULL);
    locker_unlock(locker->locker);
}

/**
 * @brief append a writer to writer list.
 * @param head is the head of list.
 * @param target is the writer to be append.
 */
static void _writerNext(writer_t *head, writer_t *target){
    assert(head);
    writer_t *current = head;
    while(current->next){
        current = current->next;
    }
    
    current->next = target;
}

/**
 * @brief  register a writer to logger.
 *
 * @param  logger is pointer to logger.   
 * @param  writer is a new writer to register. 
 */
void _registerWriter(logger_t *logger, writer_t *writer){
    assert(logger != NULL);
    assert(writer != NULL);
    assert(logger->writer != NULL);

    _writerNext(logger->writer, writer);
}

/**
 * @brief   output a log.
 *
 * @param   logger is pointer to the logger.
 * @param   tag is the name of module.
 * @param   level is the level of log.
 * @param   format is the format string to ouput.
 * @param   args is a list of variable parameters.
 * @note    
 * @see     
 */
__weak void _logger_log(logger_t *logger, const char *tag, level_t level, const char *format, va_list args){
    filter_t *filter;
    formatter_t *formater;
    writer_t *writer;
    locker_t *locker;
    assert(logger && format);
    
    //! global filter, if current level > logger.level, there is nothing to output.
    if(level > logger->level){
        return;
    }

    //! filter tag.
    assert(logger->filter != NULL);
    filter = logger->filter;

    assert(logger->locker != NULL);
    locker = logger->locker;
    
    locker->lock(locker);
    if(filter->invoke && filter->invoke(filter, tag, level)){
        return;
    }

    //! formater
    assert(logger->formatter != NULL);
    formater = logger->formatter;
    if(formater->invoke){
        formater->invoke(formater, tag, level, format, args);
    }
    
    //! writer
    writer = logger->writer;
    assert(writer != NULL);
    writer->write(writer);
    locker->unlock(locker);
}

/**
 * @brief   append a tag to filter-tag list.
 *
 * @param   filter is pointer to filter.   
 * @param   tag is pointer to tag which will be append.
 * @param   level is the level of the tag.
 */
void _filter_append(struct filter *filter, const char *tag, level_t level){
    filter_tag_t *filter_tag;
    assert(filter != NULL);
    assert(tag != NULL);
    assert(level < LOG_LEVEL_BUTT);
    assert(filter->tagMemoryPool);

    filter_tag = (filter_tag_t *)memoryPoolAlloc(filter->tagMemoryPool);
    assert(filter_tag != NULL);
    filter_tag->level = level;
    // filter_tag->tag = tag;
    strcpy(filter_tag->tag, tag);

    slist_append(&filter->list_tag, &filter_tag->list_tag_current);    
}

/**
 * @brief   invoke a filter.
 *
 * @param   filter is pointer to filter.
 * @param   tag is the tag of current log.
 * @param   level is the level of current log.
 * @return  true means tag will be filtered.    
 * @see     
 */
bool _filter_invoke(struct filter *filter, const char *tag, level_t level){
    slist_t *current;
    filter_tag_t *filter_tag;
    assert(filter);

    //! 1. tag is nil.
    if(tag == NULL && level <= filter->level){
        return false;
    }

    //! 2. filter list is nil.
    if(filter->list_tag.next == NULL && level <= filter->level){
        return false;
    }

    //! 3. find tag in the filter tag list.
    slist_for_each(current, &filter->list_tag){
        filter_tag = container_of(current, filter_tag_t, list_tag_current);
        if(strcmp(tag, filter_tag->tag) == 0   /*find the tag.*/
            && filter_tag->level >= level){    
            return false;
        }
    }

    return true;
}

/**
 * @brief   invoke a formatter.
 *
 * @param   formatter is pointer to formatter.
 * @param   tag is tag of current log.
 * @param   level is level of current log.
 * @param   format is format string of current log.
 * @param   args is the arguments list.   
 */
void _formatter_invoke(struct formatter *formatter, const char *tag, level_t level, const char *format, va_list args){
    uint32_t length;
    uint32_t colorEndLength = 0;
    assert(formatter != NULL && formatter->buffer != NULL);
    // assert(tag != NULL);
    assert(format != NULL);
    assert(level < LOG_LEVEL_BUTT);

    length = 0;
    //! color start.
    if(formatter->color){
        memcpy(formatter->buffer, LOG_COLOR_START, sizeof(LOG_COLOR_START) - 1);
        length += sizeof(LOG_COLOR_START) - 1;

        memcpy(formatter->buffer + length, color_info[level], strlen(color_info[level]));
        length += strlen(color_info[level]);

        colorEndLength = sizeof(LOG_COLOR_END) - 1; 
    }

    //! timestamp
    if(formatter->timestamp){
        static struct timeval now;
        static struct tm *tm, tm_tmp;
        time_t t = (time_t)0;

        if (gettimeofday(&now, NULL) >= 0)
        {
            t = now.tv_sec;
        }
        tm = localtime_r(&t, &tm_tmp);
        /* show the time format MM-DD HH:MM:SS */
        snprintf(formatter->buffer + length, SIZE_OF_LOG_BUFFER - length, "%02d-%02d %02d:%02d:%02d", tm->tm_mon + 1,
                tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

        /* show the millisecond */
        length += strlen(formatter->buffer + length);
        snprintf(formatter->buffer + length, SIZE_OF_LOG_BUFFER - length, ".%03ld ", now.tv_usec / 1000);
        length += strlen(formatter->buffer + length);
    }

    //! level info
    memcpy(formatter->buffer + length, level_info[level], strlen(level_info[level]));
    length += strlen(level_info[level]);

    //! tag info
    memcpy(formatter->buffer + length, tag, strlen(tag));
    length += strlen(tag);

    //! append ": " 
    formatter->buffer[length++] = ':';
    formatter->buffer[length++] = ' ';

    //! append content
    length += vsnprintf(formatter->buffer + length, SIZE_OF_LOG_BUFFER - length, format, args);

    //! cut off.
    if(length + colorEndLength + sizeof((char)'\0') > SIZE_OF_LOG_BUFFER){
        /* using max length */
        length = SIZE_OF_LOG_BUFFER;
        /* reserve some space for CSI end sign */
        length -= colorEndLength;
        /* reserve some space for string end sign */
        length -= sizeof((char)'\0');
    }

    if(formatter->color){
        memcpy(formatter->buffer + length, LOG_COLOR_END, colorEndLength);
        length += colorEndLength;
    }

    formatter->buffer[length] = '\0';
}

/**
 * @brief  output a string to console.  
 *
 * @param  writer is pointer to writer. 
 * @note   if another writer exists, it will be excuted sequentially. 
 * @see     
 */
void _consoleWriter_write(struct writer *writer){
    assert(writer != NULL);
    assert(writer->buffer != NULL);

    console_puts(writer->buffer);

    writer_t *nextWriter = writer->next;
    if(nextWriter){
        nextWriter->write(nextWriter);
    }
}

/**
 * @brief  initialise a filter. 
 * @param  filter is pointer to filter.
 * @param  mp is pointer to memory pool, where {@code filter_tag} will be allocated. 
 * @param  buffer is pointer to the log buffer. 
 * @param  level is level of the log. 
 * @see     
 */
void filterInit(struct filter *filter, memoryPool_t *mp, char *buffer, level_t level){
    assert(filter != NULL);
    assert(level < LOG_LEVEL_BUTT);

    filter->level = level;
    slist_init(&filter->list_tag);
    filter->buffer = buffer;
    filter->append = _filter_append;
    filter->invoke = _filter_invoke;
    filter->tagMemoryPool = mp;
}

/**
 * @brief   initialise a formatter.   
 * @param   formatter is pointer to formatter.
 * @param   color means whether to output log with color.
 * @param   timestamp means whether to output log with timestamp.
 * @param   buffer is pointer to log buffer.
 * @note    
 * @see     
 */
void formatterInit(struct formatter *formatter, bool color, bool timestamp, char *buffer){
    assert(formatter != NULL);
    assert(buffer != NULL);
    formatter->buffer = buffer;
    formatter->color = color;
    formatter->timestamp = timestamp;
    formatter->invoke = _formatter_invoke;
}

/**
 * @brief   default writer.
 * @param   writer is pointer to the writer.
 * @param   buffer is pointer to the log buffer.
 */
void consoleWriterInit(struct writer *writer, char *buffer){
    assert(writer != NULL);
    assert(buffer != NULL);

    memset(writer, 0, sizeof(*writer));
    strcpy(writer->name, "console");
    writer->buffer = buffer;
    writer->next = NULL;
    writer->write = _consoleWriter_write;
}

/**
 * @brief  initialise a logger.  
 * @param  logger is pointer to logger.
 * @param  level is global level of log. 
 * @param  formatter is pointer to a formatter. 
 * @param  writer is pointer to a writer.  
 * @param  filter is pointer to a filter. 
 * @param  locker is pointer to a locker.   
 * @see     
 */
void loggerInit(logger_t *logger, level_t level, 
    formatter_t *formatter, writer_t *writer, filter_t *filter, locker_t *locker){
    assert(logger);
    assert(formatter != NULL);
    assert(writer != NULL);
    assert(locker != NULL);
     
    logger->level = level,
    memset(&logger->buffer, 0, sizeof(logger->buffer));
    logger->formatter = formatter;
    logger->writer = writer;
    logger->filter = filter;

    logger->formatter->buffer = logger->buffer;
    logger->writer->buffer = logger->buffer;

    logger->run = _logger_log;
    logger->registerWriter = _registerWriter;

    logger->locker = locker;
}

/**
 * @brief   deinitialise a logger.
 * @param   logger is pointer to a logger.
 * @note    
 * @see     
 */
void loggerDeInit(logger_t *logger){
    assert(logger);

    memset(&logger->buffer, 0, sizeof(logger->buffer));
    logger->level = LOG_LEVEL_FATAL;
    logger->formatter = NULL;
    logger->writer = NULL;
    logger->run = NULL;
}

void lockerInit(struct locker *locker, void *mutex){
    assert(locker != NULL);
    assert(mutex != NULL);

    locker->locker = mutex;
    locker->lock = _locker_lock;
    locker->unlock = _locker_unlock;
}


