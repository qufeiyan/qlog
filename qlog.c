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
#include "qlog_slist.h"
#include <cstdarg>
#include <cstdio>
#include <stdio.h>


#define LOG_COLOR_START      "\x1B["
#define LOG_COLOR_END        "\x1B[0;m"
#define LOG_COLOR_RED        "\x1B[4;91m"
#define LOG_COLOR_GREEN      "\x1B[1;92m"
#define LOG_COLOR_YELLOW     "\x1B[2;93m"
#define LOG_COLOR_BLUE       "\x1B[0;94m"
#define LOG_COLOR_MAGENTA    "\x1B[4;95m"
#define LOG_COLOR_CYAN       "\x1B[1;96m"

#define LOG_COLOR_DEBUG      LOG_COLOR_GREEN
#define LOG_COLOR_INFO       LOG_COLOR_CYAN
#define LOG_COLOR_WARN       LOG_COLOR_YELLOW
#define LOG_COLOR_ERROR      LOG_COLOR_MAGENTA
#define LOG_COLOR_FATAL      LOG_COLOR_RED

// #define LOG_CTRL_LEVEL_ERROR (1)
// #define LOG_CTRL_LEVEL_WARN  (2)
// #define LOG_CTRL_LEVEL_INFO  (3)
// #define LOG_CTRL_LEVEL_DEBUG (4)    

static const char * const color_info[] =
{
    LOG_COLOR_FATAL,
    LOG_COLOR_ERROR,
    LOG_COLOR_WARN,
    LOG_COLOR_INFO,
    LOG_COLOR_DEBUG
};

logger_t *loggerInit(level_t level, formatter_t *formatter, writer_t *writer, filter_t *filter){
    assert(formatter != NULL);
    assert(writer != NULL);

    static logger_t logger = {
        .level = level,
        .buffer = {0},
        .formatter = formatter,
        .writer = writer,
        .filter = filter
    };
    
    return &logger;
}

void loggerDeInit(logger_t *logger){
    assert(logger);

    memset(&logger->buffer, 0, sizeof(logger->buffer));
    logger->level = LOG_LEVEL_FATAL;
    logger->formatter = NULL;
    logger->writer = NULL;
}

/**
 * @brief   output a log.
 
 * @param   logger is the 
 * @param   
 * @return  
 * @note    
 * @see     
 */
__weak void log(logger_t *logger, const char *tag, level_t level, const char *format, va_list args){
    filter_t *filter;
    assert(logger && log);
    
    if(level > logger->level){
        return;
    }

    //! filter tag.
    assert(logger->filter != NULL);
    filter = logger->filter;
    if(filter->invoke && filter->invoke(filter, tag)){

    }


    //! fomart

    //! write



}







