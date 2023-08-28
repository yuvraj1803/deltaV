/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 */

#ifndef __file_h__
#define __file_h__

#include "fs/path.h"
#include <stdint.h>

enum{
    READ    = 'r',
    WRITE   = 'w',
    APPEND  = 'a',
    INVALID
};

enum{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

#define EOF     -1

// structure prototypes
struct disk;
struct file_stat;

typedef int8_t (*fs_resolve) (struct disk* _disk);
typedef  void*  (*fs_open)   (struct disk* _disk, struct path* _path, uint8_t mode);
typedef  int    (*fs_read)   (struct disk* _disk, void* fd_private_data,uint32_t size, uint32_t nmemb, void* dest);
typedef  int    (*fs_seek)   (void* fd_private_data, uint32_t offset, uint8_t whence);
typedef  int    (*fs_close)  (void* fd_private_data);
typedef  int    (*fs_fstat)  (void* fd_private_data, struct file_stat* stat);

struct filesystem{
    char name[10];
	
    fs_resolve resolve;
    fs_open    open;
    fs_read    read;
    fs_seek    seek;
    fs_close   close;
    fs_fstat   stat;

};

enum FSTAT_FLAGS{
    RDONLY,
    WTONLY
};

struct file_descriptor{
    uint32_t index;
    struct filesystem* fs;
    void* fs_file_descriptor; // file descriptor for that specific file system
    struct disk* sd_card;
};

struct file_stat{
    uint32_t size;
    uint8_t  mode;
    uint32_t flags;

};

struct file_descriptor* vfs_get_file_descriptor(uint32_t fd);

int fopen(const char* filename, uint8_t mode);
int fread(uint8_t* dest, uint32_t size, uint32_t nmemb, int fd);
int fseek(int fd, uint32_t offset, uint8_t whence);
int fclose(int fd);
int fstat(int fd, struct file_stat* stat);

#endif

