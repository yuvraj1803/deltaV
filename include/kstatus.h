/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */

#ifndef __KSTATUS_H__
#define __KSTATUS_H__

enum ERRORS{

    SUCCESS = 0,

    ERR_INVARG,

    ERR_MALLOC_FAIL,

    ERR_INVPATH,

    ERR_TASK_FAIL,

    ERR_FS_FAIL,
    ERR_FS_INV_SIGN,
    ERR_FS_ROOT_NOT_FOUND,
    ERR_FS_INV_FILE_MODE,
    ERR_FS_FOPEN_FAILED,
    ERR_FS_FREAD_FAILED,
    ERR_FD_NOT_FOUND,

    ERR_VM_LOAD_FAIL,
    ERR_VM_VMAPPING_FAIL,

};

#endif

