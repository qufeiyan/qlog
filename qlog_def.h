/**
 * @file    qlog_def.h
 * @author  qufeiyan
 * @brief   Define all macro to used.
 * @version 1.0.0
 * @date    2023/06/07 22:06:22
 * @version Copyright (c) 2023
 */

/* Define to prevent recursive inclusion ---------------------------------------------------*/
#ifndef __QLOG_DEF_H
#define __QLOG_DEF_H
/* Include ---------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE_OF_NAME            (16)
#define SIZE_OF_PREFIX          (128)
#define SIZE_OF_LOG_BUFFER      (512)

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({\
    ((typeof(type) *)((char *)(ptr) - (unsigned long)(&((typeof(type) *)0)->member)))\
})


#undef __inline
#define __inline __attribute__((always_inline))

#undef __weak
#define __weak __attribute__((weak))


enum level{
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_DEBUG
};

typedef enum level level_t;


#ifdef __cplusplus
}
#endif

#endif	//  QLOG_DEF_H