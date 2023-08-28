/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */

#include "fs/fat16.h"
#include "fs/file.h"
#include "string.h"
#include "drivers/disk.h"
#include "memory.h"
#include "mm/mm.h"
#include "kstatus.h"
#include "config.h"

#define FAT16_SIGNATURE     0x29
#define FAT16_ENTRY_SIZE    0x02
#define FAT16_BAD_SECTOR    0xFF7
#define FAT16_UNUSED        0x00       

#define FAT16_DIRECTORY 0
#define FAT16_FILE 1

#define FAT16_FILE_READ_ONLY 0x01
#define FAT16_FILE_HIDDEN 0x02
#define FAT16_FILE_SYSTEM 0x04
#define FAT16_FILE_VOLUME_LABEL 0x08
#define FAT16_FILE_SUBDIRECTORY 0x10
#define FAT16_FILE_ARCHIVED 0x20
#define FAT16_FILE_DEVICE 0x40
#define FAT16_FILE_RESERVED 0x80

int8_t fat16_resolve(struct disk* _disk);
void* fat16_open(struct disk* _disk, struct path* _path, uint8_t mode);
int fat16_close(void* fd_private_data);
int fat16_stat(void* fd_private_data, struct file_stat* stat);
int fat16_read(struct disk* _disk, void* fd_private_data, uint32_t size, uint32_t nmemb, void* out);
int fat16_seek(void *fd_private_data, uint32_t offset, uint8_t seek_mode);

struct filesystem fat16 = {
    .resolve = fat16_resolve,
    .open    = fat16_open,
    .close   = fat16_close,
    .stat    = fat16_stat,
    .read    = fat16_read,
    .seek    = fat16_seek

};

struct fat16_bpb{ // bios parameter block

    uint8_t     short_jump_instruction_and_nop[3];
    uint8_t     oem_identifier[8];
    uint16_t    bytes_per_sector;
    uint8_t     sectors_per_cluster;
    uint16_t    reserved_sectors;
    uint8_t     fat_copies;
    uint16_t    root_dir_entries;
    uint16_t    number_of_sectors;
    uint8_t     media_type;
    uint16_t    sectors_per_fat;
    uint16_t    sectors_per_track;
    uint16_t    number_of_heads;
    uint32_t    hidden_setors;
    uint32_t    sectors_big;

} __attribute__((packed));

struct fat16_ebpb{ // extended bios parameter block
    uint8_t     drive_number;
    uint8_t     win_nt_bit;
    uint8_t     signature;
    uint32_t    volume_id;
    uint8_t     volume_id_str[11];
    uint8_t     system_id_str[8];
}__attribute__((packed));


struct fat16_header{

    struct fat16_bpb bpb;            // bios parameter block
    union {struct fat16_ebpb ebpb;} extended; // extended bios parameter block

};


struct fat16_file{
    uint8_t     name[8];
    uint8_t     extention[3];
    uint8_t     attributes;
    uint8_t     reserved;
    uint8_t     creation_time_in_tenths_of_a_sec;
    uint16_t    creation_time;
    uint16_t    creation_date;
    uint16_t    last_access;
    uint16_t    high_16_bits_of_first_cluster;
    uint16_t    last_modification_time;
    uint16_t    last_modification_date;
    uint16_t    low_16_bits_of_first_cluster;
    uint32_t    size;
}__attribute__((packed));

struct fat16_directory{
    struct fat16_file*  files;
    uint32_t            total_files;
    uint32_t            first_sector;
    uint32_t            last_sector;
};

struct fat16_unit{
    union{ // could either be a directory or file
        struct fat16_directory* directory;
        struct fat16_file*      file;
    };

    uint8_t fat16_unit_type; // directory/file
};

struct fat16_file_descriptor{ 
    struct fat16_unit* unit; // can be a directory or a file.
    uint32_t           file_position; // position inside the file at given time
};

struct fat16_private_data{
    struct fat16_header         header;
    struct fat16_directory      root; // root directory 

    struct disk_stream*         cluster_streamer;
    struct disk_stream*         fat_streamer;
    struct disk_stream*         directory_streamer;
};

struct filesystem* fat16_init(){

    strcpy(fat16.name, "fat16");    

    return &fat16;

}

// returns number of files in given directory.
static uint32_t fat16_total_files_in_directory(struct disk* _disk, struct fat16_private_data* fat16_private, struct fat16_directory* dir){

    // point streamer to the start of directory.
    disk_stream_seek(fat16_private->directory_streamer, dir->first_sector * _disk->sector_size);

    uint32_t total_files = 0;

    struct fat16_file temp_file;
    memset(&temp_file, 0, sizeof(temp_file));

    while(1){
        disk_stream_read(fat16_private->directory_streamer, &temp_file, sizeof(temp_file));

        if(temp_file.name[0] == 0x00) break; // end of entries.

        if(temp_file.name[0] == 0xE5) continue;

        total_files++;

    }


    return total_files;

}

