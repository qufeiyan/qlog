#include "qlog.h"
#include "qlog_api.h"



int main(){
    qlog_init(LOG_LEVEL_DEBUG, true, true, 5);

    qlog("hello", LOG_LEVEL_DEBUG, "this is %d\n", 100);
}

