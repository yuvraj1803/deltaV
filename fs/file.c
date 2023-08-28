/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */
#include "fs/file.h"
#include "config.h"
#include "memory.h"
#include "drivers/disk.h"
#include "mm/mm.h"
#include "kstatus.h"
#include "fs/parser.h"

struct file_descriptor* fd_list[FS_TOTAL_FILE_DESCRIPTORS];

void file_init(){

    memset(fd_list, 0, sizeof(fd_list));

}

struct file_descriptor* vfs_get_file_descriptor(uint32_t fd){
    if(fd >= 0 && fd < FS_TOTAL_FILE_DESCRIPTORS){
        return fd_list[fd];
    }

    return 0; //invalid fd asked.
}

struct file_descriptor* vfs_new_file_descriptor(){
    for(int index=0;index < FS_TOTAL_FILE_DESCRIPTORS;index++){
        if(fd_list[index] == 0){

            struct file_descriptor* new_fd = (struct file_descriptor*) malloc(sizeof(struct file_descriptor));
            new_fd->index = index;

            fd_list[index] = new_fd;

            return fd_list[index];
        }
    }

    return 0;
}

int fopen(const char* filename, uint8_t mode){
    if(mode == INVALID) return -ERR_FS_INV_FILE_MODE;

    struct path* file_path = parse_path(filename);

    struct disk* _disk = disk_get();

    void* file_data = _disk->fs->open(_disk, file_path, mode);

    if(file_data == 0){
        return -ERR_FS_FOPEN_FAILED;
    }

    struct file_descriptor* fd = vfs_new_file_descriptor();
    if(fd == 0){
        // couldn't find file descriptor
        return -ERR_FS_FOPEN_FAILED;
    }
    fd->sd_card = _disk;
    fd->fs   = _disk->fs;
    fd->fs_file_descriptor = file_data;

    return fd->index;
}

int fread(uint8_t* dest, uint32_t size, uint32_t nmemb, int fd){

    if(fd < 0 || fd >= FS_TOTAL_FILE_DESCRIPTORS){
        return -ERR_INVARG;
    }

    struct file_descriptor* _fd = vfs_get_file_descriptor(fd);

    if(_fd == 0){
        return -ERR_FD_NOT_FOUND;
    }


    _fd->fs->read(_fd->sd_card,_fd->fs_file_descriptor,size, nmemb, dest);

    return SUCCESS;

}

int fseek(int fd, uint32_t offset, uint8_t whence){
    struct file_descriptor* _fd = vfs_get_file_descriptor(fd);

    if(_fd == 0){
        return -ERR_FD_NOT_FOUND;
    }

    _fd->fs->seek(_fd->fs_file_descriptor, offset, whence);


    return SUCCESS;
}

int fclose(int fd){

    struct file_descriptor* _fd = vfs_get_file_descriptor(fd);

    if(_fd == 0){
        return -ERR_FD_NOT_FOUND;
    }

    fd_list[_fd->index] = 0;
    _fd->fs->close(_fd->fs_file_descriptor);


    return SUCCESS;

}

int fstat(int fd, struct file_stat* stat){
    struct file_descriptor* _fd = vfs_get_file_descriptor(fd);

    if(_fd == 0){
        return -ERR_FD_NOT_FOUND;
    }

    _fd->fs->stat(_fd->fs_file_descriptor, stat);

    return SUCCESS;

}

