/**
 * @file    qlog_slist.h
 * @author  qufeiyan
 * @brief   Define a single list.
 * @version 1.0.0
 * @date    2023/06/07 22:57:01
 * @version Copyright (c) 2023
 */

/* Define to prevent recursive inclusion ---------------------------------------------------*/
#ifndef __QLOG_SLIST_H
#define __QLOG_SLIST_H
/* Include ---------------------------------------------------------------------------------*/
#include "qlog_def.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct slist{
    struct slist *next;
};
typedef struct slist slist_node_t;
typedef struct slist slist_t;

static __inline void slist_init(slist_t *slist){
    slist->next = NULL;
}

static __inline void slist_append(slist_t *head, slist_t *target){
    slist_t *s;
    s = head;
    while(s->next){
        s = s->next;
    }
    s->next = target;
    target->next = NULL;
}

static __inline void slist_insert(slist_t *current, slist_t *target){
    target->next = current->next;
    current->next = target;
}

static __inline slist_t *slist_remove(slist_t *head, slist_t *target){
    slist_t *current;
    current = head;
    while (current && current->next != target) {
        current = current->next;
    }

    current->next = (current->next ? current->next->next : NULL);

    return head;
}

static __inline uint32_t slist_length(const slist_t *head){
    uint32_t res = 0;

    const slist_t *current = head->next;
    while(current){
        current = current->next;
        res++;
    }
    return res;
}

static __inline bool slist_empty(const slist_t *head){
    return head->next == NULL;
}

#define slist_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)



#ifdef __cplusplus
}
#endif

#endif	//  __QLOG_SLIST_H