static int8_t fat16_get_root(struct disk* _disk, struct fat16_private_data* fat16_private, struct fat16_directory* root){

    uint32_t fat16_root_sector           = (fat16_private->header.bpb.fat_copies * fat16_private->header.bpb.sectors_per_fat) + fat16_private->header.bpb.reserved_sectors;
    uint32_t fat16_root_entries          = fat16_private->header.bpb.root_dir_entries;
    uint32_t fat16_root_size             = fat16_root_entries * sizeof(struct fat16_file);
    uint32_t fat16_root_sectors_occupied = fat16_root_size / _disk->sector_size + (fat16_root_size % _disk->sector_size != 0);

    // first file of the root directory
    struct fat16_file* root_file = (struct fat16_file*) malloc(fat16_root_size);
    struct disk_stream* root_streamer = disk_stream_new();

    if(root_file == 0x0){
        // couldn't create memory for root directory.
        return -ERR_MALLOC_FAIL;
    }

    // read the root directory into our structure.
    disk_stream_seek(root_streamer, fat16_root_sector * _disk->sector_size);
    disk_stream_read(root_streamer, root_file, fat16_root_size);

    root->files        = root_file;
    root->first_sector = fat16_root_sector;
    root->last_sector  = root->first_sector + fat16_root_sectors_occupied - 1;
    root->total_files  = fat16_total_files_in_directory(_disk, fat16_private, root); 

    disk_stream_close(root_streamer);


    return SUCCESS;

}

static void fat16_destroy_directory(struct fat16_directory* dir){
    if(!dir) return;

    if(dir->files){
        free(dir->files);
    }

    free(dir);
}

static void fat16_destroy_unit(struct fat16_unit* fu){

    if(fu->fat16_unit_type == FAT16_DIRECTORY){
        fat16_destroy_directory(fu->directory);
    }else{
        free(fu->file);
    }

    free(fu);
}

static void fat16_destroy_fd(struct fat16_file_descriptor* fd){
    fat16_destroy_unit(fd->unit);
    free(fd);
}

static uint16_t fat16_get_entry(struct disk* _disk, uint32_t cluster){
    struct fat16_private_data* private_data = _disk->fs_private_data;

    uint32_t fat16_table_position = private_data->header.bpb.reserved_sectors * _disk->sector_size;

    struct disk_stream* streamer = private_data->fat_streamer;

    // we point at the entry of the required cluster in the fat table
    disk_stream_seek(streamer, fat16_table_position * cluster * FAT16_ENTRY_SIZE);

    // we now write whatever is in that entry into this variable below. remember, fat16 enties are 2 bytes long.
    uint16_t fat16_entry = 0;
    disk_stream_read(streamer, &fat16_entry, sizeof(fat16_entry));


    return fat16_entry;

}

static int fat16_get_cluster_from_offset(struct disk* _disk, uint32_t cluster, uint32_t offset){
    // returns effective cluster given starting cluster and the offset.
    struct fat16_private_data* private_data = _disk->fs_private_data;
    uint32_t cluster_size_in_bytes = private_data->header.bpb.sectors_per_cluster * _disk->sector_size;
    uint32_t current_cluster = cluster;
    uint32_t additional_clusters = offset / cluster_size_in_bytes; // if the offset overflows the cluster size, we need to check the clusters ahead as well;

    for(uint32_t cluster_index = 0;cluster_index < additional_clusters; cluster_index++){
        uint32_t fat16_entry = fat16_get_entry(_disk, current_cluster);

        if(fat16_entry == 0xFF8 || fat16_entry == 0xFFF){
            // we have reached the end of the file.
            return -ERR_INVARG;
        }

        // check if the sector is bad.
        if(fat16_entry == FAT16_BAD_SECTOR){
            return -ERR_INVARG;
        }

        // check if sector is reserved
        if(fat16_entry == 0xFF0 || fat16_entry == 0xFF6){
            return -ERR_INVARG;
        }

        if(fat16_entry == 0x00){
            // corrupt fat table

            return -ERR_INVARG;
        }

        current_cluster = fat16_entry;

    }

    return current_cluster;
}

static uint32_t fat16_cluster_to_sector(struct fat16_private_data* private, uint32_t cluster){
    return private->root.last_sector + (cluster - 2) * private->header.bpb.sectors_per_cluster + 1;
}

