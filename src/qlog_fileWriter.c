/**
 * @file    qlog_fileWriter.c
 * @author  qufeiyan
 * @brief   Define a file writer for qlog.
 * @version 1.0.0
 * @date    2023/06/07 22:36:58
 * @version Copyright (c) 2023
 */

/* Includes --------------------------------------------------------------------------------*/
#include "qlog_fileWriter.h"
#include "qlog.h"
#include "qlog_def.h"
#include "qlog_slist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>  
#include <errno.h>


// struct fileWriter{
//     writer_t super;
    
//     char filePath[SIZE_OF_FILE_PATH];
//     char directory[SIZE_OF_FILE_PATH];
//     int numberOfFiles;
//     int sizeOfFile; 

//     FILE *file;                     //! current log file pointer.  
//     int positionToWrite;            //! current position of file to write.
//     int currentNumberOfFiles;      

//     char fileBuffer[SIZE_OF_FILE_BUFFER];
//     char *ptrBufferCurrent;

//     void (*fileRotate)(struct fileWriter *);
//     // locker_t *locker;
// };
// typedef struct fileWriter fileWriter_t;

/**
 * @brief   rotate all the log files.
 * @param   fileWriter is pointer to file writer.
 * @note    
 * @see     {@code _fileWriter_flush}
 */
void _fileWriter_rotate(fileWriter_t *fileWriter){
    static char oldFileName[SIZE_OF_FILE_PATH];
    static char newFileName[SIZE_OF_FILE_PATH]; 
    int length;
    assert(fileWriter != NULL);

    length = snprintf(oldFileName, sizeof(oldFileName),  \
        "%s.%-4d", fileWriter->filePath, fileWriter->numberOfFiles - 1);
    assert(length <= sizeof(oldFileName));

    //! remove whitespace from the end of string {@code oldFileName}.
    for(int i = 3; i < 0; --i){
        if(oldFileName[length - i] == ' '){
            oldFileName[length - i] = '\0';
            break;
        }
    }
    //！delete the oldest file.
    if(access(oldFileName, F_OK) == 0){
        remove(oldFileName);
    }

    length -= 5;
    strncpy(newFileName, fileWriter->filePath, length);

    //! $(logfile).log.$(n - 1) --> $(logfile).log.$(n)
    for (int i = fileWriter->numberOfFiles - 2; i >= 0; --i) {
        snprintf(oldFileName + length, sizeof(oldFileName), ".%d", i);
        snprintf(newFileName + length, sizeof(oldFileName), ".%d", i + 1);

        rename(oldFileName, newFileName);
    }

    //! $(logfile).log --> $(logfile).log.0
    if(fileWriter->file != NULL){
        fclose(fileWriter->file);
        fileWriter->file = NULL; //! a new file will open when flush log buffer.
    }
    rename(fileWriter->filePath, oldFileName);
}

/**
 * @brief   write log string to log file buffer.
 * @param   writer is pointer to file writer.
 * @note    
 * @see     {@code _fileWriter_flush}
 */
void _fileWriter_write(writer_t *writer){
    fileWriter_t *fileWriter;
    int length;
    int lengthToWrite, freeToWrite;
    char *ptrBufferEnd;
    char *logString;
    assert(writer != NULL);
    assert(writer->flush != NULL);
    fileWriter = (fileWriter_t *)writer; 

    if(writer->enable == false) return;
    
    lengthToWrite = freeToWrite = 0;
    length = writer->length;
    ptrBufferEnd = fileWriter->fileBuffer + sizeof(fileWriter->fileBuffer);
    logString = writer->buffer;

    //! filter the color info for file writer.
    if(writer->color){
        int colorStartLength = sizeof(LOG_COLOR_START) - 1; 
        logString += colorStartLength;
        logString += sizeof(LOG_COLOR_INFO) - 1;
        length -= colorStartLength;
        length -= sizeof(LOG_COLOR_INFO) - 1; 
        length -= sizeof(LOG_COLOR_END) - 1;
    }

    while(length){
        freeToWrite = ptrBufferEnd - fileWriter->ptrBufferCurrent;
        if(length >= freeToWrite){
            lengthToWrite = freeToWrite; 
        }else{
            lengthToWrite = length;
        }

        //! write log string to the file buffer.
        memcpy(fileWriter->ptrBufferCurrent, logString, lengthToWrite);
        length -= lengthToWrite;
        fileWriter->ptrBufferCurrent += lengthToWrite;
        logString += lengthToWrite;

        assert(fileWriter->ptrBufferCurrent <= ptrBufferEnd);
        if(ptrBufferEnd == fileWriter->ptrBufferCurrent){
            //!flush the file.
            writer->flush(writer);
            fileWriter->ptrBufferCurrent = fileWriter->fileBuffer;
        }
    }

    //! call another writer.
    writer_t *nextWriter = writer->next;
    if(nextWriter){
        nextWriter->length = writer->length;
        nextWriter->write(nextWriter);
    }
}

