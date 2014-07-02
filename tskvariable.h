#ifndef TSKVARIABLE_H
#define TSKVARIABLE_H

#include "tsk/libtsk.h"
#include <QDebug>

struct TskObject
{
    const TSK_TCHAR** imagepartspath;
    int partcount;
    int offset;
    int length;
    int address;
    int objecttype;
    int type;
    TSK_IMG_INFO* readimginfo;
    TSK_FS_INFO* readfsinfo;
    TSK_FS_FILE* readfileinfo;
};

struct FileSystemObject
{
    int id;
    int rootinum;
};

#endif // TSKVARIABLE_H