/**
 * @file    qlog_writer.c
 * @author  qufeiyan
 * @brief   Define writer for qlog.
 * @version 1.0.0
 * @date    2023/06/07 22:36:58
 * @version Copyright (c) 2023
 */

/* Includes --------------------------------------------------------------------------------*/
#include "qlog.h"
#include "qlog_def.h"
#include "qlog_slist.h"
#include <assert.h>
#include <string.h>


void _fileWriter_write(writer_t *writer){

}

void _fileWriter_init(writer_t *writer, const char *filename){
    assert(writer);
    assert(filename);

    

}

writer_t *fileWriter(writer_t *writer, const char *name, char *buffer){
    assert(writer);
    strcpy(writer->name, name);
    writer->buffer = buffer;
    
    
    
    return writer;
}