/**
 * @brief   flush a file writer.
 * @param   writer is pointer to file writer. 
 * @note    writes log buffer to specific log file when this method is called.
 * @see     
 */
void _fileWriter_flush(writer_t *writer){
    fileWriter_t *fileWriter;
    int sizeToWrite;
    assert(writer != NULL);
    fileWriter = (fileWriter_t *)writer; 
    assert(fileWriter->ptrBufferCurrent > fileWriter->fileBuffer);

    sizeToWrite = fileWriter->ptrBufferCurrent - fileWriter->fileBuffer;
    if(fileWriter->positionToWrite + sizeToWrite >= fileWriter->sizeOfFile){
        fileWriter->fileRotate(fileWriter);
    }

    if(fileWriter->file == NULL){
        char *path = fileWriter->filePath;
        fileWriter->file = fopen(path, "w+");
        assert(fileWriter->file != NULL);
        fileWriter->positionToWrite = 0;
    }

    fwrite(fileWriter->fileBuffer, sizeToWrite, 1, fileWriter->file);
    fflush(fileWriter->file);
    fileWriter->positionToWrite += sizeToWrite;
}

/**
 * @brief   initialise a file writer.
 * @param   writer is pointer to file writer.
 * @param   buffer is pointer to log string.
 * @param   fileName is the name of log file.
 * @param   directory is the directory of log file.
 * @param   numberOfFiles is the number of log files.
 * @param   sizeOfFile is the size of a single log file.
 */
void fileWriterInit(writer_t *writer, char *buffer, const char *fileName, const char *directory, 
                      int numberOfFiles, int sizeOfFile){
    fileWriter_t *fileWriter;
    bool appendSuffix;
    assert(writer && buffer);
    assert(fileName && directory);

    fileWriter = (fileWriter_t *)writer;
    memset(fileWriter, 0, sizeof(*fileWriter));

    strcpy(writer->name, fileName);
    strcpy(fileWriter->directory, directory);
    strcpy(fileWriter->filePath, fileWriter->directory);
    strcat(fileWriter->filePath, "/");
    strcat(fileWriter->filePath, writer->name);

    appendSuffix = true;
    int length = strlen(fileName);
    if(length >= 4){
        const char *suffix = fileName + length - 4;
        if(strcmp(suffix, ".log") == 0){
            appendSuffix = false;
        }
    }
    //! append suffix for log file.
    if(appendSuffix){
        strcat(fileWriter->filePath, ".log");
    }

    //! create a directory if it does not exist.
    if(access(fileWriter->directory, F_OK) < 0){
        if (mkdir(fileWriter->directory, (S_IRWXU | S_IRWXG | S_IRWXO) < 0)){
            fprintf(stderr, "failed to create %s: %s\n", 
                fileWriter->directory, strerror(errno));
        }
    }

    if(access(fileWriter->filePath, F_OK) == 0){
        fprintf(stderr, "[warning]: %s exist!!!\n", fileWriter->filePath);
        remove(fileWriter->filePath);
    } 
    
    fileWriter->numberOfFiles = numberOfFiles;
    fileWriter->sizeOfFile = sizeOfFile;
    fileWriter->ptrBufferCurrent = fileWriter->fileBuffer;
    fileWriter->file = NULL;
    fileWriter->positionToWrite = 0;
    fileWriter->fileRotate = _fileWriter_rotate;

    strcpy(writer->name, "file");
    writer->buffer = buffer;
    writer->write = _fileWriter_write;
    writer->flush = _fileWriter_flush;
    writer->next = NULL;
    writer->enable = false;
}







