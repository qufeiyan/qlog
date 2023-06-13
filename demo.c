#include "qlog_api.h"

#define TAG_NAME "module"
#define LOG_D

int main(){
    qlog_init(LOG_LEVEL_DEBUG, true, true, 5);

    qlog("hello", LOG_LEVEL_DEBUG, "this is %d\n", 100);


    qlog_dbg("module.dbg", "this is module %d\n", 333);
    qlog_err("module.err", "this is module %d\n", 333);
    qlog_info("module.info", "this is module %d\n", 333);
    
}

