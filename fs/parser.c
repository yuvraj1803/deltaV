/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */

#include "fs/parser.h"
#include "fs/path.h"
#include "kstatus.h"
#include "mm/mm.h"
#include "string.h"
#include "memory.h"
#include "config.h"
#include "stdio.h"


char parse_drive(char* __path){
    if(__path[0] >= 'A' && __path[0] <= 'Z' && strlen(__path) >= 3){
        return __path[0];
    }

    return -ERR_INVPATH;
}

char* path_get_current_unit_and_advance(char** __path){

    char* current_unit_str = (char*) malloc(sizeof(char) * FS_MAX_PATH_UNIT_LENGTH);
    uint32_t current_unit_str_ptr = 0;

    // handling empty path string
    if(strlen(*__path) == 0){
        return 0x00;
    }

    char* temp_path = (char*) malloc(sizeof(char) * strlen(*__path));

    strcpy(temp_path, *__path);

    while(*temp_path != '/' && *temp_path != 0x00){
        current_unit_str[current_unit_str_ptr++] = *temp_path;
        temp_path++;
    }

    if(strlen(current_unit_str) > FS_MAX_PATH_UNIT_LENGTH){
       	panic("[ERROR]: Provided invalid path. Unable to parse!");
    }

    // we need to advance to the start of the next unit. we are currently pointing to '/'
    if(*temp_path == '/') temp_path++;

    strcpy(*__path, temp_path);

    free(temp_path);

    return current_unit_str;

}

struct path* parse_path(const char* _path){

    // provided path is longer than allowed path length
    if(strlen(_path) > FS_MAX_PATH_LENGTH){
        panic("[ERROR]: Provided invalid path. Unable to parse!");
    }


    char drive;
    struct path* parsed_path = (struct path*) malloc(sizeof(struct path));
    char* __path = (char*) malloc(sizeof(char)* strlen(_path));

    strcpy(__path, _path);

    if((drive = parse_drive(__path)) < 0){
        return 0x00; // returning 0x00 means, couldn't parse the path provided.
    }
    __path += 3; // moving pointer from drive to next unit. because path goes like, C:/unit1...


    // we go thorough all units of the path and link them one by one like we do in a linked list.
    // if path => C:/unit1/unit2/unit3 , the list would look like, 
                   // (unit1) -> (unit2) -> (unit3)...


    // head node of list of units.
    struct path_unit* unit_head = (struct path_unit*) malloc(sizeof(struct path_unit));
    unit_head->unit_str = path_get_current_unit_and_advance(&__path);
    unit_head->next_unit = 0x0;

    struct path_unit* path_unit_ptr = unit_head;
    char* current_unit = 0x0;

    while((current_unit = path_get_current_unit_and_advance(&__path)) != 0x0){
        struct path_unit* path_current_unit = (struct path_unit*) malloc(sizeof(struct path_unit));
        path_current_unit->unit_str = current_unit;
        path_current_unit->next_unit = 0x0;

        path_unit_ptr->next_unit = path_current_unit;
        path_unit_ptr = path_unit_ptr -> next_unit;
    }

    parsed_path->units = unit_head;



    return parsed_path;



}

