/**
 * @file    mempool.c
 * @author  qufeiyan
 * @brief   Simple implementation of a memory pool.
 * @version 1.0.0
 * @date    2023/06/10 11:15:56
 * @version Copyright (c) 2023
 */

/* Includes --------------------------------------------------------------------------------*/
#include "qlog_def.h"
#include "mempool.h"
#include <string.h>


#define block_list_next(list) (*(uint8_t **)(list))


static void *alloc(struct memoryPool *mp){
    uint8_t *block_current;
    assert(mp != NULL && mp->start != NULL);
    assert(mp->block_list != NULL);
    assert(mp->block_free <= mp->block_total);

    block_current = mp->block_list;
    *(uint8_t **)block_current = (uint8_t *)mp;
    mp->block_list = block_list_next(mp->block_list);
    mp->block_free--;

    return block_current + sizeof(uint8_t *);
}

static void mfree(struct memoryPool *mp, void *pointer){
    uint8_t *block_current;
    assert(mp != NULL && pointer != NULL);    
    block_current = (uint8_t *)pointer - sizeof(uint8_t *);
    assert(*(uint8_t **)block_current == (uint8_t *)mp);

    block_list_next(block_current) = mp->block_list;
    mp->block_list = block_current;
    mp->block_free++;
}

void memoryPoolInit(struct memoryPool *mp, const char *name, size_t block_total, size_t block_size, void *start){
    int32_t index;
    uint8_t *block_current;
    assert(mp != NULL && start != NULL);
    assert(block_total > 0 && block_size > 0);

    mp->name = name;
    mp->block_size = MEMORY_ALIGN_UP(block_size, MEMORY_ALIGN);
    mp->block_total = mp->block_free = block_total;
    mp->start = start;
    
    size_t block_size_real = (sizeof(uint8_t *) + mp->block_size); 
    mp->size = block_size_real * mp->block_total;
    mp->block_list = (uint8_t *)mp->start;

    //! initialise the block list.
    block_current = mp->block_list;
    for (index = 0; index < mp->block_total; ++index) {
        *(uint8_t **)(block_current + index * block_size_real) = \
            (block_current + (index + 1) * block_size_real);        
    }
    *(uint8_t **)(block_current + (index - 1) * block_size_real) = NULL;

    mp->alloc = alloc;
    mp->free = mfree;
}
