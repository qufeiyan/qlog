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
#include "qlog_api.h"
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
#define LOG_COLOR_YELLOW     "2;93m"
#define LOG_COLOR_BLUE       "0;94m"
#define LOG_COLOR_MAGENTA    "4;95m"
#define LOG_COLOR_CYAN       "1;96m"

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


__weak void _logger_log(logger_t *logger, const char *tag, level_t level, const char *format, va_list args);
void loggerInit(logger_t *logger, level_t level, formatter_t *formatter, writer_t *writer, filter_t *filter){
    assert(logger);
    assert(formatter != NULL);
    assert(writer != NULL);
     
    logger->level = level,
    memset(&logger->buffer, 0, sizeof(logger->buffer));
    logger->formatter = formatter;
    logger->writer = writer;
    logger->filter = filter;

    logger->formatter->buffer = logger->buffer;
    logger->writer->buffer = logger->buffer;

    logger->run = _logger_log;
}

void loggerDeInit(logger_t *logger){
    assert(logger);

    memset(&logger->buffer, 0, sizeof(logger->buffer));
    logger->level = LOG_LEVEL_FATAL;
    logger->formatter = NULL;
    logger->writer = NULL;
    logger->run = NULL;
}

/**
 * @brief   output a log.
 *
 * @param   logger is pointer to the logger.
 * @param   tag is the name of module.
 * @param   level is the level of log.
 * @param   format is the string to ouput.
 * @param   args is the arguments list for format string.
 * @note    
 * @see     
 */
__weak void _logger_log(logger_t *logger, const char *tag, level_t level, const char *format, va_list args){
    filter_t *filter;
    formatter_t *formater;
    writer_t *writer;
    assert(logger && format);
    
    //! global filter, if current level > logger.level, there is nothing to output.
    if(level > logger->level){
        return;
    }

    //! filter tag.
    assert(logger->filter != NULL);
    filter = logger->filter;
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
}


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

void formatterInit(struct formatter *formatter, bool color, bool timestamp, char *buffer){
    assert(formatter != NULL);
    assert(buffer != NULL);
    formatter->buffer = buffer;
    formatter->color = color;
    formatter->timestamp = timestamp;
    formatter->invoke = _formatter_invoke;
}

void consoleWriter_write(struct writer *writer){
    assert(writer != NULL);
    assert(writer->buffer != NULL);

    console_puts(writer->buffer);
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
    writer->write = consoleWriter_write;
}

void writerNext(writer_t *head, writer_t *target){
    assert(head);
    writer_t *current = head;
    while(current->next){
        current = current->next;
    }
    
    current->next = target;
}







