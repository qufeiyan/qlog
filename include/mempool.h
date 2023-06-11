/**
 * @file    mempool.h
 * @author  qufeiyan
 * @brief   
 * @version 1.0.0
 * @date    2023/06/10 12:34:09
 * @version Copyright (c) 2023
 */

/* Define to prevent recursive inclusion ---------------------------------------------------*/
#ifndef __MEMPOOL_H
#define __MEMPOOL_H
/* Include ---------------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct memoryPool{
    const char* name;
    void *start;
    size_t size;
    size_t block_size;
    size_t block_total;
    size_t block_free;
    uint8_t *block_list;

    void *(*alloc)(struct memoryPool *);
    void (*free)(struct memoryPool *, void *pointer);
};
typedef struct memoryPool memoryPool_t;

void memoryPoolInit(struct memoryPool *mp, const char *name, 
    size_t block_total, size_t block_size, void *start);


#define memoryPoolAlloc(mp) ({\
    void *ptr;\
    typeof((mp)) _mp = (memoryPool_t *)(mp);\
    ptr = _mp->alloc(_mp);\
    ptr;\
})

#define memoryPoolFree(mp) ({\
    typeof((mp)) _mp = (memoryPool_t *)(mp);\
    _mp->free(_mp);\
})


#ifdef __cplusplus
}
#endif

#endif	//  __MEMPOOL_H