static int fat16_read_from_disk(struct disk* _disk, uint32_t first_cluster, uint32_t offset, uint32_t size, void* out){

    struct fat16_private_data* f16pd = _disk->fs_private_data;
    struct disk_stream* streamer = f16pd->cluster_streamer;

    uint32_t total_cluster_size = f16pd->header.bpb.sectors_per_cluster * _disk->sector_size;
    uint32_t current_cluster = fat16_get_cluster_from_offset(_disk, first_cluster, offset);

    if(current_cluster < 0){
        return -ERR_INVARG;
    }    

    uint32_t first_sector = fat16_cluster_to_sector(f16pd, current_cluster);
    uint32_t position_in_sector = first_sector * _disk->sector_size + offset;

    // we can only read one cluster at a time
    uint32_t size_readable = size > total_cluster_size ? total_cluster_size : size;

    disk_stream_seek(streamer, position_in_sector);
    disk_stream_read(streamer, out, size_readable);
    size -= size_readable;

    if(size > 0){
        fat16_read_from_disk(_disk, first_cluster, offset + size_readable, size, out + size_readable);
    }

    return SUCCESS;

}

static void fat16_to_proper_string(char ** out, const char* in, uint32_t size){

    int counter = 0;

    while(*in != 0x00 && *in != 0x20){
        **out = *in;
        *out+=1;
        in+=1;
        counter++;

        if(counter >= size){
            **out = 0x00;
            return;
        }

    }

    if(*in == 0x20){
        **out = 0x00;
    }

}

static void fat16_get_full_relative_name(struct fat16_file* file, char* out, uint32_t length){
    memset(out, 0, length);
    char* temp_out = out;

    fat16_to_proper_string(&temp_out, (const char*) file->name, sizeof(file->name));

    if(file->extention[0] != 0x00 && file->extention[0] != 0x20){
        *temp_out++ = '.';
        fat16_to_proper_string(&temp_out, (const char*) file->extention, sizeof(file->extention));
    }
}

static struct fat16_directory* fat16_load_directory(struct disk* _disk, struct fat16_file* file){

    struct fat16_directory* dir = malloc(sizeof(struct fat16_directory));
    struct fat16_private_data* private = _disk->fs_private_data;



    if(!dir){
        return 0;
    }

    if(!(file->attributes & FAT16_FILE_SUBDIRECTORY)){
        free(dir);
        return -0;
    }

    // first cluster and sectors of the directory entries.
    uint32_t first_cluster = file->low_16_bits_of_first_cluster | file->high_16_bits_of_first_cluster;
    uint32_t first_sector  = private->root.last_sector + (first_cluster - 2) * private->header.bpb.sectors_per_cluster +1;
    
    dir->first_sector = first_sector;

    uint32_t total_files = fat16_total_files_in_directory(_disk, private, dir);
    dir->total_files = total_files;

    uint32_t directory_size = total_files * sizeof(struct fat16_file);
    dir->files = malloc(directory_size);

    if(!dir->files){
        return 0;
    }

    if(fat16_read_from_disk(_disk, first_cluster, 0, directory_size, dir->files) < 0){
        free(dir->files);
        return 0;
    }

    return dir;


}

static struct fat16_file* fat16_clone_file(struct fat16_file* file, uint32_t size){
    struct fat16_file* file_copy = malloc(sizeof(struct fat16_file));


    if(!file_copy){
        return 0;
    }


    if(size < sizeof(struct fat16_file)){
        free(file_copy);
        return 0;
    }

    memcpy(file_copy, file, size);
    return file_copy;
}

static struct fat16_unit* fat16_new_fat_unit_for_directory_unit(struct disk* _disk, struct fat16_file* file){
    struct fat16_unit* unit = malloc(sizeof(struct fat16_unit));

    if(!unit){
        return 0;
    }

    // if the item passed is a directory
    if(file->attributes & FAT16_FILE_SUBDIRECTORY){
        unit->directory = fat16_load_directory(_disk, file);
        unit->fat16_unit_type = FAT16_DIRECTORY;
        return unit;
    }

    // if the item passed is a file

    unit->file = fat16_clone_file(file, sizeof(struct fat16_file));
    unit->fat16_unit_type = FAT16_FILE;

    return unit;

}

static struct fat16_unit* fat16_find_unit_in_directory(struct disk*_disk, struct fat16_directory* dir, const char* unit_str){
    struct fat16_unit* unit = 0;
    char temp_name[FS_MAX_PATH_UNIT_LENGTH];
    memset(temp_name, 0, sizeof(temp_name));


    for(int dir_unit =0 ;dir_unit < dir->total_files;dir_unit++){
            fat16_get_full_relative_name(&dir->files[dir_unit], temp_name, sizeof(temp_name));


            if(strncmp((const char*) temp_name, (const char*) unit_str, sizeof(temp_name)) == 0){ // if the strings match
                unit = fat16_new_fat_unit_for_directory_unit(_disk, &dir->files[dir_unit]);
            }

    }

    return unit;

}

