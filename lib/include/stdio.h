/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#ifndef __stdio_h__
#define __stdio_h__

void log(char* str);
void panic(char* str);
void printf(char* fmt, ...);
void gets(char* str); // doesnt check for buffer overflow
char getch();

#endif
