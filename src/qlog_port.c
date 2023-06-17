/**
 * @file    qlog_port.c
 * @author  qufeiyan
 * @brief   
 * @version 1.0.0
 * @date    2023/06/11 20:47:44
 * @version Copyright (c) 2023
 */

/* Includes --------------------------------------------------------------------------------*/

#include "qlog.h"
#include "qlog_api.h"
#include "qlog_def.h"
#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <pthread.h>

/**
 * @brief Default output function.
 * @note If user do not overload it, fputs in libc will be the default method. 
 *       And fputs is thread-safed. 
 * @param fmt
 */
__weak void console_puts(const char *str){
    fputs(str, stdout);
}

/**
 * @brief By default, pthread mutex API is used for thread safety.
 * @note  
 */
__weak void *locker_init(void *args){
    static pthread_mutex_t mid;
    pthread_mutex_t *_locker = &mid;
    pthread_mutex_init(_locker, NULL);   
    return _locker; 
}

__weak void locker_lock(void *args){
    pthread_mutex_t *_locker = args;
    pthread_mutex_lock(_locker);
}

__weak void locker_unlock(void *args){
    pthread_mutex_t *_locker = args;
    pthread_mutex_unlock(_locker);
}

__weak void locker_deinit(void *args){
    pthread_mutex_t *_locker = args;
    pthread_mutex_destroy(_locker);
}
