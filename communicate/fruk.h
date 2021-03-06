/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : fruk.h
 * Author        : lb
 * Date          : 2014-09-23
 * Description   : Io log defintion 
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-23
 *   Author      : lb
 *   Modification: Created file. Copy from windows real time backup module. Changed some difintion.

*************************************************************************************************************/

/*++

Copyright (c) 1999-2002  Microsoft Corporation

Module Name:

    fruk.h

Abstract:

    Header file which contains the structures, type definitions,
    constants, global variables and function prototypes that are
    shared between kernel and user mode.

Environment:

    Kernel & user mode

--*/

#ifndef _FILEREPLUK_H
#define _FILEREPLUK_H



#pragma pack(1)


#ifndef MAX_PATH
#if defined(__linux__)
#define MAX_PATH	260
#else
#define MAX_PATH	260
#endif

#endif

#if defined(__linux__)
    #define ULONG unsigned long
    #define ULONGLONG unsigned long long
	#define ulonglong unsigned long long
    #define WCHAR char

    typedef struct _GUID {
        unsigned long  Data1;
        unsigned short Data2;
        unsigned short Data3;
        unsigned char  Data4[ 8 ];
    } GUID;

    #if defined(MIDL_PASS)
    typedef struct _LARGE_INTEGER {
    #else // MIDL_PASS
    typedef union _LARGE_INTEGER {
        struct {
            unsigned long LowPart;
            unsigned long HighPart;
        };
        struct {
            unsigned long LowPart;
            unsigned long HighPart;
        } u;
    #endif //MIDL_PASS
        unsigned long long QuadPart;
    } LARGE_INTEGER;
#endif

#define MIN_SECTOR_SIZE         	512
#define FILEREPL_READ_BUFFER_SIZE   1024*8      //不能小于1024*8
#define FILEREPL_MAX_PATH       	MAX_PATH

#define ALIGN_SIZE(s,a) \
            (s) += (a) - 1;\
            (s) /= (a);\
            (s) *= (a);

#define FILTER_TYPE_INVALID     0
#define FILTER_TYPE_FILE        1
#define FILTER_TYPE_DIR         2
#define FILTER_TYPE_SET         3


typedef struct _FILTER_ITEM_
{
    ULONG   ulFileType;   //0文件；1 目录； 
    WCHAR   wszFilterName[FILEREPL_MAX_PATH];
}FILTER_ITEM,  *PFILTER_ITEM;

typedef struct _REALTIME_BACKUP_DATA_
{
    ULONG   ulSize;                     //数据的总长度
    GUID    guidSetId;                  //一个备份集的ID（实例也可认为是一个集合）
    ULONG   ulFilterItemCounts;         //监控项总数
    WCHAR   wszBakCacheDir[FILEREPL_MAX_PATH];  //备份日志文件存放目录
    FILTER_ITEM FilterItems[1];  
}REALTIME_BACKUP_DATA;

typedef struct _FILEREPL_NOTIFICATION {

	ULONG Type;             // for quad-word alignement of the Contents structure
	ULONG bNormalRunning;   // driver's running status
	union
	{
        struct  
        {
            REALTIME_BACKUP_DATA     BackupData;

        } AddOrDel;
	};
    
} FILEREPL_NOTIFICATION, *PFILEREPL_NOTIFICATION;



typedef struct _FILEREPL_REPLY 
{
    ULONG RetVal;
    
} FILEREPL_REPLY, *PFILEREPL_REPLY;

// #define LOG_FILE_TMP_EXT            L"TMP"
#define LOG_FILE_EXT                L"LOG"

#define LOG_FILE_HEADER_SIZE        1024

#define LOG_FILE_TYPE_WRITE         1
#define LOG_FILE_TYPE_STOP_INIT     2  // 初始化停止标志
#define LOG_FILE_TYPE_ENDOFFILE     3  // 重设文件大小
#define LOG_FILE_TYPE_NEWFILE       4  // 新建文件，可能是覆盖原文件，也可能是替换原文件
#define LOG_FILE_TYPE_DELETEFILE    5  // 删除文件
#define LOG_FILE_TYPE_RENAMEFILE    6  // 文件改名
#define LOG_FILE_TYPE_MOVEIN        7  //监控外到监控内
#define LOG_FILE_TYPE_HARDLINK      8  // 创建硬链接
#define LOG_FILE_TYPE_SOFTLINK	    9  // 创建软链接

//
// IO日志文件头，该头结构固定长度LOG_FILE_HEADER_SIZE字节，头结构后是IO的文件数据
//
typedef struct _LOG_HEAD_ENDOFFILE_
{
    LARGE_INTEGER EndOfFile;   
}LOG_HEAD_ENDOFFILE;

typedef struct _LOG_FILE_HEADER_
{
    ULONG       ulHeaderSize;   // 头大小
    ULONG       ulType;         // 类型，以区别是正常写的IO,表明初始化终止，文件改名...
    ULONGLONG   ullSeqNo;       // 顺序号
	ULONGLONG   ullGolbalSeqNo;  // 全局顺序号
    ULONG       ulLogSize;       // 整个日志文件的大小
    ULONG       bIsDir;          // 是否目录
	WCHAR	    wszFilePath[FILEREPL_MAX_PATH];  // 监控文件名

    LARGE_INTEGER   liSystemTime;   // io的时间,UTC

    union
    {
        struct
        {
            ULONG Length;              // Length of transfer,文件数据的大小
            LARGE_INTEGER ByteOffset;  // Offset to write to,数据在文件中的位置

        } write;

        LOG_HEAD_ENDOFFILE endoffile;

        ULONG       ExtraData[1];        
    };

} LOG_FILE_HEADER, *PLOG_FILE_HEADER;

typedef struct _LOG_FILE_
{
    LOG_FILE_HEADER hdr;
    char            Reserved[LOG_FILE_HEADER_SIZE-sizeof(LOG_FILE_HEADER)];


    ULONG       Data[1];        //附加数据,比如改名的信息...  

} LOG_FILE, *PLOG_FILE;

#pragma pack()

#endif //  _FILEREPLUK_H