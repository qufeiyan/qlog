/**
 * @file    qlog_fileWriter.h
 * @author  qufeiyan
 * @brief   
 * @version 1.0.0
 * @date    2023/06/18 19:07:35
 * @version Copyright (c) 2023
 */

/* Define to prevent recursive inclusion ---------------------------------------------------*/
#ifndef __QLOG_FILEWRITER_H
#define __QLOG_FILEWRITER_H
/* Include ---------------------------------------------------------------------------------*/
#include "qlog.h"
#include "qlog_port.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fileWriter{
    writer_t super;
    
    char filePath[SIZE_OF_FILE_PATH];
    char directory[SIZE_OF_FILE_PATH];
    int numberOfFiles;
    int sizeOfFile; 

    FILE *file;                     //! current log file pointer.  
    int positionToWrite;            //! current position of file to write.
    int currentNumberOfFiles;      

    char fileBuffer[SIZE_OF_FILE_BUFFER];
    char *ptrBufferCurrent;

    void (*fileRotate)(struct fileWriter *);
    // locker_t *locker;
};
typedef struct fileWriter fileWriter_t;

#ifdef __cplusplus
}
#endif

#endif	//  __QLOG_FILEWRITER_H