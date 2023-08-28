/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */

#include "drivers/disk.h"
#include "mm/mm.h"
#include "fs/file.h"
#include "fs/fat16.h"

struct disk*   sd_card;

void disk_init(){

    sd_card->disk_id = 'A';
    sd_card->sector_size  = 512;
    sd_card->fs = fat16_init(); // we only support fat16 sd cards for now.
				
}

struct disk* disk_get(){

    return sd_card;
}

struct disk_stream* disk_stream_new(){
    struct disk_stream* streamer = (struct disk_stream*) malloc(sizeof(struct disk_stream));
    streamer->streamer_pos = 0;

    return streamer;
}

// returns a disk streamer pointing at given position
struct disk_stream* disk_stream_init(struct disk_stream* streamer, uint32_t position){
    streamer->streamer_pos = position;

    return streamer;
}

// given a disk streamer, points it to given location.
void disk_stream_seek(struct disk_stream* streamer, uint32_t position){
    streamer->streamer_pos = position;
}

void disk_stream_close(struct disk_stream* streamer){
    free(streamer);
}

void disk_stream_read(struct disk_stream* streamer, void* read_buffer, uint32_t bytes_to_read){


    // finding which sector the streamer is currently at
    uint32_t sector        = streamer->streamer_pos / sd_card->sector_size;
    uint32_t sector_offset = streamer->streamer_pos % sd_card->sector_size;

    // if we are trying to read out of bounds of current sector
    uint8_t is_overflowing = (sector_offset + bytes_to_read) >= sd_card->sector_size;

    uint8_t temp_buffer[sd_card->sector_size];
    uint32_t temp_bytes_to_read = bytes_to_read;


    // we can only read one sector at a time, so we first read the current sector in which the streamer is at.
    if(is_overflowing){
        bytes_to_read -= (sector_offset + bytes_to_read) - sd_card->sector_size;
    }

    // reading the current sector and adding it to the read buffer provided
    sd_read_sector(sector, 1, temp_buffer);

    for(int index = 0;index < bytes_to_read; index++){
        *(uint8_t*) read_buffer++ = temp_buffer[sector_offset + index];
    }

    streamer->streamer_pos += bytes_to_read;

    // now if we tried to read out of bounds, we recursively read the next sector.
    if(is_overflowing){
        disk_stream_read(streamer, read_buffer, temp_bytes_to_read - bytes_to_read);
    }


}

void sd_read_sector(uint32_t lba, uint32_t sector_count, void* read_buffer){


}

