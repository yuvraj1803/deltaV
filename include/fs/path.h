/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */

#ifndef __PATH_H__
#define __PATH_H__


struct path_unit{
    char *unit_str; // each unit stored in string format.
    struct path_unit* next_unit;
};

// path of a file in the file system
struct path{

    struct path_unit* units; // first unit of the path

};



#endif

