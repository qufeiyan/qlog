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
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

//！ color info.
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


#define SIZE_OF_NAME            (16)
#define SIZE_OF_PREFIX          (128)

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
    ((typeof(type) *)((char *)(ptr) - (unsigned long)(&((typeof(type) *)0)->member)))
#endif


#undef __inline
#define __inline inline __attribute__((always_inline))

#undef __weak
#define __weak __attribute__((weak))


#undef MEMORY_ALIGN_UP
#define MEMORY_ALIGN_UP(addr, size) ({\
    (((addr) + (size) - 1) & ~((size) - 1));\
})
#undef MEMORY_ALIGN_DOWN 
#define MEMORY_ALIGN_DOWN(addr, size) ({\
    ((addr)) & ~((size) - 1);\
})

#undef MEMORY_ALIGN
#define MEMORY_ALIGN (sizeof(size_t))

#if IS_USE_ASSERT
#undef assert
#define assert(expression)                            \
do{                                                   \
    if(!(expression)){                                \
        fprintf(stderr, "%s:%d [%s]" #expression      \
            " is expected...\n", __FILE__, __LINE__, __FUNCTION__);     \
        abort();                                      \
    }                                                 \
}while(0)
#else
#undef assert
#define assert(expression)  (void)(expression)
#endif


#ifdef __cplusplus
}
#endif

#endif	//  QLOG_DEF_H