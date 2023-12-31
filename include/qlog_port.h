/**
 * @file    qlog_port.h
 * @author  qufeiyan
 * @brief   
 * @version 1.0.0
 * @date    2023/06/11 19:50:01
 * @version Copyright (c) 2023
 */

/* Define to prevent recursive inclusion ---------------------------------------------------*/
#ifndef __QLOG_PORT_H
#define __QLOG_PORT_H
/* Include ---------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


#define COUNT_OF_TAG            (32)    //! maximum number of the filter tag.

#define SIZE_OF_LOG_BUFFER      (512)   //！ size of the log buffer.

#define SIZE_OF_FILE_PATH       (64)    //! maximum size of the file name.

// #define SIZE_OF_FILE_BUFFER     (4096)  //! maximum size of the file buffer.

#define SIZE_OF_FILE_BUFFER     (512)

/**
 * @brief   customed console output api.
 * @param   str is the string to output to console. 
 * @note    
 * @see     
 */
void console_puts(const char *str);

void *locker_init(void *args);
void locker_lock(void *args);
void locker_unlock(void *args);
void locker_deinit(void *args);

#ifdef __cplusplus
}
#endif

#endif	//  __QLOG_PORT_H