static struct fat16_unit* fat16_get_unit(struct disk* _disk, struct path_unit* _path){
    struct fat16_private_data* private = _disk->fs_private_data;
    struct fat16_unit* root = fat16_find_unit_in_directory(_disk, &private->root, _path->unit_str);
    
    struct fat16_unit* current_unit = root;

    if(!root){
        return 0;
    }

    struct path_unit* next_path_unit = _path->next_unit;

    while(next_path_unit != 0){
        if(current_unit->fat16_unit_type == FAT16_DIRECTORY){
            struct fat16_unit* temp_unit = fat16_find_unit_in_directory(_disk, current_unit->directory, next_path_unit->unit_str);
            current_unit = temp_unit;
            next_path_unit = next_path_unit->next_unit;
        }else{
            current_unit = 0;
            break;
        }
    }

    return current_unit;


}

int8_t fat16_resolve(struct disk* _disk){

    struct fat16_private_data* fat16_private = (struct fat16_private_data*) malloc(sizeof(struct fat16_private_data));


    // initialising private data structure
    fat16_private->cluster_streamer     = disk_stream_new();
    fat16_private->directory_streamer   = disk_stream_new();
    fat16_private->fat_streamer         = disk_stream_new();

    // general purpose streamer 
    struct disk_stream* fat16_streamer  = disk_stream_new();

    // reading the fs header in the disk into private data structure.
    disk_stream_read(fat16_streamer, &fat16_private->header, sizeof(fat16_private->header));

    if(fat16_private->header.extended.ebpb.signature != 0x29){

        disk_stream_close(fat16_streamer);
        free(fat16_private);
        
        _disk->fs_private_data = 0;

        return -ERR_FS_INV_SIGN; // invalid signature
    }

    // link filesystem private data to the disk
    _disk->fs_private_data = fat16_private;

    if(fat16_get_root(_disk, fat16_private, &fat16_private->root) != SUCCESS){

        disk_stream_close(fat16_streamer);
        free(fat16_private);

        _disk->fs_private_data = 0;

        return -ERR_FS_ROOT_NOT_FOUND; // couldn't locate root directory on disk
    }


    disk_stream_close(fat16_streamer);


    return SUCCESS; // filesystem resolved


}

void* fat16_open(struct disk* _disk, struct path* _path, uint8_t mode){
    if(mode != READ){
            return 0;
    }

    struct fat16_file_descriptor* fd = malloc(sizeof(struct fat16_file_descriptor));

    fd->file_position = 0;
    fd->unit = fat16_get_unit(_disk, _path->units);

    if(!fd->unit){
        free(fd);
        return 0;
    }

    return fd;
}

int fat16_read(struct disk* _disk, void* fd_private_data, uint32_t size, uint32_t nmemb, void* out){

    struct fat16_file_descriptor* fd = (struct fat16_file_descriptor*) (fd_private_data);
    
    if(fd->unit->fat16_unit_type == FAT16_DIRECTORY){
        return -ERR_INVARG; // cant fread a directory :(
    }

    
    int members_read_successfully = 0;

    uint32_t offset = fd->file_position;

    for(int member=0;member < nmemb;member++){

        if(fat16_read_from_disk(_disk, fd->unit->file->low_16_bits_of_first_cluster | fd->unit->file->high_16_bits_of_first_cluster, offset,size,out) < 0){
            break;
        }
        members_read_successfully++;

        out += size;
        offset += size;

    }

    return members_read_successfully;


}


int fat16_close(void* fd_private_data){

    fat16_destroy_fd((struct fat16_file_descriptor*) (fd_private_data));

    return SUCCESS;

}

int fat16_stat(void* fd_private_data, struct file_stat* stat){
    
    struct fat16_file_descriptor* _fd = (struct fat16_file_descriptor*) (fd_private_data);
    struct fat16_unit*            _fu = _fd->unit;

    if(_fu->fat16_unit_type == FAT16_DIRECTORY){
        return -ERR_INVARG; // cannot stat a directory
    }

    struct fat16_file* _file = _fu->file;

    stat->size = _file->size;

    stat->flags = 0;
    stat->flags |= _file->attributes;

    return SUCCESS;

}

int fat16_seek(void *fd_private_data, uint32_t offset, uint8_t seek_mode){

    struct fat16_file_descriptor* fd = (struct fat16_file_descriptor*) fd_private_data;

    if(fd->unit->fat16_unit_type == FAT16_DIRECTORY){
        return -ERR_INVARG;
    }

    // if going out of the file.
    if(fd->unit->file->size <= offset){
        return -ERR_INVARG;
    }

    int rval = 0; // return val

    switch(seek_mode)
    {
        case SEEK_SET:
            fd->file_position = offset;
            break;
        case SEEK_CUR:
            fd->file_position += offset;
            break;
        case SEEK_END:
            rval = EOF;
            break;
        default:
            rval = -ERR_INVARG;
            break;

    }

    return rval;


}
