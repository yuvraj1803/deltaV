/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */

#include "string.h"
#include <stddef.h>

// returns the length of a given string
uint32_t strlen(const char* str){
    uint32_t strpos = 0;
    while(str[strpos] != 0x00){
        strpos++;
    }

    return strpos;
}

// copies string from src to tgt
char* strcpy(char* tgt, const char* src){
    uint32_t tgt_ix = 0;
    uint32_t src_ix = 0;

    while(src[src_ix] != 0x00){
        tgt[tgt_ix++] = src[src_ix++];
    }
    tgt[tgt_ix] = 0x00; // mark the end of the copied string

    return tgt;

}

char tolower(char c){
    if(c >= 'a' && c <= 'z') return c; // already in lower case.

    return c + 32;
}



int strncmp(const char* s1, const char* s2, uint32_t n){
    while(n--){
        if(tolower(*s1) == tolower(*s2)){
            s1++;
            s2++;
        }else{
            if(*s1 < *s2) return -1;
            else return 1;
        }
    }

    return 0;
}

int strcmp(const char* s1, const char* s2){
    if(strlen(s1) != strlen(s2)) return 1;

    return strncmp(s1, s2, strlen(s1));
}

int strncpy(char* tgt, char* src, uint32_t len){
    int written = 0;

    int tgt_ix = 0;
    int src_ix = 0;

    for(written = 0;written < len;written++){
        if(src[src_ix] == 0x00) break;
        tgt[tgt_ix] = src[src_ix];
        tgt_ix++;
        src_ix++;
    }

    return written;

}
char *strchr(const char *s, int c)
{
        char *p = (char *)s;

        while (*p != '\0' && *p != c)
                p++;

        if (*p == '\0')
                return NULL;

        else
                return p;
}


