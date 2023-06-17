#include "qlog_api.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>

#define TAG_NAME "module"

static int count = 0;

void *func(void *args){
    long num = (long)args;
    level_t level = num;
    while(1){
        switch (level) {
            case LOG_LEVEL_ERROR:{
                loge("count is %d\n", count++);
                break;
            }
            case LOG_LEVEL_WARNING:{
                logw("count is %d\n", count++);
                break;
            }
            case LOG_LEVEL_INFO:{
                logi("count is %d\n", count++);
                break;
            }
            case LOG_LEVEL_DEBUG:{
                logd("count is %d\n", count++);
                break;
            }
            default:
                break;
        }
    }
    return NULL;
}


int main(){
    qlog_init(LOG_LEVEL_DEBUG, true, true, 5);
    // qlog_filter("module.err", 4);
    qlog("hello", LOG_LEVEL_INFO, "this is %d\n", sizeof(level_t));

    pthread_t tid[4];
    size_t i;
    for (i = 0; i < sizeof(tid)/sizeof(tid[0]); i++)
    {   
        pthread_create(&tid[i], NULL, func, (void *)(i+1));
    }


    qlog_dbg("module.dbg", "this is module %d\n", 333);
    qlog_err("module.err", "this is module %d\n", 333);
    qlog_dbg("module.err", "this is module %d\n", 333);
    qlog_info("module.info", "this is module %d\n", 333);
    
    logi("hello, it is %d\n", 7);
    loge("hello, it is %f\n", 5.2345);
    logd("hello, it is %s\n", "qlog");

    for (i = 0; i < sizeof(tid)/sizeof(tid[0]); i++) {
        pthread_join(tid[i], NULL);
    } 

    return 0;
}


